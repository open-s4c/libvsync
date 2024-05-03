/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_RECYCLE_POOL_H
#define VSYNC_RECYCLE_POOL_H

#include <vsync/atomic/atomicptr_stamped.h>
__thread vqueue_ub_node_t *node_pool = NULL;

static inline void
node_pool_put(vqueue_ub_node_t *qnode)
{
    vuint8_t current_stamp = 0;
    // resetting the stamp to zero can cause ABA
    (void)vatomicptr_stamped_get(&qnode->next, &current_stamp);
    vatomicptr_stamped_set(&qnode->next, node_pool, current_stamp + 1);
    node_pool = qnode;
}

static inline vqueue_ub_node_t *
node_pool_get(void)
{
    vqueue_ub_node_t *head = node_pool;
    if (head) {
        node_pool = vatomicptr_stamped_get_pointer(&node_pool->next);
    }
    return head;
}

static inline vqueue_ub_node_t *
node_pool_get_and_destroy(void)
{
    vqueue_ub_node_t *head = node_pool;
    node_pool              = NULL;
    return head;
}


static inline void
node_pool_destroy(vqueue_ub_node_t *head,
                  void (*callback)(vqueue_ub_node_t *, void *))
{
    vqueue_ub_node_t *curr = head;
    vqueue_ub_node_t *succ = NULL;
    while (curr) {
        succ = vatomicptr_stamped_get_pointer(&curr->next);
        callback(curr, NULL);
        curr = succ;
    }
}


#endif
