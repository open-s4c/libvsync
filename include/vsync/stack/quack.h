/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_QUACK_H
#define VSYNC_QUACK_H
/*******************************************************************************
 * @file  quack.h
 * @brief Lockfree concurrent stack/queue (Treiber stack)
 * @ingroup lock_free linearizable
 *
 * It's a queue! It's a stack! No, it's a quack!
 *
 * Quack is a concurrent stack that can be accessed as queue. It is also known
 * as Treiber stack. The producers call quack_push() to add new elements to the
 * stack. The consumers call quack_popall() to return a singled-linked list in
 * LIFO order (as a stack).  The caller can change the order to FIFO order by
 * calling quack_reverse().  The caller is reposible for iterating over the
 * elements of the list.
 *
 * ## Example quack
 * ```
 * typedef struct {
 *      quack_node_t n; // embed quack_node in your data structure
 *      int value;
 * } node_t;
 *
 * quack_t stack;
 * node_t *mynode;
 *
 * // to push into the stack
 * quack_push(&stack, &mynode->n);
 *
 * // to pop from and iterate in FIFO order
 * quack_node_t *n = quack_reverse(quack_popall(&stack));
 * for (; n != NULL; n = n->next) {
 *     node_t *nn = (node_t*) n;
 *     // use node
 * }
 * ```
 * ## Reference
 * Treiber, R.K., 1986. Systems programming: Coping with parallelism.
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/vtypes.h>

typedef struct quack_node_s {
    struct quack_node_s *next;
} quack_node_t;

typedef struct {
    vatomicptr(quack_node_t *) head;
} quack_t;

/** Initializer of quack. */
#define QUACK_INIT()                                                           \
    {                                                                          \
        .head = VATOMIC_INIT(0)                                                \
    }

/**
 * Initializes quack.
 *
 * @param q quack data structure
 */
static inline void
quack_init(quack_t *s)
{
    vatomicptr_init(&s->head, NULL);
}

/**
 * Pushes node into the quack.
 *
 * @param q quack data structure
 * @param n node element
 */
static inline void
quack_push(quack_t *q, quack_node_t *n)
{
    quack_node_t *next = NULL;
    do {
        next    = (quack_node_t *)vatomicptr_read_rlx(&q->head);
        n->next = next;
    } while (vatomicptr_cmpxchg_rel(&q->head, next, n) != next);
}

/**
 * Checks if quack is empty.
 *
 * @param q quack data structure
 * @return true if there is an element in quack, false otherwise.
 */
static inline vbool_t
quack_is_empty(quack_t *const q)
{
    return vatomicptr_read_rlx(&q->head) == NULL;
}

/**
 * Pops all elements from the quack in LIFO order. The user is responsible for
 * iterating over the elements (accessible with `next` pointer).
 *
 * @param q quack data structure
 * @return head of quack
 */
static inline quack_node_t *
quack_popall(quack_t *q)
{
    return (quack_node_t *)vatomicptr_xchg_acq(&q->head, NULL);
}

/**
 * Reverse order of elements taken from the quack (from LIFO to FIFO or
 * vice-versa).
 *
 * @param n head or tail of quack
 * @return tail or head of quack
 */
static inline quack_node_t *
quack_reverse(quack_node_t *n)
{
    quack_node_t *prev = NULL;
    quack_node_t *next = NULL;
    quack_node_t *curr = n;

    while (curr != NULL) {
        next       = curr->next;
        curr->next = prev;
        prev       = curr;
        curr       = next;
    }
    return prev;
}
#endif
