/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <test/skiplist/skiplist_interface.h>
#include <stdio.h>
#include <test/thread_launcher.h>

#define DS_IDX 0
#define NNODES 10

void
init(void)
{
    for (vuintptr_t i = 0; i < NNODES; i++) {
        skip_add(MAIN_TID, DS_IDX, i, NULL);
    }
}

vbool_t g_success[NTHREADS][NNODES];

void *
run(void *args)
{
    vsize_t tid            = (vsize_t)(vuintptr_t)args;
    vskiplist_node_t *node = NULL;

    skip_reg(tid);

    for (vuintptr_t i = 0; i < NNODES; i++) {
        skip_enter(tid);
        node = skip_lookup(tid, DS_IDX, i);
        if (node) {
            g_success[tid][i] = skip_rem_node(tid, DS_IDX, i, node);
        }
        skip_exit(tid);
    }

    skip_dereg(tid);
    return NULL;
}

void
verify_removed_once(void)
{
    vbool_t removed = false;

    for (vuintptr_t i = 0; i < NNODES; i++) {
        removed = false;
        /* each key has been removed by at most on thread */
        for (vsize_t t = 0; t < NTHREADS; t++) {
            if (g_success[t][i]) {
                ASSERT(!removed);
                removed = true;
            }
        }
        ASSERT(removed);
    }
}

int
main(void)
{
    skip_init();
    init();
    launch_threads(NTHREADS, run);
    verify_removed_once();
    skip_destroy();
    return 0;
}
