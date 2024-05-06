/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSTACK_XBO_H
#define VSTACK_XBO_H
/******************************************************************************
 * @file  xbo_stack.h
 * @brief Unbounded lock-free stack with exponential backoff.
 * @ingroup requires_smr lock_free linearizable
 *
 * When an attempt to pop/push fails, the calling thread is
 * put to sleep before it reattempts the operation. The sleep duration is
 * determined randomly and it may not exceed the current limit, which doubles on
 * each backoff. The current limit cannot exceed `max_backoff` passed to
 * `vstack_init`.
 *
 * @example
 * @include eg_stack_xbo.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 11]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 *****************************************************************************/
#include <vsync/atomic.h>
#include <vsync/common/assert.h>
#include <vsync/stack/internal/stack_core.h>
#include <vsync/utils/backoff.h>
#include <vsync/common/verify.h>

typedef struct vstack_s {
    vstack_core_t core;
    backoff_t backoff;
} vstack_t;

/**
 * Initializes the given `stack`.
 *
 * @param stack address of vstack_t object.
 * @param vstack_usleep address of `usleep` like function.
 * @param rand_fun  a function pointer to a function that generates a random
 * number.
 * @param min_backoff minimum amount of microseconds a thread is allowed to
 * sleep.
 * @param max_backoff maximum allowed amount of microseconds to sleep.
 */
static inline void
vstack_init(vstack_t *stack, backoff_usleep_fun_t vstack_usleep,
            backoff_rand_fun_t rand_fun, vuint32_t min_backoff,
            vuint32_t max_backoff)
{
    ASSERT(stack);
    ASSERT(vstack_usleep);
    ASSERT(rand_fun);
    ASSERT(min_backoff <= max_backoff);

    /* init stack core */
    vstack_core_init(&stack->core);

    /* init exponential backoff object */
    xbobackoff_init(&stack->backoff, min_backoff, max_backoff, vstack_usleep,
                    rand_fun);
}
/**
 * Pushes the given `node` to the top of the given `stack`.
 *
 * @param stack address of vstack_t object.
 * @param node  address of vstack_node_t object.
 * @note this operation always succeeds.
 */
static inline void
vstack_push(vstack_t *stack, vstack_node_t *node)
{
    ASSERT(stack);
    ASSERT(node);

    while (true) {
        if (vstack_core_try_push(&stack->core, node)) {
            return;
        } else {
            verification_ignore();
            xbobackoff_backoff(&stack->backoff);
        }
    }
}
/**
 * Pops the top of the given `stack`.
 *
 * @param stack address of vstack_t object.
 * @return address of vstack_node_t object. If the stack is not empty.
 * @return NULL on empty stack.
 * @note must be called inside an SMR critical section.
 */
static inline vstack_node_t *
vstack_pop(vstack_t *stack)
{
    vbool_t success     = false;
    vstack_node_t *node = NULL;

    ASSERT(stack);

    while (true) {
        node = vstack_core_try_pop(&stack->core, &success);
        if (success) {
            return node;
        } else {
            verification_ignore();
            xbobackoff_backoff(&stack->backoff);
        }
    }
}
/**
 * Pops all remaining nodes in the stack and calls `destroy` on them.
 *
 * @param stack address of vstack_t object.
 * @param destroy function address of type vstack_node_handler_t.
 * @param arg second argument of `destroy`, can be NULL.
 */
static inline void
vstack_destroy(vstack_t *stack, vstack_node_handler_t destroy, void *arg)
{
    vstack_core_destroy(&stack->core, destroy, arg);
}
/**
 * Calls the given `visitor` function on each stack node.
 *
 * @param stack address of vstack_t object.
 * @param visitor function address of type vstack_node_handler_t.
 * @param arg second argument of `visitor`, can be NULL.
 */
static inline void
_vstack_visit(vstack_t *stack, vstack_node_handler_t visitor, void *arg)
{
    vstack_core_visit(&stack->core, visitor, arg);
}
#endif
