/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#define LF_LISTSET_DONT_EMBED_SMR_NODE
#include <vsync/map/listset_lf.h>
#define MOCK_INJECTED_FIELD vlistset_node_t lst_node

#include <vsync/common/dbg.h>
#include <vsync/common/compiler.h>
#include <test/thread_launcher.h>
#include <test/smr/ismr.h>

vlistset_t g_lst;

void
retire_callback(vlistset_node_t *node, void *args)
{
    mock_node_t *mock = V_CONTAINER_OF(node, mock_node_t, lst_node);
    ismr_retire(&mock->smr_node, mock_node_free, false);
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
#if !defined(SMR_KCLEANUP)
        ismr_recycle(tid);
#endif
    }

    ismr_dereg(tid);
    DBG_GREEN("T%zu #adds: %zu, #rems: %zu", tid, count_added, count_removes);
    return NULL;
}

int
main(void)
{
    ismr_init();
    vlistset_init(&g_lst, retire_callback, &g_lst, cmp_callback);
    launch_threads(SMR_MAX_NTHREADS, run);
    mock_node_no_leak();
    vlistset_destroy(&g_lst);
    ismr_destroy();
    ASSERT(mock_node_no_leak());
    return 0;
}
