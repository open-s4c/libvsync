/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSTACK_CORE_H
#define VSTACK_CORE_H

#include <vsync/vtypes.h>
#include <vsync/atomic.h>
#include <vsync/common/cache.h>
#include <vsync/common/assert.h>

typedef struct vstack_node_s {
    struct vstack_node_s
        *next; /* next does not need to be atomic, because it
                  is never changed in parallel. It is set before insertion! */
} vstack_node_t;

typedef struct vstack_core_s {
    vatomicptr(vstack_node_t *) top;
} VSYNC_CACHEALIGN vstack_core_t;

typedef void (*vstack_node_handler_t)(vstack_node_t *node, void *args);

/**
 * Initializes the given `stack` object.
 *
 * @param stack address of vstack_core_t object.
 */
static inline void
vstack_core_init(vstack_core_t *stack)
{
    ASSERT(stack);
    vatomicptr_write(&stack->top, NULL);
}

/**
 * Tries to push the given `node` to the top of the given `stack`.
 *
 * @param stack address of vstack_core_t object.
 * @param node address of vstack_core_t.
 * @return true push succeeded.
 * @return false push failed.
 */
static inline vbool_t
vstack_core_try_push(vstack_core_t *stack, vstack_node_t *node)
{
    vstack_node_t *top = NULL;
    ASSERT(stack);
    ASSERT(node);
    top        = vatomicptr_read(&stack->top);
    node->next = top;
    return vatomicptr_cmpxchg(&stack->top, top, node) == top;
}
/**
 * Tries to pop the top of the given `stack`.
 *
 * `out_success` indicates if the operation succeeded。
 *
 * @param stack address of vstack_core_t object
 * @param out_success output parameter, false: pop failed, true: pop succeeded
 * @return vstack_node_t* the popped object, can be NULL if the stack is empty.
 * This is only valid if the operation succeeded.
 */
static inline vstack_node_t *
vstack_core_try_pop(vstack_core_t *stack, vbool_t *out_success)
{
    vstack_node_t *node    = NULL;
    vstack_node_t *top     = NULL;
    vstack_node_t *old_top = NULL;

    ASSERT(stack);
    ASSERT(out_success);
    top = vatomicptr_read(&stack->top);
    if (top) {
        node         = top->next;
        old_top      = vatomicptr_cmpxchg(&stack->top, top, node);
        *out_success = top == old_top;
        return old_top;
    }
    *out_success = true;
    return NULL;
}
/**
 * Calls the given `visitor` function on each stack node.
 *
 * @param stack address of vstack_core_t object.
 * @param visitor function address of type vstack_node_handler_t.
 * @param arg second argument of `visitor`, can be NULL.
 */
static inline void
vstack_core_visit(vstack_core_t *stack, vstack_node_handler_t visitor,
                  void *arg)
{
    vstack_node_t *curr = NULL;
    vstack_node_t *next = NULL;

    ASSERT(stack);
    ASSERT(visitor);
    curr = vatomicptr_read(&stack->top);
    while (curr) {
        next = curr->next;
        visitor(curr, arg);
        curr = next;
    }
}
/**
 * Pops all remaining nodes in the stack and calls destroy on them.
 *
 * @param stack address of vstack_core_t object.
 * @param destroy function address of type vstack_node_handler_t.
 * @param destroy_arg second argument of `destroy`, can be NULL.
 */
static inline void
vstack_core_destroy(vstack_core_t *stack, vstack_node_handler_t destroy,
                    void *destroy_arg)
{
    vbool_t success     = false;
    vstack_node_t *node = NULL;
    ASSERT(stack);
    ASSERT(destroy);
    while (true) {
        node = vstack_core_try_pop(stack, &success);

        ASSERT(success &&
               "are you sure all threads finished before you called "
               "vstack_destroy!");

        if (node == NULL) {
            break;
        }

        destroy(node, destroy_arg);
    }
}

#endif
