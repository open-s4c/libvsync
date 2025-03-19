/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H

void
pre(void)
{
}
/**
 * for thread with tid = 0
 *
 */
void
t0(vsize_t tid)
{
    enq(tid, 1, 'A');
    enq(tid, 2, 'B');
}

/**
 * for thread with tid = 1
 *
 */
data_t *deq_1 = NULL;
data_t *deq_2 = NULL;
void
t1(vsize_t tid)
{
    deq_1 = deq(tid);
    deq_2 = deq(tid);
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
    switch (g_len) {
        case 2:
            ASSERT(!deq_1);
            ASSERT(!deq_2);
            break;
        case 1:
            if (deq_1) {
                ASSERT(deq_1->key == 1);
                ASSERT(!deq_2);
            } else if (deq_2) {
                ASSERT(deq_2->key == 1);
                ASSERT(!deq_1);
            } else {
                ASSERT(0 &&
                       "we expect at least one dequeue to succeed if the final "
                       "state length is 1");
            }
            ASSERT(g_final_state[0] == 2);
            break;
        case 0:
            ASSERT(deq_1);
            ASSERT(deq_2);
            // verify FIFO
            ASSERT(deq_1->key == 1);
            ASSERT(deq_2->key == 2);
            break;
        default:
            ASSERT(0 && "the length makes no sense");
            break;
    }
}
#endif
