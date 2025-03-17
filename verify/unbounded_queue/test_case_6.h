/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
#include <vsync/common/verify.h>
#define INIT_WITH_N_NODES 2
#define KICKOUT_SENTINEL

void
pre(void)
{
    char lbl = 'a';
    /* kick out sentinel */
    enq(MAIN_TID, 0, lbl);
    void *data = deq(MAIN_TID);
    free(data);

    for (vsize_t i = 1; i <= INIT_WITH_N_NODES; i++, lbl++) {
        enq(MAIN_TID, i, lbl);
    }
}

/**
 * This test case is created targeting bugs that  resulting from recycling
 * node (e.g. in the recycle queue)
 */

/**
 * for thread with tid = 0
 *
 */
vbool_t deq_succeeded = false;
void
t0(vsize_t tid)
{
    data_t *node = NULL;

    enq(tid, 3, 'A'); // reuses nothing

    node = deq(tid);
    ASSERT(node);
    ASSERT(node->key == 1);
    free(node);

    node = deq(tid);
    ASSERT(node);
    ASSERT(node->key == 2);
    free(node);
}

/**
 * for thread with tid = 1
 *
 */
void
t1(vsize_t tid)
{
    enq(tid, 4, 'B');
}
/**
 * for thread with tid = 2
 *
 */
void
t2(vsize_t tid)
{
    queue_clean(tid);
}
void
post(void)
{
    queue_print(&g_queue, get_final_state_cb);
    ASSERT(g_len == 2);
}
#endif
