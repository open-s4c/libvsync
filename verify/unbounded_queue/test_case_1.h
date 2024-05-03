/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

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

/**
 * for thread with tid = 0
 *
 */
vuint64_t g_t1_deq = 0;
void
t1(vsize_t tid)
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
t2(vsize_t tid)
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
t3(vsize_t tid)
{
    enq(tid, 3, 'C');
    queue_clean(tid);
}

void
verify(void)
{
    /*
     * - 1 2 or 2
     * - 2 3 or 3
     * - 3 2 or 2
     */

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
        ASSERT((g_final_state[0] == 1 && g_final_state[1] == 2) ||
               (g_final_state[0] == 2 && g_final_state[1] == 3) ||
               (g_final_state[0] == 3 && g_final_state[1] == 2));
    }
}
