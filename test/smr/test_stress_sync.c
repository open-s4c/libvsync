/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#include <vsync/map/listset_lf.h>
#define MOCK_INJECTED_FIELD vlistset_node_t lst_node
#include <vsync/common/assert.h>
#include <vsync/common/dbg.h>
#include <vsync/common/compiler.h>
#include <test/thread_launcher.h>
#include <test/smr/ismr.h>

/*
 * @note notice that this test looks pretty much like test_stress
 * However, we don't use recycle here, instead we use a pattern close to HM sync
 * and free directly afterwards. As a result, we expect for everything to be
 * freed before destroy
 */

#define MAX_RETIRE_COUNT (TST_IT * SMR_MAX_NTHREADS)
#define ARR_LEN          (SMR_MAX_NTHREADS + 1)
vlistset_t g_lst;

vsize_t idx[ARR_LEN]                                  = {0};
mock_node_t *retired_nodes[ARR_LEN][MAX_RETIRE_COUNT] = {0};

__thread vsize_t my_tid = SIZE_MAX;

vbool_t
is_sync_required(vsize_t tid)
{
    ASSERT(tid < ARR_LEN);
    return idx[tid] > 0;
}
void
add_to_free(mock_node_t *mock, vsize_t tid)
{
    ASSERT(tid < ARR_LEN);
    vsize_t my_idx = idx[tid];
    ASSERT(my_idx < MAX_RETIRE_COUNT);
    retired_nodes[tid][my_idx] = mock;
    idx[tid] += 1;
}
void
free_all(vsize_t tid)
{
    vsize_t i = 0;

    ASSERT(tid < ARR_LEN);
    vsize_t len = idx[tid];
    for (i = 0; i < len; i++) {
        mock_node_free(&retired_nodes[tid][i]->smr_node, NULL);
    }

    idx[tid] = 0;
}

void
retire_callback(vlistset_node_t *node, void *args)
{
    ASSERT(my_tid < ARR_LEN);
    mock_node_t *mock = V_CONTAINER_OF(node, mock_node_t, lst_node);
    add_to_free(mock, my_tid);
    V_UNUSED(args);
}

int
cmp_callback(vlistset_node_t *node, vlistset_key_t key)
{
    mock_node_t *mock = V_CONTAINER_OF(node, mock_node_t, lst_node);
    if (mock->x < key)
        return -1;
    else if (mock->x == key)
        return 0;
    else
        return 1;
}

/*
 * Ensure everything that was handed in to SMR is freed
 * Ensure retirement kept things safe till exit
 */

void *
run(void *args)
{
    vsize_t i             = 0;
    vsize_t tid           = (vsize_t)(vuintptr_t)args;
    my_tid                = tid;
    vsize_t count_removes = 0;
    vsize_t count_added   = 0;

    ismr_reg(tid);

    for (i = 0; i < TST_IT; i++) {
        ismr_enter(tid);
        vbool_t success = vlistset_get(&g_lst, i) != NULL;
        if (success) {
            success = vlistset_remove(&g_lst, i);
            if (success) {
                count_removes++;
            }
        } else {
            mock_node_t *mock = mock_node_alloc();
            mock->owner       = tid;
            mock->x           = i;
            success           = vlistset_add(&g_lst, i, &mock->lst_node);
            if (success) {
                count_added++;
            } else {
                mock_node_free(&mock->smr_node, NULL);
            }
        }
        ismr_exit(tid);

        if (is_sync_required(tid)) {
            ismr_sync(tid);
            free_all(tid);
        }
    }

    ismr_dereg(tid);
    DBG_GREEN("T%zu #adds: %zu, #rems: %zu", tid, count_added, count_removes);
    return NULL;
}

int
main(void)
{
#if !defined(SMR_KCLEANUP) && !defined(SMR_CLEANUP)
    my_tid = MAIN_TID;
    ismr_init();
    vlistset_init(&g_lst, retire_callback, &g_lst, cmp_callback);
    launch_threads(SMR_MAX_NTHREADS, run);
    mock_node_no_leak();
    vlistset_destroy(&g_lst);
    free_all(MAIN_TID);
    ASSERT(mock_node_no_leak());
    ismr_destroy();
#endif
    return 0;
}
