/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VQUEUE_PRIO_HEAP_BASED_H
#define VQUEUE_PRIO_HEAP_BASED_H
/*******************************************************************************
 * @file vqueue_prio_heap_based.h
 * @brief Unbounded heap-based priority queue
 * @ingroup linearizable
 *
 * This is a fine-grained blocking implementation, which uses rec_spinlock.h.
 * It has an unbounded priority range, and bounded capacity.
 *
 * @example
 * @include eg_queue_prio_heap.c
 *
 * @note If you wish to use this implementation in user-space. Contact
 * the authors for recursive `pthread_mutex` support.
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 15.4]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/
#include <vsync/vtypes.h>
#include <vsync/common/assert.h>
#include <vsync/common/verify.h>
#include <vsync/common/dbg.h>
#include <vsync/common/compiler.h>
#include <vsync/common/assert.h>
#include <vsync/utils/math.h>
#include <vsync/queue/internal/vqueue_prio_lock.h>

#if !defined(VQUEUE_PRIO_HEAP_CAPACITY)
    #if defined(VSYNC_VERIFICATION)
        #define VQUEUE_PRIO_HEAP_CAPACITY 4U
    #else
        /**
         * `VQUEUE_PRIO_HEAP_CAPACITY` is equal to `1024` by default. Users may
         * overwrite it by defining `VQUEUE_PRIO_HEAP_CAPACITY=N`.
         * This value determines how many nodes can be stored in the
         * queue.
         *
         * @note `VQUEUE_PRIO_HEAP_CAPACITY` must be power of two.
         */
        #define VQUEUE_PRIO_HEAP_CAPACITY 1024U
    #endif
#endif

#define VQUEUE_PRIO_HEAP_LEN         (VQUEUE_PRIO_HEAP_CAPACITY + 1U)
#define VQUEUE_PRIO_ROOT             1U
#define VQUEUE_PRIO_NO_ONE           VUINT32_MAX
#define VQUEUE_PRIO_HEAP_LCK_IDX     0U
#define VQUEUE_PRIO_CALC_LCHILD(_p_) ((_p_)*2U)
#define VQUEUE_PRIO_CALC_RCHILD(_p_) (((_p_)*2U) + 1U)
#define VQUEUE_PRIO_CALC_PARENT(_c_) ((_c_) / 2U)
#define VQUEUE_PRIO_MID_IDX          (VQUEUE_PRIO_HEAP_CAPACITY / 2U)

typedef enum vqueue_prio_node_status_e {
    NODE_TAG_EMPTY,     /* the node is not in use */
    NODE_TAG_AVAILABLE, /* the node holds data and priority */
    NODE_TAG_BUSY /* the node is being percolated up tree, and is not yet in
                        its proper position */
} vqueue_prio_node_tag_t;

typedef struct vqueue_prio_heap_node_s {
    vqueue_prio_lock_t lock;
    vqueue_prio_node_tag_t tag;
    vuint32_t score;
    vuint32_t owner; /* while the nodes' tag is busy, the owner field holds
                        the ID of the thread responsible for moving it */
    void *data;
} vqueue_prio_node_t;

typedef vuint32_t (*vqueue_prio_fun_get_tid)(void);
typedef void (*vqueue_prio_handle_node_t)(void *, void *);

typedef struct vqueue_prio_s {
    vsize_t next;                  /* the index of the first unused node */
    vqueue_prio_lock_t *heap_lock; /* heap lock */
    vqueue_prio_node_t
        heap[VQUEUE_PRIO_HEAP_LEN]; /* the binary heap is represented by an
                           array of nodes. The tree VQUEUE_PRIO_ROOT resides at
                           index 1, the right and left children of array entry i
                           are entries 2*i and (2*i) +1 respectively */

    vqueue_prio_fun_get_tid get_tid_fun;
} vqueue_prio_t;

/**
 * Internal functions
 */
static inline void _vqueue_prio_lock(vqueue_prio_t *pqueue, vsize_t idx);
static inline void _vqueue_prio_unlock(vqueue_prio_t *pqueue, vsize_t idx);
static inline void _vqueue_prio_node_init(vqueue_prio_node_t *node);
static inline void _vqueue_prio_node_swap(vqueue_prio_node_t *a,
                                          vqueue_prio_node_t *b);
static inline vbool_t _vqueue_prio_node_am_owner(vqueue_prio_t *pqueue,
                                                 vsize_t idx);
static inline void _vqueue_prio_node_occupy(vqueue_prio_t *pqueue, vsize_t idx,
                                            void *data, vuint32_t score);

/**
 * Initializes the given priority queue object `pqueue`.
 *
 * @param get_tid_fun function pointer that returns the calling thread id
 * `vuint32_t`.
 * @param pqueue address of vqueue_prio_t object.
 */
static inline void
vqueue_prio_init(vqueue_prio_t *pqueue, vqueue_prio_fun_get_tid get_tid_fun)
{
    vsize_t i = 0;

    V_STATIC_ASSERT(V_IS_POWER_OF_TWO(VQUEUE_PRIO_HEAP_CAPACITY),
                    "VQUEUE_PRIO_HEAP_CAPACITY should be power of two");

    /* note that insertion starts at index 1 which is equal to VQUEUE_PRIO_ROOT
     */
    pqueue->next = VQUEUE_PRIO_ROOT;

    /* initialize all nodes and their locks  */
    for (i = 0; i < VQUEUE_PRIO_HEAP_LEN; i++) {
        _vqueue_prio_node_init(&pqueue->heap[i]);
    }

    ASSERT(get_tid_fun);

    /* since first node is never used, we use its lock as a heap lock */
    pqueue->heap_lock   = &pqueue->heap[0].lock;
    pqueue->get_tid_fun = get_tid_fun;
}

/**
 * Destroys all remaining enqueued objects.
 *
 * @note this is not thread safe and must be called iff all threads are done
 * accessing the queue.
 *
 * @param pqueue address of vqueue_prio_t object.
 * @param destroy_cb address of a callback function to call on each queue
 * object to destroy.
 * @param args second argument of `destroy_cb`.
 */
static inline void
vqueue_prio_destroy(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t destroy_cb,
                    void *args)
{
    vsize_t i = 0;

    if (destroy_cb) {
        for (i = 1; i < VQUEUE_PRIO_HEAP_LEN; i++) {
            if (pqueue->heap[i].tag != NODE_TAG_EMPTY) {
                destroy_cb(pqueue->heap[i].data, args);
            }
        }
    }
    /* pqueue_destroy all locks */
    for (i = 0; i < VQUEUE_PRIO_HEAP_LEN; i++) {
        vqueue_prio_lock_destroy(&pqueue->heap[i].lock);
    }
}
/**
 * Inserts the given object `data` into the heap.
 *
 * @param pqueue address of vqueue_prio_t object.
 * @param priority priority of the given object.
 * @param data address of the object to enqueue.
 * @return true insertion succeeded.
 * @return false insertion failed the heap is full.
 */
static inline vbool_t
vqueue_prio_add(vqueue_prio_t *pqueue, void *data, vsize_t priority)
{
    vsize_t old_child        = 0;
    vsize_t child            = 0;
    vsize_t parent           = 0;
    vqueue_prio_node_t *heap = &pqueue->heap[0];

    /* acquire the heap lock */
    _vqueue_prio_lock(pqueue, VQUEUE_PRIO_HEAP_LCK_IDX);

    if (pqueue->next >= VQUEUE_PRIO_HEAP_LEN) {
        /* the heap is full we cannot insert more items */
        _vqueue_prio_unlock(pqueue, VQUEUE_PRIO_HEAP_LCK_IDX);
        return false;
    }

    /* find where you can insert */
    child = pqueue->next++;

    /* acquire the lock of the child */
    _vqueue_prio_lock(pqueue, child);

    /* own the child node, mark it as busy and fill in data and priority */
    ASSERT(priority <= VUINT32_MAX && "unsound conversion");
    _vqueue_prio_node_occupy(pqueue, child, data, (vuint32_t)priority);

    /* unlock the heap */
    _vqueue_prio_unlock(pqueue, VQUEUE_PRIO_HEAP_LCK_IDX);

    /* unlock the child */
    _vqueue_prio_unlock(pqueue, child);

    /* percolate child up the tree to find the right position */
    while (child > VQUEUE_PRIO_ROOT) {
        parent = VQUEUE_PRIO_CALC_PARENT(child);
        _vqueue_prio_lock(pqueue, parent);
        _vqueue_prio_lock(pqueue, child);
        old_child = child;

        /* if the parent is available and the child is owned by the caller */
        if (heap[parent].tag == NODE_TAG_AVAILABLE &&
            _vqueue_prio_node_am_owner(pqueue, child)) {
            if (heap[child].score < heap[parent].score) {
                /* if the child has higher priority than the parent then we swap
                 * the child with its parent */
                _vqueue_prio_node_swap(&heap[child], &heap[parent]);
                /* move up */
                child = parent;
            } else {
                /* if the child has the right position already, i.e. having a
                 * priority lower than the parent. Then we are done. We make the
                 * child available now and release ownership. */
                heap[child].tag   = NODE_TAG_AVAILABLE;
                heap[child].owner = VQUEUE_PRIO_NO_ONE;
                _vqueue_prio_unlock(pqueue, old_child);
                _vqueue_prio_unlock(pqueue, parent);
                return true;
            }
        } else if (!_vqueue_prio_node_am_owner(pqueue, child)) {
            /* if the child is not owned by the caller, then the node must have
             * been moved up by a concurrent removeMin() call, so we move up the
             * tree to search for our node */
            child = parent;
        } else {
            verification_ignore();
        }

        _vqueue_prio_unlock(pqueue, old_child);
        _vqueue_prio_unlock(pqueue, parent);
    } /* as long as we did not read the VQUEUE_PRIO_ROOT */

    /* if we are at VQUEUE_PRIO_ROOT */
    if (child == VQUEUE_PRIO_ROOT) {
        _vqueue_prio_lock(pqueue, VQUEUE_PRIO_ROOT);
        if (_vqueue_prio_node_am_owner(pqueue, VQUEUE_PRIO_ROOT)) {
            heap[VQUEUE_PRIO_ROOT].tag = NODE_TAG_AVAILABLE;
            heap[child].owner          = VQUEUE_PRIO_NO_ONE;
        }
        _vqueue_prio_unlock(pqueue, VQUEUE_PRIO_ROOT);
    }
    return true;
}
/**
 * Retrieves an object with the highest available priority.
 *
 * @param pqueue address of vqueue_prio_t object.
 * @return void* address of the retrieved object.
 */
static inline void *
vqueue_prio_remove_min(vqueue_prio_t *pqueue)
{
    vsize_t bottom           = 0;
    vsize_t child            = 0;
    vsize_t left             = 0;
    vsize_t right            = 0;
    vsize_t parent           = 0;
    vqueue_prio_node_t *heap = NULL;
    void *data               = NULL;

    ASSERT(pqueue);

    heap = &pqueue->heap[0];

    /* acquire heap lock */
    _vqueue_prio_lock(pqueue, VQUEUE_PRIO_HEAP_LCK_IDX);

    /* handle empty heap */
    if (pqueue->next == VQUEUE_PRIO_ROOT) {
        /* heap is empty release heap lock and return */
        _vqueue_prio_unlock(pqueue, VQUEUE_PRIO_HEAP_LCK_IDX);
        return NULL;
    }

    ASSERT(pqueue->next > VQUEUE_PRIO_ROOT);

    /* find the index of leaf node to replace the node you are about to consume
     */
    bottom = --pqueue->next;

    /* acquire the lock of the VQUEUE_PRIO_ROOT node */
    _vqueue_prio_lock(pqueue, VQUEUE_PRIO_ROOT);

    /* lock the bottom item  */
    _vqueue_prio_lock(pqueue, bottom);

    /* release heap lock */
    _vqueue_prio_unlock(pqueue, VQUEUE_PRIO_HEAP_LCK_IDX);

    /* if there is nothing to consume at the VQUEUE_PRIO_ROOT return */
    if (heap[VQUEUE_PRIO_ROOT].tag == NODE_TAG_EMPTY) {
        _vqueue_prio_unlock(pqueue, VQUEUE_PRIO_ROOT);
        _vqueue_prio_unlock(pqueue, bottom);
        return NULL;
    }

    /* VQUEUE_PRIO_ROOT is not empty, we will consume it */
    data                         = heap[VQUEUE_PRIO_ROOT].data;
    heap[VQUEUE_PRIO_ROOT].tag   = NODE_TAG_EMPTY;
    heap[VQUEUE_PRIO_ROOT].owner = VQUEUE_PRIO_NO_ONE;

    /* TODO: check if the following is valid
     * this problem is not handled in the original algorithm
     * it is possible that an item gets added with busy tag
     * unlocked, before it is resorted to settle in the correct location
     * meanwhile remove swaps it, before its tag is set to available.
     * this is a quick-fix that need to be thoroughly tested/verified
     */
    if (heap[bottom].tag == NODE_TAG_BUSY &&
        heap[bottom].owner != VQUEUE_PRIO_NO_ONE) {
        heap[bottom].tag   = NODE_TAG_AVAILABLE;
        heap[bottom].owner = VQUEUE_PRIO_NO_ONE;
    }

    /* we swap bottom with VQUEUE_PRIO_ROOT */
    _vqueue_prio_node_swap(&heap[bottom], &heap[VQUEUE_PRIO_ROOT]);

    /* bottom is now empty we release its lock */
    _vqueue_prio_unlock(pqueue, bottom);

    /* if the VQUEUE_PRIO_ROOT is empty (bottom/leaf is the same as
     * VQUEUE_PRIO_ROOT) after swapping then we are done as there are no other
     * nodes in the heap */
    if (heap[VQUEUE_PRIO_ROOT].tag == NODE_TAG_EMPTY) {
        _vqueue_prio_unlock(pqueue, VQUEUE_PRIO_ROOT);
        return data;
    }

    /* The node in the VQUEUE_PRIO_ROOT might not be in the right position we
     * need to percolate it down the tree until it reaches its proper position
     */
    parent = VQUEUE_PRIO_ROOT;

    while (parent < VQUEUE_PRIO_MID_IDX) {
        left  = VQUEUE_PRIO_CALC_LCHILD(parent);
        right = VQUEUE_PRIO_CALC_RCHILD(parent);

        ASSERT(left < VQUEUE_PRIO_HEAP_CAPACITY);
        ASSERT(right < VQUEUE_PRIO_HEAP_CAPACITY);

        _vqueue_prio_lock(pqueue, left);
        _vqueue_prio_lock(pqueue, right);

        if (heap[left].tag == NODE_TAG_EMPTY) {
            /* if the left is empty, we unlock both children and return */
            _vqueue_prio_unlock(pqueue, right);
            _vqueue_prio_unlock(pqueue, left);
            break;
        } else if (heap[right].tag == NODE_TAG_EMPTY ||
                   heap[left].score < heap[right].score) {
            /* if the right child is empty or the left child has higher
             * priority, then we can release the right. Left is the candidate
             * for swap */
            _vqueue_prio_unlock(pqueue, right);
            child = left;
        } else {
            /* the right child is not empty and has a higher priority than the
             * left, we can release the left. Right is the candidate for swap
             */
            _vqueue_prio_unlock(pqueue, left);
            child = right;
        }
        /* we check if the child need to be swapped with its parent, i.e. has
         * higher priority */
        if (heap[child].score < heap[parent].score &&
            heap[child].tag != NODE_TAG_EMPTY) {
            _vqueue_prio_node_swap(&heap[parent], &heap[child]);
            ASSERT(heap[child].score >= heap[parent].score);
            /* we release the parent */
            _vqueue_prio_unlock(pqueue, parent);
            /* we process the child as the new parent */
            parent = child;
        } else {
            ASSERT(heap[child].score >= heap[parent].score);
            /* the parent has the correct position no need to swap we are done
             */
            _vqueue_prio_unlock(pqueue, child);
            break;
        }
    }

    _vqueue_prio_unlock(pqueue, parent);
    return data;
}

/**
 * Returns true iff the node's tag is BUSY and the owner is the calling
 * thread.
 *
 * @note a node can only be owned when its tag is busy
 * @param idx node index
 *
 * @return true the calling thread is the owner of the given node
 * @return false the calling thread is not the owner of the given node
 */
static inline vbool_t
_vqueue_prio_node_am_owner(vqueue_prio_t *pqueue, vsize_t idx)
{
    ASSERT(idx >= VQUEUE_PRIO_ROOT);
    ASSERT(idx < VQUEUE_PRIO_HEAP_LEN);
    if (pqueue->heap[idx].tag == NODE_TAG_BUSY &&
        pqueue->heap[idx].owner == pqueue->get_tid_fun()) {
        return true;
    }
    return false;
}
/**
 * Swaps the content of the two given nodes `a` and `b`.
 *
 * @note The lock field is not among the swapped fields.
 *
 * @param a address of vqueue_prio_node_t object
 * @param b address of vqueue_prio_node_t object
 */
static inline void
_vqueue_prio_node_swap(vqueue_prio_node_t *a, vqueue_prio_node_t *b)
{
    vuint32_t owner            = 0;
    vuint32_t score            = 0;
    vqueue_prio_node_tag_t tag = 0;
    void *data                 = NULL;

    /* copy a to tmp vars */
    score = a->score;
    owner = a->owner;
    tag   = a->tag;
    data  = a->data;

    /* copy b to a */
    a->score = b->score;
    a->owner = b->owner;
    a->tag   = b->tag;
    a->data  = b->data;

    /* tmp var to a */
    b->score = score;
    b->owner = owner;
    b->tag   = tag;
    b->data  = data;
}
/**
 * Occupies/reserves the given `node`, by changing its status to busy
 * and sets its data and priority fields to the given values.
 *
 * @param idx node index
 * @param data
 * @param priority
 */
static inline void
_vqueue_prio_node_occupy(vqueue_prio_t *pqueue, vsize_t idx, void *data,
                         vuint32_t priority)
{
    ASSERT(idx >= VQUEUE_PRIO_ROOT);
    ASSERT(idx < VQUEUE_PRIO_HEAP_LEN);
    pqueue->heap[idx].score = priority;
    pqueue->heap[idx].data  = data;
    pqueue->heap[idx].owner = pqueue->get_tid_fun();
    pqueue->heap[idx].tag   = NODE_TAG_BUSY;
}

/**
 * Initializes the given `node`.
 *
 * @param node
 */
static inline void
_vqueue_prio_node_init(vqueue_prio_node_t *node)
{
    ASSERT(node);
    node->tag   = NODE_TAG_EMPTY;
    node->owner = VQUEUE_PRIO_NO_ONE;
    node->data  = NULL;
    vqueue_prio_lock_init(&node->lock);
}

/**
 * Calls `visitor` function on all available nodes in the queue.
 *
 * @note this function is not thread-safe and does not acquire any locks prior
 * to reading the heap content. It is there for testing reasons only and should
 * not be called while more than one thread is accessing the heap.
 *
 * @param pqueue address of vqueue_prio_t object.
 * @param visitor callback function to call on available nodes.
 * @param arg second parameter of `visitor`.
 */
static inline void
_vqueue_prio_visit(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t visitor,
                   void *arg)
{
    vsize_t i     = 0;
    vsize_t left  = 0;
    vsize_t right = 0;

    for (i = VQUEUE_PRIO_ROOT; i < VQUEUE_PRIO_HEAP_LEN; i++) {
        ASSERT(pqueue->heap[i].tag != NODE_TAG_BUSY);
        ASSERT(pqueue->heap[i].owner == VQUEUE_PRIO_NO_ONE);

        left  = VQUEUE_PRIO_CALC_LCHILD(i);
        right = VQUEUE_PRIO_CALC_RCHILD(i);

        if (left < VQUEUE_PRIO_HEAP_CAPACITY &&
            right < VQUEUE_PRIO_HEAP_CAPACITY) {
            if (pqueue->heap[left].tag == NODE_TAG_AVAILABLE) {
                ASSERT(pqueue->heap[left].score >= pqueue->heap[i].score);
                if (pqueue->heap[right].tag == NODE_TAG_AVAILABLE) {
                    ASSERT(pqueue->heap[right].score >= pqueue->heap[i].score);
                }
            } else {
                ASSERT(pqueue->heap[right].tag == NODE_TAG_EMPTY);
            }
        }

        if (pqueue->heap[i].tag == NODE_TAG_AVAILABLE) {
            visitor((vqueue_prio_node_t *)pqueue->heap[i].data, arg);
        }

        const char *status = pqueue->heap[i].tag == NODE_TAG_AVAILABLE ?
                                 "avail" :
                             pqueue->heap[i].tag == NODE_TAG_EMPTY ? "empty" :
                                                                     "busy";
        V_UNUSED(status);

        DBG_GREEN("[%zu] Tag: %s Priority:%u ", i, status,
                  pqueue->heap[i].score);
    }
}
/* active only during testing */
#ifdef VQUEUE_PRIO_TESTING
    /* ensure locks are always acquired in order and no dead lock can happen
     */
    #ifdef VSYNC_VERIFICATION
        /* because genmc is buggy we need a work around
         * #genmc_bug https://github.com/MPI-SWS/genmc/issues/49
         * +2 because ids start from 1, and we also want to count for main
         * thread
         */

        #define NUM_THREADS (NTHREADS + 2)
int locked_child[VQUEUE_PRIO_HEAP_LEN][NUM_THREADS];
    #else
__thread int locked_child[VQUEUE_PRIO_HEAP_LEN];
    #endif
#endif

/**
 * Acquires the lock associated with the given index.
 *
 * @param pqueue address of vqueue_prio_t object
 * @param idx lock index
 */
static inline void
_vqueue_prio_lock(vqueue_prio_t *pqueue, vsize_t idx)
{
    vuint32_t tid = pqueue->get_tid_fun();
    ASSERT(idx < VQUEUE_PRIO_HEAP_LEN);

#ifdef VQUEUE_PRIO_TESTING
    #ifdef VSYNC_VERIFICATION
    ASSERT(tid < NUM_THREADS);
    for (vsize_t i = idx + 1; i < VQUEUE_PRIO_HEAP_LEN; i++) {
        ASSERT(!locked_child[i][tid]);
    }
    locked_child[idx][tid]++;
    #else
    for (vsize_t i = idx + 1; i < VQUEUE_PRIO_HEAP_LEN; i++) {
        ASSERT(!locked_child[i]);
    }
    locked_child[idx]++;
    #endif
#endif

    vqueue_prio_lock_acquire(&pqueue->heap[idx].lock, tid);
}

/**
 * Releases the lock associated with the given index.
 *
 * @param pqueue address of vqueue_prio_t object
 * @param idx lock index
 */
static inline void
_vqueue_prio_unlock(vqueue_prio_t *pqueue, vsize_t idx)
{
    vuint32_t tid = pqueue->get_tid_fun();
    ASSERT(idx < VQUEUE_PRIO_HEAP_LEN);

#ifdef VQUEUE_PRIO_TESTING
    #ifdef VSYNC_VERIFICATION
    ASSERT(tid < NUM_THREADS);
    locked_child[idx][tid]--;
    #else
    locked_child[idx]--;
    #endif
#endif

    vqueue_prio_lock_release(&pqueue->heap[idx].lock, tid);
}

#undef VQUEUE_PRIO_MID_IDX
#undef VQUEUE_PRIO_CALC_LCHILD
#undef VQUEUE_PRIO_CALC_RCHILD
#undef VQUEUE_PRIO_CALC_PARENT
#undef VQUEUE_PRIO_HEAP_LEN
#undef VQUEUE_PRIO_ROOT
#undef VQUEUE_PRIO_NO_ONE
#undef VQUEUE_PRIO_HEAP_LCK_IDX
#endif
