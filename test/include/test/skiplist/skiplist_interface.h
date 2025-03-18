/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_SKIP_LIST_INTERFACE_H
#define VSYNC_SKIP_LIST_INTERFACE_H

#include <time.h>

#if defined(LOCK_FREE_SKIPLIST)
    #include <vsync/map/skiplist_lf.h>
#else
    #error "no vskiplist implementation is defined/included"
#endif

#include <test/trace_manager.h>
#include <test/vmem_stdlib.h>
#include <test/rand.h>
#include <test/smr/ismr.h>
#define N_DS 1

#ifndef NTHREADS
    #error "NTHREADS need to be defined"
#endif

/* an additional trace for the main thread */
#define NTRACES        (SMR_MAX_NTHREADS + 1)
#define TRACE_CAPACITY 100

trace_t g_added[N_DS][NTRACES];
trace_t g_removed[N_DS][NTRACES];

vskiplist_node_t *g_head = NULL;
vskiplist_node_t *g_tail = NULL;
vskiplist_t g_vskiplist[N_DS];

typedef struct skiplist_mock_node_s {
    smr_node_t smr_node;
    vskiplist_key_t key;
    vskiplist_node_t skip_node; /* must be here */
} skiplist_mock_node_t;

static inline skiplist_mock_node_t *
skiplist_mock_node_from_snode(vskiplist_node_t *snode)
{
    if (snode) {
        return V_CONTAINER_OF(snode, skiplist_mock_node_t, skip_node);
    }
    return NULL;
}

static inline void
skiplist_mock_node_print(vskiplist_node_t *snode, vbool_t marked, void *args)
{
    ASSERT(snode);
    skiplist_mock_node_t *cnode = skiplist_mock_node_from_snode(snode);

    printf("\t%s %" VUINTPTR_FORMAT "\n", marked ? "DNE" : "EXISTS",
           cnode->key);

    V_UNUSED(args);
}

static inline int
skiplist_mock_node_cmp(vskiplist_node_t *snode, vskiplist_key_t key)
{
    ASSERT(snode);
    skiplist_mock_node_t *cnode = skiplist_mock_node_from_snode(snode);
    if (cnode->key < key) {
        return -1;
    } else if (cnode->key == key) {
        return 0;
    } else {
        return 1;
    }
}

static inline void
skip_free_cb(smr_node_t *node, void *args)
{
    skiplist_mock_node_t *mock =
        V_CONTAINER_OF(node, skiplist_mock_node_t, smr_node);
    vmem_free(mock);
    V_UNUSED(args);
}

static inline void
skip_retire_cb(vskiplist_node_t *node, void *args)
{
    skiplist_mock_node_t *mock =
        V_CONTAINER_OF(node, skiplist_mock_node_t, skip_node);
    ismr_retire(&mock->smr_node, skip_free_cb, false);
    V_UNUSED(node, args);
}

static inline void
skip_init(void)
{
    vsize_t i = 0;
    vsize_t j = 0;

    ismr_init();

    g_head = vmem_malloc(VSKIPLIST_SENTINEL_SZ);
    ASSERT(g_head);

    g_tail = vmem_malloc(VSKIPLIST_SENTINEL_SZ);
    ASSERT(g_tail);

    for (i = 0; i < N_DS; i++) {
        vskiplist_init(&g_vskiplist[i], skiplist_mock_node_cmp, skip_retire_cb,
                       NULL, (vuint32_t)rand_gen_seed(), g_head, g_tail);

        for (j = 0; j < NTRACES; j++) {
            trace_init(&g_added[i][j], TRACE_CAPACITY);
            trace_init(&g_removed[i][j], TRACE_CAPACITY);
        }
    }
}

static inline void
skip_reg(vsize_t tid)
{
    ismr_reg(tid);
}

static inline void
skip_enter(vsize_t tid)
{
    ismr_enter(tid);
}

static inline void
skip_exit(vsize_t tid)
{
    ismr_exit(tid);
}

static inline void
skip_dereg(vsize_t tid)
{
    ismr_dereg(tid);
}

static inline void
skip_clean(vsize_t tid)
{
    ismr_recycle(tid);
}

static inline void
skip_print(vsize_t ds_idx)
{
    ASSERT(ds_idx < N_DS);

    for (vsize_t level = 0; level < VSKIPLIST_MAX_HEIGHT; level++) {
        printf("vskiplist.level[%zu]\n", level);
        _vskiplist_visit_level(&g_vskiplist[ds_idx], level,
                               skiplist_mock_node_print, NULL);
    }
}

static inline vskiplist_node_t *
skip_lookup(vsize_t tid, vsize_t ds_idx, vskiplist_key_t key)
{
    vskiplist_node_t *skip_node = NULL;
    ASSERT(ds_idx < N_DS);
    skip_node = vskiplist_lookup(&g_vskiplist[ds_idx], key);
    if (skip_node) {
        ASSERT(skiplist_mock_node_cmp(skip_node, key) == 0);
    }
    V_UNUSED(tid);
    return skip_node;
}

vsize_t g_ds_idx = 0;

vbool_t
verify_unit(trace_unit_t *unit)
{
    vskiplist_node_t *node = NULL;
    // verify nodes that were removed no longer exist
    node = skip_lookup(NTHREADS, g_ds_idx, unit->key);
    if (unit->count == 0) {
        /* this means it has been removed and it should not exist */
        return (node == NULL);
    } else {
        /* there is only one instance of this key remaining in the trace and it
         * exists in the vskiplist */
        return (skiplist_mock_node_cmp(node, unit->key) == 0) &&
               (unit->count == 1);
    }
}

static inline void
vskiplist_visit_node(vskiplist_node_t *snode, vbool_t marked, void *args)
{
    trace_t *final_trace = args;
    ASSERT(snode);
    skiplist_mock_node_t *cnode = skiplist_mock_node_from_snode(snode);
    if (!marked) {
        trace_add(final_trace, cnode->key);
    }
}

void
skip_verify(vsize_t ds_idx)
{
    vsize_t t = 0;
    trace_t trace_add;
    trace_t trace_rem;
    trace_t final_state;
    vbool_t success = false;

    trace_init(&trace_add, TRACE_CAPACITY);
    trace_init(&trace_rem, TRACE_CAPACITY);
    trace_init(&final_state, TRACE_CAPACITY);

    for (t = 0; t < NTRACES; t++) {
        trace_merge_into(&trace_add, &g_added[ds_idx][t]);
        trace_merge_into(&trace_rem, &g_removed[ds_idx][t]);
    }

    trace_subtract_from(&trace_add, &trace_rem);

    g_ds_idx = ds_idx;
    success  = trace_verify(&trace_add, verify_unit);

    ASSERT(success);

    /* collect all remaining items */
    _vskiplist_visit_level(&g_vskiplist[ds_idx], 0, vskiplist_visit_node,
                           &final_state);

    vbool_t equal = trace_is_subtrace(&trace_add, &final_state, NULL);

    if (!equal || !success) {
        trace_print(&final_state, "final_state");
        trace_print(&trace_add, "remaining items in trace");
    }

    ASSERT(equal && "the results skip_list does not match the expectation");

    trace_destroy(&trace_add);
    trace_destroy(&trace_rem);
    trace_destroy(&final_state);
}

static inline void
skip_destroy(void)
{
    vsize_t i = 0;
    vsize_t j = 0;

    for (i = 0; i < N_DS; i++) {
        skip_verify(i);
        vskiplist_destroy(&g_vskiplist[i]);

        for (j = 0; j < NTRACES; j++) {
            trace_destroy(&g_added[i][j]);
            trace_destroy(&g_removed[i][j]);
        }
    }
    ismr_destroy();
    vmem_free(g_head);
    g_head = NULL;
    vmem_free(g_tail);
    g_tail = NULL;
    ASSERT(vmem_no_leak());
}

static inline vskiplist_node_t *
skip_lookup_next(vsize_t tid, vsize_t ds_idx, vskiplist_key_t key)
{
    ASSERT(ds_idx < N_DS);
    V_UNUSED(tid);
    return vskiplist_lookup_next(&g_vskiplist[ds_idx], key);
}

static inline vskiplist_node_t *
skip_get_next(vsize_t tid, vsize_t ds_idx, vskiplist_node_t *node)
{
    ASSERT(ds_idx < N_DS);
    V_UNUSED(tid);
    return vskiplist_get_succ(&g_vskiplist[ds_idx], node);
}

static inline vbool_t
skip_add(vsize_t tid, vsize_t ds_idx, vskiplist_key_t key,
         vskiplist_node_t **existing_node)
{
    skiplist_mock_node_t *node = NULL;
    vbool_t success            = false;

    ASSERT(ds_idx < N_DS);

    vsize_t height = 0;
    vsize_t sz     = vskiplist_calc_node_sz(&g_vskiplist[ds_idx],
                                            sizeof(skiplist_mock_node_t), &height);
    node           = vmem_malloc(sz);
    node->key      = key;

    if (existing_node) {
        *existing_node = NULL;
    }
    success = vskiplist_add(&g_vskiplist[ds_idx], key, &node->skip_node,
                            existing_node, height);

    if (success) {
        if (existing_node) {
            ASSERT(*existing_node == NULL);
        }
        trace_add(&g_added[ds_idx][tid], key);
    } else {
        if (existing_node) {
            ASSERT(*existing_node);
            ASSERT(skiplist_mock_node_cmp(*existing_node, key) == 0);
            ASSERT(*existing_node != &node->skip_node);
        }
        vmem_free(node);
    }
    V_UNUSED(tid);
    return success;
}

static inline vbool_t
skip_rem(vsize_t tid, vsize_t ds_idx, vskiplist_key_t key)
{
    vbool_t success = false;

    ASSERT(ds_idx < N_DS);
    success = vskiplist_remove(&g_vskiplist[ds_idx], key);

    if (success) {
        trace_add(&g_removed[ds_idx][tid], key);
    }
    V_UNUSED(tid);
    return success;
}

static inline vbool_t
skip_rem_node(vsize_t tid, vsize_t ds_idx, vskiplist_key_t key,
              vskiplist_node_t *node)
{
    ASSERT(ds_idx < N_DS);
    vbool_t success = vskiplist_remove_node(&g_vskiplist[ds_idx], key, node);
    if (success) {
        trace_add(&g_removed[ds_idx][tid], key);
    }

    V_UNUSED(tid);
    return success;
}

#endif
