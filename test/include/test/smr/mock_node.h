/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_SMR_MOCK_NODE_H
#define VSYNC_SMR_MOCK_NODE_H

#include <vsync/common/compiler.h>
#include <vsync/vtypes.h>
#include <test/vmem_stdlib.h>

typedef struct mock_node_s {
    uintptr_t x;
    vsize_t owner;
    vatomic64_t key;
    smr_node_t smr_node;
    vatomicptr_t next;
#ifdef MOCK_INJECTED_FIELD
    MOCK_INJECTED_FIELD;
#endif
} mock_node_t;

typedef void (*cb_notify)(mock_node_t *node);

cb_notify g_notify = NULL;

static inline void *
mock_node_alloc(void)
{
    return vmem_malloc(sizeof(mock_node_t));
}

static inline vuint64_t
mock_node_get_free_count(void)
{
    return vmem_get_free_count();
}

static inline void
mock_node_free(smr_node_t *snode, void *args)
{
    mock_node_t *node = V_CONTAINER_OF(snode, mock_node_t, smr_node);
    if (g_notify) {
        g_notify(node);
    }
#if !defined(SMR_GBA)
    vmem_free(node);
#endif
    V_UNUSED(args);
}

static inline vbool_t
mock_node_no_leak(void)
{
    return vmem_no_leak();
}

#endif
