/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSTACK_BASELINE_H
#define VSTACK_BASELINE_H

#include <vsync/atomic.h>
#include <vsync/common/assert.h>
#include <vsync/stack/internal/stack_core.h>

typedef struct vstack_s {
    vstack_core_t core;
} vstack_t;

/**
 * Initializes the given `stack`.
 *
 * @param stack address of vstack_t object.
 * @param vstack_usleep address of `usleep` like function.
 */
static inline void
vstack_init(vstack_t *stack)
{
    ASSERT(stack);
    /* init stack core */
    vstack_core_init(&stack->core);
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

    while (!vstack_core_try_push(&stack->core, node))
        ;
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
