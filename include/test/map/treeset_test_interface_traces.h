/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TREESET_TEST_INTERFACE_TRACES
#define VSYNC_TREESET_TEST_INTERFACE_TRACES

#include <test/vmem_stdlib.h>
#include <test/trace_manager.h>

#define NTRACES           NTHREADS
#define DEFAULT_TRACE_LEN 10

trace_t g_added[NTRACES];
trace_t g_removed[NTRACES];
treeset_t g_tree;
vsize_t collected_count;
vsize_t final_count;


static inline vbool_t
tr_add_trace(vsize_t tid, treeset_key_t key)
{
    vbool_t success = treeset_add(&g_tree, key, NULL, NULL);
    if (success) {
        trace_add(&g_added[tid], key);
    }
    return success;
}

static inline vbool_t
tr_rem_trace(vsize_t tid, treeset_key_t key)
{
    vbool_t success = treeset_remove(&g_tree, key, NULL);
    if (success) {
        trace_add(&g_removed[tid], key);
    }
    return success;
}

static inline vbool_t
tr_con_trace(vsize_t tid, treeset_key_t key)
{
    V_UNUSED(tid);
    vbool_t success = treeset_contains(&g_tree, key, NULL);
    return success;
}

static inline void
tr_init_trace(void)
{
    vmem_lib_t mem_lib = VMEM_LIB_DEFAULT();
    treeset_init(&g_tree, mem_lib);
    for (vsize_t i = 0; i < NTRACES; ++i) {
        trace_init(&g_added[i], DEFAULT_TRACE_LEN);
        trace_init(&g_removed[i], DEFAULT_TRACE_LEN);
    }
}

static inline void
tr_destroy_trace(void)
{
    treeset_destroy(&g_tree);
}

static inline void
tr_verify_allocs(void)
{
    vuint64_t allocs = vmem_get_alloc_count();
    vuint64_t frees  = vmem_get_free_count();

    printf("Allocs: %" VUINT64_FORMAT "\n", allocs);
    printf("Frees:  %" VUINT64_FORMAT "\n", frees);
    assert(allocs == frees);
}

vbool_t
verify_trace_unit(trace_unit_t *unit)
{
    if (unit->count > 0) {
        ASSERT(unit->count == 1);
        ASSERT(treeset_contains(&g_tree, unit->key, NULL));
        collected_count++;
    }
    return true;
}

void
visitor(treeset_key_t key, void *a, void *b)
{
    V_UNUSED(key, a, b);
    final_count++;
}

static inline void
tr_verify_traces(void)
{
    trace_t collected;
    trace_init(&collected, DEFAULT_TRACE_LEN);

    for (vsize_t i = 0; i < NTRACES; ++i) {
        trace_merge_into(&collected, &g_added[i]);
    }
    for (vsize_t i = 0; i < NTRACES; ++i) {
        trace_subtract_from(&collected, &g_removed[i]);
    }

    ASSERT(trace_verify(&collected, verify_trace_unit));
    treeset_visit(&g_tree, visitor, NULL);

    printf("Collected: %zu\n", collected_count);
    printf("Final:     %zu\n", final_count);
    assert(collected_count == final_count);

    trace_destroy(&collected);
}

#endif
