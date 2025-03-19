/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef DOUBLE_LINKED_LIST_H
#define DOUBLE_LINKED_LIST_H

#include <vsync/common/assert.h>
#include <vsync/common/dbg.h>

typedef struct dbl_list_node_s {
    struct dbl_list_node_s *next;
    struct dbl_list_node_s *prev;
} dbl_list_node_t;

typedef struct dbl_list_s {
    dbl_list_node_t *head;
    vsize_t count;
} dbl_list_t;

#define DBL_LIST_INIT()                                                        \
    {                                                                          \
        .head = NULL, .count = 0                                               \
    }

typedef void (*dbl_list_visitor)(dbl_list_node_t *node, void *args);

static inline void
dbl_list_init(dbl_list_t *list)
{
    ASSERT(list);
    list->head  = NULL;
    list->count = 0;
}

static inline void
dbl_list_add(dbl_list_t *list, dbl_list_node_t *node)
{
    ASSERT(list);
    ASSERT(node);

    node->next = list->head;
    if (list->head) {
        list->head->prev = node;
    }
    list->head = node;
    node->prev = NULL;
    list->count++;
}

static inline void
dbl_list_rem(dbl_list_t *list, dbl_list_node_t *node)
{
    ASSERT(list);
    ASSERT(node);

    dbl_list_node_t *succ = node->next;
    dbl_list_node_t *prev = node->prev;

    if (prev) {
        ASSERT(prev->next == node);
        ASSERT(list->head != node);
        prev->next = succ;
    } else {
        ASSERT(list->head == node);
        list->head = succ;
    }

    if (succ) {
        ASSERT(succ->prev == node);
        succ->prev = prev;
    }

    list->count--;
}

static inline void
dbl_list_visit_forward(dbl_list_node_t *node, dbl_list_visitor visitor,
                       void *args)
{
    dbl_list_node_t *curr = NULL;
    dbl_list_node_t *succ = NULL;

    curr = node;
    while (curr) {
        succ = curr->next;
        visitor(curr, args);
        curr = succ;
    }
}

static inline void
dbl_list_visit_backward(dbl_list_node_t *node, dbl_list_visitor visitor,
                        void *args)
{
    dbl_list_node_t *curr = NULL;
    dbl_list_node_t *prev = NULL;

    curr = node;
    while (curr) {
        prev = curr->prev;
        visitor(curr, args);
        curr = prev;
    }
}

static inline vsize_t
dbl_list_get_length(dbl_list_t *list)
{
    ASSERT(list);
    return list->count;
}

static inline dbl_list_node_t *
dbl_list_get_head(dbl_list_t *list)
{
    return list->head;
}

static inline dbl_list_node_t *
dbl_list_get_next(dbl_list_node_t *node)
{
    return node->next;
}

#endif
