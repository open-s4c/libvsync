/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
/**
 * Pre-insert with Priority 1
 * T0 & T2 Enqueue with Priority 1
 * T2 dequeues
 */

vsize_t ds_idx            = 0;
vuintptr_t g_consumed_key = 0;

#define PRIORITY 1U

#if defined(VSYNC_VERIFICATION_QUICK)
    #define REMAINING_COUNT 1
#else
    #define REMAINING_COUNT 2
#endif

void
pre(void)
{
    pqueue_enq(MAIN_TID, ds_idx, 1, PRIORITY);
}

void
t0(vsize_t tid)
{
    /* replace the item that is going to be dequeued */
    pqueue_enq(tid, ds_idx, 2, PRIORITY);
}

void
t1(vsize_t tid)
{
    /* remove minimum priority should be one */
    vpriority_t priority = 0;
    g_consumed_key       = pqueue_deq(tid, ds_idx, &priority);
    ASSERT(g_consumed_key);
    ASSERT(priority == PRIORITY);
    pqueue_clean(tid);
}

void
t2(vsize_t tid)
{
#if defined(VSYNC_VERIFICATION_QUICK)
    V_UNUSED(tid);
#else
    pqueue_enq(tid, ds_idx, 3, PRIORITY);
#endif
}

void
post(void)
{
    vpriority_t priority = 0;
    vuintptr_t key       = 0;
    vuintptr_t remaining[REMAINING_COUNT];

    /* verify remaining nodes */
    for (vsize_t i = 0; i < REMAINING_COUNT; i++) {
        key = pqueue_deq(MAIN_TID, ds_idx, &priority);
        ASSERT(key != 0);
        ASSERT(priority == PRIORITY);
        ASSERT(key != g_consumed_key);
        remaining[i] = key;
        if (i > 0) {
            ASSERT(remaining[i] != remaining[i - 1]);
        }
    }
    /* queue state should be empty now */
    priority = 0;
    key      = pqueue_deq(MAIN_TID, ds_idx, &priority);
    ASSERT(key == 0);
    ASSERT(priority == 0);
}
#endif
