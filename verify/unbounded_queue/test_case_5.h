/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
/**
 * check queue_empty()
 */

void
pre(void)
{
    enq(MAIN_TID, 1U, 'a');
}

/**
 * for thread with tid = 0
 *
 */
void
t0(vsize_t tid)
{
    enq(tid, 2, 'A');
    void *data = deq(tid);
    free(data);
}

/**
 * for thread with tid = 1
 *
 */
int msg = 0;
void
t1(vsize_t tid)
{
    msg        = 1;
    void *data = deq(tid);
    free(data);
}
/**
 * for thread with tid = 2
 *
 */
void
t2(vsize_t tid)
{
    /* must imply both dequeues are done and thus the msg must be passed */
    if (empty(tid)) {
        ASSERT(msg == 1);
    } else {
        queue_clean(tid);
    }
}
void
post(void)
{
    queue_print(&g_queue, get_final_state_cb);
    ASSERT(g_len == 0);
}
#endif
