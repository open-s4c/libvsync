/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_LISTSET_TEST_INTERFACE
#define VSYNC_LISTSET_TEST_INTERFACE

#include <test/listset/ilistset.h>
#include <test/trace_manager.h>
#include <vsync/vtypes.h>

#define NTRACES           (NTHREADS + 2U)
#define DEFAULT_TRACE_LEN 10


#define N_DS 2


vlistset_t g_list[N_DS];

trace_t g_added[N_DS][NTRACES];

trace_t g_removed[N_DS][NTRACES];


static inline vbool_t
lst_add(vsize_t tid, vsize_t lst_id, vlistset_key_t key)
{
    listset_mock_node_t *node = NULL;
    vbool_t success           = false;

    ASSERT(tid < NTRACES);
    ASSERT(lst_id < N_DS);

    ismr_enter(tid);
    node      = vmem_malloc(sizeof(listset_mock_node_t));
    node->key = key;

    success = ilistset_add(tid, &g_list[lst_id], key, &node->list_node);
    if (success) {
        trace_add(&g_added[lst_id][tid], key);
    } else {
        vmem_free(node);
    }
    ismr_exit(tid);
    return success;
}

static inline void
lst_print(vsize_t lst_id, char *lbl)
{
    DBG("%s={ ", lbl);
    ilistset_print(&g_list[lst_id], vlistset_mock_node_print, NULL);
    V_UNUSED(lst_id);
    DBG(" }\n");
}

static inline vbool_t
lst_rem(vsize_t tid, vsize_t lst_id, vlistset_key_t key)
{
    vbool_t success = false;

    ASSERT(tid < NTRACES);
    ASSERT(lst_id < N_DS);

    ismr_enter(tid);

    success = ilistset_remove(tid, &g_list[lst_id], key);
    if (success) {
        trace_add(&g_removed[lst_id][tid], key);
    }
    ismr_exit(tid);
    return success;
}

static inline void
lst_clean(vsize_t tid)
{
    ismr_recycle(tid);
}

static inline vbool_t
lst_con(vsize_t tid, vsize_t lst_id, vlistset_key_t key)
{
    ASSERT(tid < NTRACES);
    ASSERT(lst_id < N_DS);

    ismr_enter(tid);
    vlistset_node_t *n = ilistset_get(tid, &g_list[lst_id], key);

    if (n) {
        listset_mock_node_t *mock_node =
            vlistset_node_to_mock((vlistset_node_t *)n);
        ASSERT(mock_node->key == key);
    }

    ismr_exit(tid);
    return n != NULL;
}

static inline void
lsts_init(void)
{
    vsize_t i = 0;
    vsize_t j = 0;

    ismr_init();

    for (i = 0; i < N_DS; i++) {
        ilistset_init(&g_list[i]);

        for (j = 0; j < NTRACES; j++) {
            trace_init(&g_added[i][j], DEFAULT_TRACE_LEN);
            trace_init(&g_removed[i][j], DEFAULT_TRACE_LEN);
        }
    }
}

static inline void
lsts_destroy(void)
{
    vsize_t i = 0;
    vsize_t j = 0;

    for (i = 0; i < N_DS; i++) {
        ilistset_destroy(&g_list[i]);
        for (j = 0; j < NTRACES; j++) {
            trace_destroy(&g_added[i][j]);
            trace_destroy(&g_removed[i][j]);
        }
    }

    ismr_destroy();

#if !defined(DEACTIVATE_LISTSET_SMR)
    ASSERT(vmem_no_leak());
#endif
}

static inline void
lst_reg(vsize_t tid)
{
    ismr_reg(tid);
}


static inline void
lst_dereg(vsize_t tid)
{
    ismr_dereg(tid);
}

vlistset_t *g_lst_to_verify = NULL;
vbool_t
verify_trace_unit(trace_unit_t *unit)
{
    if (unit->count > 0) {
        ASSERT(g_lst_to_verify);
        ASSERT(unit->count == 1);
        ASSERT(ilistset_get(MAIN_TID, g_lst_to_verify, unit->key));
    }
    return true;
}

static inline void
lst_verify_traces(vsize_t lst_id)
{
    const vsize_t trace_len = DEFAULT_TRACE_LEN * NTRACES;
    vsize_t i               = 0;
    trace_t joint_adds;
    trace_t joint_rems;
    trace_t final_state;
    ASSERT(lst_id < N_DS);
    g_lst_to_verify = &g_list[lst_id];

    trace_init(&joint_adds, trace_len);
    trace_init(&joint_rems, trace_len);
    trace_init(&final_state, trace_len);

    for (i = 0; i < NTRACES; i++) {
        trace_merge_into(&joint_adds, &g_added[lst_id][i]);
        trace_merge_into(&joint_rems, &g_removed[lst_id][i]);
    }

    /* all added and not removed keys */
    trace_subtract_from(&joint_adds, &joint_rems);

    /* all added and not removed keys */
    vbool_t success = trace_verify(&joint_adds, verify_trace_unit);

    ASSERT(success);

    // this might be a stronger way consider it
    ilistset_to_trace(g_lst_to_verify, &final_state);

    vbool_t equal = trace_is_subtrace(&joint_adds, &final_state, NULL);
    if (!equal || !success) {
        trace_print(&final_state, "final_state");
        trace_print(&joint_adds, "remaining items in trace");
    }
    ASSERT(equal && "the results listset does not match the expectation");

    trace_destroy(&joint_adds);
    trace_destroy(&joint_rems);
    trace_destroy(&final_state);
}

#endif
