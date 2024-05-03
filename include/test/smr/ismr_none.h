/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_ISMR_NONE_H
#define VSYNC_ISMR_NONE_H

#include <vsync/common/assert.h>
#include <vsync/smr/internal/smr_node.h>
#include <test/locked_trace.h>
#include <pthread.h>
#include <stdlib.h>

#define MAIN_TID 0

#define CAPACITY 100


locked_trace_t global_trace;

typedef struct {
    smr_node_t *address;
    smr_node_destroy_fun callback;
    void *args;
} smr_none_retire_info_t;


static inline vbool_t
_ismr_none_destroy_all_cb(trace_unit_t *unit)
{
    ASSERT(unit);
    smr_none_retire_info_t *info = (smr_none_retire_info_t *)unit->key;
    info->callback(info->address, info->args);
    free(info);
    return true;
}

static inline void
ismr_init(void)
{
    locked_trace_init(&global_trace, CAPACITY);
}

static inline void
ismr_enter(vsize_t tid)
{
    V_UNUSED(tid);
}

static inline void
_ismr_none_trace(smr_node_t *node, smr_node_destroy_fun fun, void *args)
{
    /**
     * SMR_NONE means there is no SMR installed
     * Since we cannot just free retired nodes, we add to a global trace
     * protected by a lock, and on destruction we free all of them. This helps
     * if memory checks are active, so intentional leaks are not reported as
     * bugs.
     */
    smr_none_retire_info_t *info = malloc(sizeof(smr_none_retire_info_t));
    if (info) {
        info->address  = node;
        info->callback = fun;
        info->args     = args;

        locked_trace_add(&global_trace, (vuintptr_t)info);
    } else {
        ASSERT(0 && "allocation failed");
    }
}

static inline void
ismr_retire(vsize_t tid, smr_node_t *node, smr_node_destroy_fun fun,
            vbool_t local)
{
    _ismr_none_trace(node, fun, NULL);
    V_UNUSED(tid, local);
}

static inline void
ismr_retire_with_arg(smr_node_t *node, smr_node_destroy_fun fun, void *args)
{
    _ismr_none_trace(node, fun, args);
}

static inline void
ismr_exit(vsize_t tid)
{
    V_UNUSED(tid);
}

static inline void
ismr_reg(vsize_t tid)
{
    V_UNUSED(tid);
}

static inline void
ismr_dereg(vsize_t tid)
{
    V_UNUSED(tid);
}

static inline void
ismr_destroy(void)
{
    locked_trace_destroy(&global_trace, _ismr_none_destroy_all_cb);
}

static inline vbool_t
ismr_sync(vsize_t tid)
{
    V_UNUSED(tid);
    return true;
}

static inline void
ismr_recycle(vsize_t tid)
{
    V_UNUSED(tid);
}

static inline char *
ismr_get_name(void)
{
    return "NONE";
}

#endif
