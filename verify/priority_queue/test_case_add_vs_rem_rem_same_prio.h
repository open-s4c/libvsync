/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
vsize_t ds_idx               = 0;
vuintptr_t g_t1_consumed_key = 0;
vuintptr_t g_t2_consumed_key = 0;

void
pre(void)
{
    pqueue_enq(MAIN_TID, ds_idx, 1, 1);
}

void
t0(vsize_t tid)
{
    /* replace the item that is going to be dequeued */
    pqueue_enq(tid, ds_idx, 2, 1);
}

void
t1(vsize_t tid)
{
    /* remove minimum priority should be one */
    vpriority_t priority = 0;
    g_t1_consumed_key    = pqueue_deq(tid, ds_idx, &priority);
    if (g_t1_consumed_key) {
        ASSERT(priority == 1);
    }
}

void
t2(vsize_t tid)
{
    vpriority_t priority = 0;
    g_t2_consumed_key    = pqueue_deq(tid, ds_idx, &priority);
    if (g_t2_consumed_key) {
        ASSERT(priority == 1);
    }
}

void
post(void)
{
    vpriority_t priority = 0;
    vuintptr_t key       = 0;

    /* at least one have succeeded */
    ASSERT(g_t1_consumed_key || g_t2_consumed_key);

    /* if both consumed */
    if (g_t1_consumed_key && g_t2_consumed_key) {
        /* each must have consumed a different node */
        ASSERT((g_t1_consumed_key == 1 && g_t2_consumed_key == 2) ||
               (g_t1_consumed_key == 2 && g_t2_consumed_key == 1));

        /* the queue must be empty now */
        key = pqueue_deq(MAIN_TID, ds_idx, &priority);
        ASSERT(key == 0);
        ASSERT(priority == 0);
    } else {
        vuintptr_t consumed_key = g_t1_consumed_key | g_t2_consumed_key;
        vuintptr_t expected_remaining_key = consumed_key == 2 ? 1 : 2;

        key = pqueue_deq(MAIN_TID, ds_idx, &priority);
        ASSERT(key == expected_remaining_key);
        ASSERT(priority == 1);
    }
}
#endif
