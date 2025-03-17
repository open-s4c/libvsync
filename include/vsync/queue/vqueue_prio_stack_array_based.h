/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VQUEUE_PRIO_ARRAY_BASED_H
#define VQUEUE_PRIO_ARRAY_BASED_H
/*******************************************************************************
 * @file  vqueue_prio_stack_array_based.h
 * @brief Array-based bounded priority queue.
 * @ingroup requires_smr linearizable lock_free unbounded_queue
 *
 * This implementation is an array of stacks (see elimination_stack.h).
 * Each stack is associated with a priority equals to the array index of the
 * slot that the stack occupies. It has bounded priority range and unbounded
 * capacity.
 *
 * It requires SMR (see gdump.h)
 *
 * @note A different type of stack can be used e.g., xbo_stack.h, or a stack
 * without a backoff mechanism, which might be better suited to low contention
 * scenarios. Contact the authors, if you need to use different type of
 * stack.
 *
 * @example
 * @include eg_queue_prio_stack_array.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 15.2]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/
#include <vsync/common/dbg.h>
#include <vsync/stack/elimination_stack.h>

#if !defined(VQUEUE_PRIO_PRIORITY_RANGE)
    /**
     * `VQUEUE_PRIO_PRIORITY_RANGE` configures the queue to accept priorities
     * from [0:VQUEUE_PRIO_PRIORITY_RANGE-1]
     *
     * @note `VQUEUE_PRIO_PRIORITY_RANGE` must be power of two.
     */
    #define VQUEUE_PRIO_PRIORITY_RANGE 16U
#endif

typedef vstack_node_t vqueue_prio_node_t;

typedef void (*vqueue_prio_handle_node_t)(vqueue_prio_node_t *node, void *arg);

typedef struct vqueue_prio_s {
    vstack_t bin[VQUEUE_PRIO_PRIORITY_RANGE];
    vqueue_prio_handle_node_t retire_fun;
    void *retire_fun_arg;
} vqueue_prio_t;

/**
 * Initializes the queue.
 *
 * @param pqueue address of vqueue_prio_t object.
 * @param retire_cb callback function used for retiring detached nodes to the
 * SMR.
 * @param retire_cb_arg second parameter of `retire_cb`.
 * @param rand_fun a function pointer to a function that generates a random
 * number.
 */
static inline void
vqueue_prio_init(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t retire_fun,
                 void *retire_fun_arg, backoff_rand_fun_t rand_fun)
{
    vsize_t i       = 0;
    vstack_t *stack = NULL;

    ASSERT(retire_fun);
    pqueue->retire_fun     = retire_fun;
    pqueue->retire_fun_arg = retire_fun_arg;

    for (i = 0; i < VQUEUE_PRIO_PRIORITY_RANGE; i++) {
        stack = &pqueue->bin[i];
        vstack_init(stack, rand_fun);
    }
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
    vsize_t i       = 0;
    vstack_t *stack = NULL;
    for (i = 0; i < VQUEUE_PRIO_PRIORITY_RANGE; i++) {
        stack = &pqueue->bin[i];
        vstack_destroy(stack, destroy_cb, args);
    }
}
/**
 * Enqueues the given `node`.
 *
 * @param pqueue address of vqueue_prio_t object.
 * @param node address of vqueue_prio_node_t object.
 * @param priority the priority associated with `node`.
 */
static inline void
vqueue_prio_add(vqueue_prio_t *pqueue, vqueue_prio_node_t *node,
                vsize_t priority)
{
    vstack_t *stack = NULL;
    ASSERT(priority < VQUEUE_PRIO_PRIORITY_RANGE);
    stack = &pqueue->bin[priority];
    vstack_push(stack, node);
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
    vsize_t i                 = 0;
    vstack_t *stack           = NULL;
    vqueue_prio_node_t *qnode = NULL;

    for (i = 0; i < VQUEUE_PRIO_PRIORITY_RANGE; i++) {
        stack = &pqueue->bin[i];
        qnode = (vqueue_prio_node_t *)vstack_pop(stack);
        if (qnode) {
            pqueue->retire_fun(qnode, pqueue->retire_fun_arg);
            return qnode;
        }
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
    vsize_t i       = 0;
    vstack_t *stack = NULL;

    for (i = 0; i < VQUEUE_PRIO_PRIORITY_RANGE; i++) {
        stack = &pqueue->bin[i];
        _vstack_visit(stack, visitor, arg);
    }
}
#endif
