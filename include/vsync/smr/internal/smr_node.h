/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSMR_NODE_H
#define VSMR_NODE_H

#include <vsync/common/assert.h>
#include <vsync/common/cache.h>

struct smr_node_s;

typedef void (*smr_node_destroy_fun)(struct smr_node_s *, void *args);

typedef struct smr_node_core_s {
    struct smr_node_core_s *next;
} smr_node_core_t;

typedef struct smr_node_s {
    smr_node_core_t core; /* keep as first field */
    int type; /* type is currently used only by kcleanup and once kcleanup gets
                 updated this will be removed */
    smr_node_destroy_fun destroy_fun;
    void *destroy_fun_arg;
} smr_node_t;

#if defined(VSYNC_VERIFICATION)
    #define V_SMR_NODE_BUFF_SIZE 2U
#else
    #if !defined(V_SMR_NODE_BUFF_SIZE)
        #define V_SMR_NODE_BUFF_SIZE 4U
    #endif
#endif

typedef struct smr_node_buff_s {
    smr_node_t *buffer[V_SMR_NODE_BUFF_SIZE];
    vsize_t idx;
} smr_node_buff_t;

static inline void
smr_node_buff_init(smr_node_buff_t *buff)
{
    ASSERT(buff);
    buff->idx = 0;
}

static inline vbool_t
smr_node_buff_insert(smr_node_buff_t *buff, smr_node_t *node,
                     smr_node_destroy_fun destroy_fun, void *destroy_fun_arg)
{
    ASSERT(buff);
    node->destroy_fun     = destroy_fun;
    node->destroy_fun_arg = destroy_fun_arg;
    node->core.next       = NULL;
    if (buff->idx < V_SMR_NODE_BUFF_SIZE) {
        buff->buffer[buff->idx] = node;
        node->core.next =
            buff->idx == 0 ? NULL : &buff->buffer[buff->idx - 1]->core;
        buff->idx++;
        return true;
    }
    return false;
}

static inline vsize_t
smr_node_buff_popall(smr_node_buff_t *buff, smr_node_t **head,
                     smr_node_t **tail)
{
    vsize_t count = 0;
    ASSERT(buff);
    ASSERT(buff->idx <= V_SMR_NODE_BUFF_SIZE);

    if (buff->idx > 0) {
        // pop the last one
        *head = buff->buffer[buff->idx - 1];
        *tail = buff->buffer[0];
        count = buff->idx;
        // empty
        buff->idx = 0;
    }
    return count;
}

#undef V_SMR_NODE_BUFF_SIZE
#endif
