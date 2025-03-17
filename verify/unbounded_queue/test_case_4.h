/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H

/**
 * This test case can detect access to freed memory, if free is used instead of
 * SMR in LF queue
 *
 * The scenario why we cannot just use free in the LF implementation
 * initial state empty queue
 *	T = Sentinel
 *	H = Sentinel
 * T1 (enq(A)): reads T = Sentinel, SLEEP
 * T2 (enq(B)): reads T = Sentinel, Sentinel->next = B, T = B
 * T3 (deq(B)): reads H = Sentinel, reads Sentinel->next = B, reads data from B,
 * advances H, H = B, Frees(Sentinel) T1 (enq(A)): .. WAKE UP read
 * Sentinel->next (it is freed)!! BUG
 *
 */

void
pre(void)
{
    /* kickout sentinel */
    enq(MAIN_TID, 0, 'a');
    void *data = deq(MAIN_TID);
    free(data);
}

/**
 * for thread with tid = 0
 *
 */
void
t0(vsize_t tid)
{
    enq(tid, 2, 'A');
}

/**
 * for thread with tid = 1
 *
 */
data_t *deq_1      = NULL;
vbool_t g_dequeued = false;
void
t1(vsize_t tid)
{
    deq_1 = deq(tid);
    if (deq_1) {
        ASSERT(deq_1->key == 2 || deq_1->key == 3);
        free(deq_1);
        deq_1      = NULL;
        g_dequeued = true;
    } else {
        verification_ignore();
    }
}
/**
 * for thread with tid = 2
 *
 */
void
t2(vsize_t tid)
{
    enq(tid, 3, 'B');
    queue_clean(tid);
}
void
post(void)
{
    queue_print(&g_queue, get_final_state_cb);
    if (g_dequeued) {
        ASSERT(g_len == 1);
    } else {
        ASSERT(g_len == 2);
    }
    ASSERT(g_final_state[0] == 2 || g_final_state[0] == 3);
}
#endif
