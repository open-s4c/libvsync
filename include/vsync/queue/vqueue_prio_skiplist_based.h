/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VQUEUE_PRIO_SKIPLIST_BASED_H
#define VQUEUE_PRIO_SKIPLIST_BASED_H
/*******************************************************************************
 * @file vqueue_prio_skiplist_based.h
 * @brief Skiplist-based priority queue implementation.
 * @ingroup requires_smr linearizable lock_free unbounded_queue
 *
 * This implementation uses a lock-free skiplist (see skiplist_lf.h) to
 * implement the priority queue. It has unbounded priority range and unbounded
 * capacity.
 *
 * It requires SMR (see gdump.h)
 *
 * ### Configuration
 * Check skiplist_lf.h for configuring the number of levels of the
 * skiplist.
 *
 *
 * @example
 * @include eg_queue_prio_skiplist.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 15.5]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/
#include <vsync/vtypes.h>
#include <vsync/common/compiler.h>
#include <vsync/map/skiplist_lf.h>

typedef struct vqueue_prio_node_s {
    vskiplist_key_t priority;
    vatomic8_t marked;
    vskiplist_node_t skip_node; /* keep last contains VLA field */
} vqueue_prio_node_t;

typedef vskiplist_node_t vqueue_prio_sentinel_t;

typedef void (*vqueue_prio_handle_node_t)(vqueue_prio_node_t *node, void *arg);

typedef struct vqueue_prio_s {
    vskiplist_t lst;
    vqueue_prio_handle_node_t retire_cb;
    void *retire_cb_arg;
} vqueue_prio_t;

typedef struct vqueue_destroy_cb_param_s {
    vqueue_prio_handle_node_t destroy_cb;
    void *destroy_cb_arg;
} vqueue_destroy_cb_param_t;

#define VQUEUE_PRIO_SENTINEL_SZ VSKIPLIST_SENTINEL_SZ

/**
 * Internal functions
 */
static inline vqueue_prio_node_t *
_vqueue_prio_find_and_mark_min(vqueue_prio_t *pqueue);
/**
 * Internal callback functions
 */
static inline int _vqueue_prio_cmp_priority(vskiplist_node_t *,
                                            vskiplist_key_t);
static inline void _vqueue_prio_visit_node(vskiplist_node_t *, vbool_t, void *);

static inline void _vqueue_prio_destroy_node(vskiplist_node_t *node,
                                             vbool_t marked, void *args);

static inline void _vqueue_prio_redirect_retire_cb(vskiplist_node_t *, void *);

/**
 * Initializes the queue.
 *
 * @param pqueue address of vqueue_prio_t object.
 * @param retire_cb callback function used for retiring detached nodes to the
 * SMR.
 * @param retire_cb_arg second parameter of `retire_cb`.
 * @param rand_seed passed down to the skiplist for random height generation.
 *
 * @param head the address of the dynamically allocated head sentinel object of
 * size `VQUEUE_PRIO_SENTINEL_SZ`.
 * @param tail the address of the dynamically allocated tail sentinel object of
 * size `VQUEUE_PRIO_SENTINEL_SZ`.
 *
 * @note both `head` and `tail` must have size `VQUEUE_PRIO_SENTINEL_SZ`.
 * @note `head` and `tail` must never be modified directly by the user, and they
 * must be allocated before `vqueue_prio_init`. They can be freed only after
 * `vqueue_prio_destroy`. i.e. they must have a life span larger or equal to
 * `pqueue`.
 */
static inline void
vqueue_prio_init(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t retire_cb,
                 void *retire_cb_arg, vuint32_t rand_seed,
                 vqueue_prio_sentinel_t *head, vqueue_prio_sentinel_t *tail)
{
    ASSERT(retire_cb);
    pqueue->retire_cb     = retire_cb;
    pqueue->retire_cb_arg = retire_cb_arg;

    vskiplist_init(&pqueue->lst, _vqueue_prio_cmp_priority,
                   _vqueue_prio_redirect_retire_cb, pqueue, rand_seed, head,
                   tail);
}
/**
 * Calls the given callback on all remaining nodes.
 *
 * Nodes can be freed in `destroy_cb`
 *
 * @note this is not thread safe and must be called if and only if all threads
 * are done accessing the queue
 *
 * @param pqueue address of vqueue_prio_t object.
 * @param destroy_cb address of a callback function to call on each remaining
 * node.
 * @param args second argument of `destroy_cb`.
 */
static inline void
vqueue_prio_destroy(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t destroy_cb,
                    void *args)
{
    vsize_t top_level = VSKIPLIST_MAX_HEIGHT - 1;
    vqueue_destroy_cb_param_t params;
    params.destroy_cb     = destroy_cb;
    params.destroy_cb_arg = args;

    /* visit and pqueue_destroy all remaining nodes */
    for (vsize_t i = top_level; i <= top_level; i--) {
        _vskiplist_visit_level(&pqueue->lst, i, _vqueue_prio_destroy_node,
                               &params);
    }
}
/**
 * Calculates the required size and height of `vqueue_prio_node_t` container.
 *
 * @param pqueue address of vqueue_prio_t object.
 * @param container_sz the size of the container object returned by sizeof.
 * @param height output parameter. Indicates to how many levels the node will be
 * connected to in the skiplist.
 *
 * @return vsize_t the minimum required size of the container object of
 * `vqueue_prio_node_t`.
 *
 * @note users must call this function everytime they wish to add a new node to
 * the priority queue. see `vqueue_prio_add`.
 *
 * @note if the queue node is not embedded in a container struct. Pass
 * `container_sz` as `sizeof(vqueue_prio_node_t)`.
 */
static inline vsize_t
vqueue_prio_calc_node_sz(vqueue_prio_t *pqueue, vsize_t container_sz,
                         vsize_t *height)
{
    return vskiplist_calc_node_sz(&pqueue->lst, container_sz, height);
}
/**
 * Enqueues the given `node`.
 *
 * @param pqueue address of vqueue_prio_t object.
 * @param node address of vqueue_prio_node_t object.
 * @param priority the priority associated with `node`.
 * @param height the value of the output param `height` updated by
 * `vqueue_prio_calc_node_sz`.
 *
 * @note see `vqueue_prio_calc_node_sz` for container size calculation.
 */
static inline void
vqueue_prio_add(vqueue_prio_t *pqueue, vqueue_prio_node_t *node,
                vuint32_t priority, vsize_t height)
{
    vbool_t success = false;

    ASSERT(pqueue);
    ASSERT(node);

    /* initializing node */
    node->priority = priority;
    vatomic8_write(&node->marked, 0);

    success =
        vskiplist_add(&pqueue->lst, priority, &node->skip_node, NULL, height);

    ASSERT(success &&
           "we should allow for inserting nodes with same priority ");
}
/**
 * Dequeues a node with highest priority available.
 *
 * @param pqueue address of vqueue_prio_t object.
 * @return vqueue_prio_node_t* address of dequeued object.
 */
static inline vqueue_prio_node_t *
vqueue_prio_remove_min(vqueue_prio_t *pqueue)
{
    vqueue_prio_node_t *pnode   = NULL;
    vskiplist_node_t *skip_node = NULL;

    pnode = _vqueue_prio_find_and_mark_min(pqueue);
    if (pnode) {
        skip_node = &pnode->skip_node;
        /* physically detach the node */
        _vskiplist_detach_node(&pqueue->lst, pnode->priority, skip_node);
        return pnode;
    }

    return NULL;
}
/**
 * Visits all available nodes in the given priority queue
 * @note don't use while threads are running
 * @param pqueue address of vqueue_prio_t object
 * @param visitor function address of node visitor function
 * @param arg second argument of `visitor`
 */
static inline void
_vqueue_prio_visit(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t visitor,
                   void *arg)
{
    void *arguments[2];

    ASSERT(pqueue);
    ASSERT(visitor);

    arguments[0] = (void *)visitor;
    arguments[1] = arg;

    _vskiplist_visit_level(&pqueue->lst, 0, _vqueue_prio_visit_node, arguments);
}
/**
 * Finds the node with highest priority and marks it as removed
 *
 * @param pqueue address of vqueue_prio_t object
 * @return vqueue_prio_node_t* address of vqueue_prio_node_t object that has the
 * highest priority
 */
static inline vqueue_prio_node_t *
_vqueue_prio_find_and_mark_min(vqueue_prio_t *pqueue)
{
    vskiplist_t *lst          = NULL;
    vskiplist_node_t *head    = NULL;
    vqueue_prio_node_t *pnode = NULL;

    ASSERT(pqueue);
    lst = &pqueue->lst;

    while (head = vskiplist_get_first(lst), head) {
        pnode = V_CONTAINER_OF(head, vqueue_prio_node_t, skip_node);
        /* if unmarked node is found */
        if (vatomic8_read(&pnode->marked) == 0) {
            /* attempt to mark it */
            if (vatomic8_cmpxchg(&pnode->marked, 0, 1) == 0) {
                /* we marked it successfully it is ours, we can proceed to
                 * remove it */
                return pnode;
            } else {
                verification_ignore();
            }
        } else {
            verification_ignore();
        }
    }
    return NULL;
}
/**
 * This is a callback function used for sorting nodes in the skiplist
 * @note in order to allow for insertion of multiple nodes with the same
 * priority, we tread equality same as less than
 * @param snode address of vskiplist_node_t object
 * @param priority priority to compare against
 * @return int return `-1` if the given priority is less than or equal to the
 * given nodes' priority, returns `1` otherwise
 */
static inline int
_vqueue_prio_cmp_priority(vskiplist_node_t *snode, vskiplist_key_t priority)
{
    vqueue_prio_node_t *pnode = NULL;
    ASSERT(snode);

    pnode = V_CONTAINER_OF(snode, vqueue_prio_node_t, skip_node);
    if (pnode->priority <= priority) {
        return -1;
    } else {
        return 1;
    }
}
/**
 * This is a callback function used for redirecting the freeing of skiplist node
 * to pqueue node
 * @note this is a callback function
 * @param snode  address of vskiplist_node_t object
 * @param arg  address of vqueue_prio_t object
 */
static inline void
_vqueue_prio_redirect_retire_cb(vskiplist_node_t *snode, void *arg)
{
    vqueue_prio_node_t *pnode = NULL;
    vqueue_prio_t *pqueue     = NULL;

    ASSERT(arg);
    ASSERT(snode);

    pnode  = V_CONTAINER_OF(snode, vqueue_prio_node_t, skip_node);
    pqueue = (vqueue_prio_t *)arg;
    pqueue->retire_cb(pnode, pqueue->retire_cb_arg);
}
/**
 * This is a callback function used for visiting skiplist nodes
 *
 * @param snode address of vskiplist_node_t object
 * @param removed indicates if the node was marked for removal
 * @param arg array of void* args
 */
static inline void
_vqueue_prio_visit_node(vskiplist_node_t *snode, vbool_t removed, void *arg)
{
    vqueue_prio_handle_node_t public_visitor = NULL;
    vqueue_prio_node_t *pnode                = NULL;
    void **args                              = (void **)arg;

    ASSERT(snode);
    ASSERT(args);

    public_visitor = (vqueue_prio_handle_node_t)args[0];
    void *param    = args[1];
    pnode          = V_CONTAINER_OF(snode, vqueue_prio_node_t, skip_node);
    /* visit only not marked nodes ! */
    if (vatomic8_read(&pnode->marked) == 0) {
        ASSERT(removed == false);
        public_visitor(pnode, param);
    } else {
        ASSERT(removed);
    }
}
/**
 * This is a callback function called by `vqueue_prio_destroy`
 * it destroys both the given node and the data associated with it
 *
 * @param node address of vskiplist_node_t object
 * @param removed indicates if the node was marked for removal
 * @param args array of void* args
 */
static inline void
_vqueue_prio_destroy_node(vskiplist_node_t *node, vbool_t marked, void *args)
{
    vqueue_destroy_cb_param_t *param = (vqueue_destroy_cb_param_t *)args;
    /* here we pqueue_destroy the node directly */
    vbool_t fully_disconnected = _vskiplist_disconnect_node(node);
    if (fully_disconnected) {
        vqueue_prio_node_t *pnode =
            V_CONTAINER_OF(node, vqueue_prio_node_t, skip_node);
        param->destroy_cb(pnode, param->destroy_cb_arg);
    }
    V_UNUSED(marked);
}
#endif
