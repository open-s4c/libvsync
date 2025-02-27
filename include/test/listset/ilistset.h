/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTEST_ILISTSET_H
#define VTEST_ILISTSET_H

#if defined(LISTSET_COARSE)
    #include <vsync/map/listset_coarse.h>
#elif defined(LISTSET_FINE)
    #include <vsync/map/listset_fine.h>
#elif defined(LISTSET_OPT)
    #include <vsync/map/listset_opt.h>
#elif defined(LISTSET_LAZY)
    #include <vsync/map/listset_lazy.h>
#elif defined(LISTSET_LF)
    #include <vsync/map/listset_lf.h>
#else
    #error you forgot defining the implementation
#endif

#ifdef DEACTIVATE_LISTSET_SMR
    #define SMR_NONE
#else
    #define SMR_GDUMPV3
#endif

#include <vsync/common/compiler.h>
#include <test/vmem_stdlib.h>
#include <test/smr/ismr.h>
#include <test/trace_manager.h>


typedef struct listset_mock_node_s {
    char lbl;
    vlistset_key_t key;
    vlistset_node_t list_node;
    smr_node_t smr_node;
} listset_mock_node_t;

vatomic64_t g_retire_count = VATOMIC_INIT(0);

static inline listset_mock_node_t *
vlistset_node_to_mock(vlistset_node_t *lnode)
{
    return lnode ? V_CONTAINER_OF(lnode, listset_mock_node_t, list_node) : NULL;
}

static inline void
_free_callback(smr_node_t *node, void *args)
{
    listset_mock_node_t *mock =
        V_CONTAINER_OF(node, listset_mock_node_t, smr_node);
    vmem_free(mock);
    V_UNUSED(args);
}

static inline void
_retire_callback(vlistset_node_t *node, void *args)
{
    listset_mock_node_t *mock = vlistset_node_to_mock(node);
    ismr_retire(&mock->smr_node, _free_callback, false);
    vatomic64_inc_rlx(&g_retire_count);
    V_UNUSED(args);
}
static inline int
_cmp_callback(vlistset_node_t *a, vlistset_key_t key)
{
    listset_mock_node_t *mock_a = vlistset_node_to_mock(a);

    if (mock_a->key == key) {
        return 0;
    } else if (mock_a->key < key) {
        return -1;
    } else {
        return 1;
    }
}


static inline void
vlistset_mock_node_print(vlistset_node_t *lnode, void *arg)
{
    listset_mock_node_t *mock_node = vlistset_node_to_mock(lnode);

    DBG("%" VUINTPTR_FORMAT ", ", mock_node->key);
    V_UNUSED(arg);
}

static inline void
ilistset_init(vlistset_t *g_lst)
{
    vlistset_init(g_lst, _retire_callback, NULL, _cmp_callback);
}

static inline void
ilistset_destroy(vlistset_t *g_lst)
{
    vlistset_destroy(g_lst);
}

static inline vbool_t
ilistset_add(vsize_t tid, vlistset_t *g_lst, vlistset_key_t key,
             vlistset_node_t *node)
{
    V_UNUSED(tid);
    return vlistset_add(g_lst, key, node);
}

static inline vlistset_node_t *
ilistset_get(vsize_t tid, vlistset_t *g_lst, vlistset_key_t key)
{
    V_UNUSED(tid);
    return vlistset_get(g_lst, key);
}

static inline vbool_t
ilistset_remove(vsize_t tid, vlistset_t *g_lst, vlistset_key_t key)
{
    V_UNUSED(tid);
    return vlistset_remove(g_lst, key);
}

static inline void
_ilistset_add_to_trace(vlistset_node_t *node, void *arg)
{
    trace_t *trace                 = (trace_t *)arg;
    listset_mock_node_t *mock_node = vlistset_node_to_mock(node);
    trace_add(trace, mock_node->key);
}

static inline void
ilistset_print(vlistset_t *lst, vlistset_handle_node_t print, void *arg)
{
    _vlistset_visit(lst, print, arg, false);
}

static inline void
ilistset_to_trace(vlistset_t *lst, trace_t *a)
{
    _vlistset_visit(lst, _ilistset_add_to_trace, a, false);
}


#endif
