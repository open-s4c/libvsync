/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VQUEUE_UB_LF_RECYCLE_H
#define VQUEUE_UB_LF_RECYCLE_H
/*******************************************************************************
 * @file  unbounded_queue_lf_recycle.h
 * @brief Lock-free recycle unbounded queue.
 *
 * @ingroup lock_free linearizable unbounded_queue
 *
 * @note  Retired Nodes of this queue **cannot** be freed, but can be recycled
 * This queue makes sure that the occurrence of ABA problem is improbable, by
 * using the 7 LSB bits of nodes addresses as a stamp. `cas` can succeed iff the
 * stamps match.
 *
 * @note `vqueue_ub_node_t` objects must be 128byte aligned (i.e. the 7 LSB of
 * their addresses are always zeros). i.e. allocate using `memalign`
 *
 * @note maintain a thread local free node list that you place retired nodes on
 * and reuse them for new allocations, unless the list is empty
 *
 *
 * @example
 * @include eg_unbounded_queue_lf_recycle.c
 *
 * ### Operating conditions
 * - `vqueue_ub_node_t` objects must be 128byte aligned
 * - Nodes cannot be freed, they can only be recycled
 *
 * ### Recycling tips
 * - maintain local pool per thread
 * - when a thread retires a node, add the node to the local pool
 * - when a thread wants to allocate a new node fetch a node from the local
 *  unless it is not empty. If it is empty, allocate it with your normal
 *  allocator e.g. memalign
 *
 * @note the above suggestions works well for threads that perform roughly the
 * same number of enqueues and dequeues. If that is not the case, a thread
 * needs a strategy of stealing nodes from other pools.
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 10.6]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/

#include <vsync/vtypes.h>
#include <vsync/atomic.h>
#include <vsync/atomic/atomicptr_stamped.h>
#include <vsync/common/verify.h>
#include <vsync/common/assert.h>
#include <vsync/queue/internal/ub/vqueue_ub_common.h>

// alignment in bytes
#define RECYCLE_QUEUE_NODE_ALIGNMENT V_ATOMICPTR_STAMPED_REQUIRED_ALIGNMENT

typedef struct vqueue_ub_node_s {
    void *data;
    vatomicptr_stamped(struct vqueue_ub_node_s *) next;
} __attribute__((aligned(RECYCLE_QUEUE_NODE_ALIGNMENT))) vqueue_ub_node_t;

typedef struct vqueue_ub_s {
    vatomicptr_stamped(vqueue_ub_node_t *) head;
    vatomicptr_stamped(vqueue_ub_node_t *) tail;
    vqueue_ub_node_t sentinel;
} vqueue_ub_t;

/**
 * Initialize the queue.
 *
 * @note Function must be called before threads start operating on the queue.
 *
 * @param q address of vqueue_ub_t object
 */
static inline void
vqueue_ub_init(vqueue_ub_t *q)
{
    ASSERT(q);
    vatomicptr_stamped_set_rlx(&q->sentinel.next, NULL, 0);
    vatomicptr_stamped_set_rlx(&q->head, &q->sentinel, 0);
    vatomicptr_stamped_set_rlx(&q->tail, &q->sentinel, 0);
}

/**
 * Destroys all remaining nodes in the queue.
 *
 * @note call only after all threads finished operating on the queue.
 * @note NOT THREAD-SAFE.
 *
 * @param q address of vqueue_ub_t object
 * @param retire function pointer of the callback that handles recycling/freeing
 * the remaining nodes.
 * @param arg extra parameter passed to `retire`.
 */
static inline void
vqueue_ub_destroy(vqueue_ub_t *q, vqueue_ub_node_handler_t retire, void *arg)
{
    vqueue_ub_node_t *next = NULL;
    vqueue_ub_node_t *curr = NULL;

    curr = vatomicptr_stamped_get_pointer(&q->head);

    while (curr) {
        next = vatomicptr_stamped_get_pointer(&curr->next);
        if (curr != &q->sentinel) {
            retire(curr, arg);
        }
        curr = next;
    }
}
/**
 * Enqueue the given node `qnode` in the given queue `q`.
 *
 * @param q address of vqueue_ub_t object
 * @param qnode address of vqueue_ub_node_t
 * @param data address of data object to enqueue.
 */
static inline void
vqueue_ub_enq(vqueue_ub_t *q, vqueue_ub_node_t *qnode, void *data)
{
    vqueue_ub_node_t *last = NULL;
    vqueue_ub_node_t *next = NULL;
    vuint8_t l_stamp       = 0;
    vuint8_t next_stamp    = 0;
    vuint8_t t_stamp       = 0;
    vuint8_t stamp         = 0;
    vbool_t cas_succeeded  = false;

    ASSERT(q);
    ASSERT(qnode);
    qnode->data = data;

#if !defined(VSYNC_VERIFICATION)
    // We skip this read for verification, because it will fail on reading
    // uninitialized memory. Which is true when qnode is freshly allocation and
    // not recycled. Nevertheless, we know in reality the trashy stamp value
    // will not be an issue.
    vatomicptr_stamped_get(&qnode->next, &stamp);
#endif
    // Because qnode might be recycled, we don't want to reset the stamp to
    // not increase ABA chances. So we keep the stamp value as is.
    // If it is used for the first time it will still be ok to have a trashy
    // stamp, we can start the count from any place that is not zero.
    vatomicptr_stamped_set(&qnode->next, NULL, stamp);

    while (true) {
        last = vatomicptr_stamped_get(&q->tail, &l_stamp);
        next = vatomicptr_stamped_get(&last->next, &next_stamp);

        if (last == vatomicptr_stamped_get(&q->tail, &t_stamp) &&
            l_stamp == t_stamp) {
            // check if the node has a successor
            if (next == NULL) {
                // append the new node
                // tail->next = n
                if (vatomicptr_stamped_cmpxchg(&last->next, next, next_stamp,
                                               qnode, (next_stamp + 1))) {
                    // advance the tail to the new node.
                    // tail = n
                    // if this cas succeeds it means
                    // we managed to advance the tail.
                    // if it fails, then someone else
                    // already helped advancing the tail
                    // either way it is a success.
                    /* LP */
                    cas_succeeded = vatomicptr_stamped_cmpxchg(
                        &q->tail, last, l_stamp, qnode, (l_stamp + 1));
                    (void)cas_succeeded;
                    verification_assume(cas_succeeded);
                    return;
                }
            } else {
                // there is a successor
                // we help advance the tail to the successor
                // someone else is in the process of attaching
                // its node.
                /* LP for the thread that I am helping */
                verification_ignore();
                vatomicptr_stamped_cmpxchg(&q->tail, last, l_stamp, next,
                                           (l_stamp + 1));
            }
        }
    }
}
/**
 * Check if the queue is empty.
 *
 * @note Use function with caution. It is subject to ABA problems and might
 * return empty when the queue is not empty.

 * @param q address of vqueue_ub_t object
 * @return true the queue is empty
 * @return false the queue is not empty
 */
static inline vbool_t
vqueue_ub_empty(vqueue_ub_t *q)
{
    vqueue_ub_node_t *first = NULL;
    vqueue_ub_node_t *last  = NULL;
    ASSERT(q);

    first = (vqueue_ub_node_t *)vatomicptr_stamped_get_pointer(&q->head);
    last  = (vqueue_ub_node_t *)vatomicptr_stamped_get_pointer(&q->tail);
    return (first == last);
}

/**
 * Dequeue a node from the given queue `q`.
 *
 * @param q address of vqueue_ub_t object
 * @param retire function pointer of the callback that recycles a detached
 * `vqueue_ub_node_t`.
 * @param arg extra parameter passed to `retire`.
 * @return NULL when the queue is empty.
 * @return a non NULL address of the dequeued data object.
 */
static inline void *
vqueue_ub_deq(vqueue_ub_t *q, vqueue_ub_node_handler_t retire, void *arg)
{
    vqueue_ub_node_t *first = NULL;
    vqueue_ub_node_t *last  = NULL;
    vqueue_ub_node_t *next  = NULL;
    vuint8_t f_stamp        = 0;
    vuint8_t l_stamp        = 0;
    vuint8_t n_stamp        = 0;
    vuint8_t t_stamp        = 0;
    void *data              = NULL;

    while (true) {
        first = vatomicptr_stamped_get(&q->head, &f_stamp);
        last  = vatomicptr_stamped_get(&q->tail, &l_stamp);
        next  = vatomicptr_stamped_get(&first->next, &n_stamp);

        /* check things are still consistent with what we read above */
        if (first == vatomicptr_stamped_get(&q->head, &t_stamp) &&
            t_stamp == f_stamp) {
            if (first == last) {
                /* queue is empty */
                if (next == NULL) {
                    /* LP */
                    return NULL;
                }
                verification_ignore();
                /* Help current enqueuer by advancing tail, tail = tail->next */
                vatomicptr_stamped_cmpxchg(&q->tail, last, l_stamp, next,
                                           (l_stamp + 1));
            } else {
                /* we extract data before dequeuing, because the node next is
                 * going to be a sentinel and some other thread might
                 * recycle/free before we read it, if we move this statement
                 * after the cas */
                data = next->data;
                /* LP */
                if (vatomicptr_stamped_cmpxchg(&q->head, first, f_stamp, next,
                                               (f_stamp + 1))) {
                    if (first != &q->sentinel) {
                        retire(first, arg);
                    }
                    return data;
                }
            }
        } // if consistent values
    }     // while
}
/**
 * Iterate through the queue nodes and count them.
 *
 * @note returns an atomic snapshot of the length, which may be stale
 *
 * @param q address of vqueue_ub_t object
 * @return vsize_t the length of the queue
 */
static inline vsize_t
vqueue_ub_get_length(vqueue_ub_t *q)
{
    vsize_t count              = 0;
    vqueue_ub_node_t *head     = NULL;
    vqueue_ub_node_t *tail     = NULL;
    vqueue_ub_node_t *curr     = NULL;
    vbool_t is_sentinel_behind = false;

    ASSERT(q);

    head = vatomicptr_stamped_get_pointer(&q->head);
    tail = vatomicptr_stamped_get_pointer(&q->tail);

    ASSERT(head && "head should always point to a sentinel");

    curr = head;

    while (curr != tail) {
        count++;
        ASSERT(!is_sentinel_behind || curr != &q->sentinel);
        curr               = vatomicptr_stamped_get_pointer(&curr->next);
        is_sentinel_behind = true;
    }

    V_UNUSED(is_sentinel_behind);
    return count;
}
/**
 * Visit all available nodes in the queue.
 *
 * @note NOT-THREAD-SAFE.
 * @param q address of vqueue_ub_t object.
 * @param visitor function pointer of visitor function.
 * @param arg extra argument passed to visitor function if needed.
 */
static inline void
_vqueue_ub_visit_nodes(vqueue_ub_t *q, vqueue_ub_node_handler_t visitor,
                       void *arg)
{
    vqueue_ub_node_t *next = NULL;
    vqueue_ub_node_t *curr = NULL;

    ASSERT(visitor);

    curr = vatomicptr_stamped_get_pointer(&q->head);
    ASSERT(curr);
    // skip the sentinel
    curr = vatomicptr_stamped_get_pointer(&curr->next);

    while (curr) {
        next = vatomicptr_stamped_get_pointer(&curr->next);
        visitor(curr, arg);
        curr = next;
    }
}

#endif
