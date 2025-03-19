/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H

int msg = 0;

/**
 * Possible global orders of enq
 * - 3 1 2
 * - 1 2 3
 * - 1 3 2
 * Possible final states, depending on if t2 deq succeeds or not
 *   !deq   deq
 * - 1 2 or 2
 * - 2 3 or 3
 * - 3 2 or 2
 */

void
pre(void)
{
}

/**
 * for thread with tid = 0
 *
 */
vuint64_t g_t1_deq = 0;
void
t0(vsize_t tid)
{
    enq(tid, 1, 'A');
    msg = 100;
    enq(tid, 2, 'B');
    data_t *node = deq(tid);
    ASSERT(node);
    ASSERT(node->key == 1 || node->key == 2 || node->key == 3);
    g_t1_deq = node->key;
    free(node);
}

/**
 * for thread with tid = 1
 *
 */
vbool_t g_deq_success = false;
vuint64_t g_t2_deq    = 0;
void
t1(vsize_t tid)
{
    data_t *node = deq(tid);
    if (node) {
        ASSERT(node->key == 1 || node->key == 2 || node->key == 3);
        g_deq_success = true;
        g_t2_deq      = node->key;
        // verify message passing
        if (node->key == 2) {
            ASSERT(msg == 100);
        }
        free(node);
    }
}

/**
 * for thread with tid = 2
 *
 */
void
t2(vsize_t tid)
{
    enq(tid, 3, 'C');
    queue_clean(tid);
}

void
post(void)
{
    /*
     * - 1 2 or 2
     * - 2 3 or 3
     * - 3 2 or 2
     */

    /* extract final state */
    queue_print(&g_queue, get_final_state_cb);

    if (g_deq_success) {
        ASSERT(g_len == 1);
        ASSERT(g_t1_deq != g_t2_deq);
        ASSERT(g_t1_deq != g_final_state[0]);
        ASSERT(g_t2_deq != g_final_state[0]);
        ASSERT(g_final_state[0] == 2 || g_final_state[0] == 3);
    } else {
        ASSERT(g_len == 2);
        ASSERT(g_t1_deq != g_final_state[0]);
        ASSERT(g_t1_deq != g_final_state[1]);
        vbool_t state_1_2 = (g_final_state[0] == 1 && g_final_state[1] == 2);
        vbool_t state_2_3 = (g_final_state[0] == 2 && g_final_state[1] == 3);
        vbool_t state_3_2 = (g_final_state[0] == 3 && g_final_state[1] == 2);
        ASSERT(state_1_2 || state_2_3 || state_3_2);
    }
}
#endif
