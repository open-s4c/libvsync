/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
vsize_t ds_idx = 0;

void
pre(void)
{
    pqueue_enq(MAIN_TID, ds_idx, 1, 1);
    pqueue_enq(MAIN_TID, ds_idx, 1, 1);
    pqueue_enq(MAIN_TID, ds_idx, 2, 2);
}

void
t0(vsize_t tid)
{
    /* remove minimum priority should be one */
    vpriority_t priority;
    vuintptr_t key;
    key = pqueue_deq(tid, ds_idx, &priority);
    ASSERT(key == 1);
    ASSERT(priority == 1);
}

void
t1(vsize_t tid)
{
    /* remove minimum priority should be one */
    vpriority_t priority;
    vuintptr_t key;
    key = pqueue_deq(tid, ds_idx, &priority);
    ASSERT(key == 1);
    ASSERT(priority == 1);
}

void
t2(vsize_t tid)
{
#if defined(VSYNC_VERIFICATION_QUICK)
        // skip cleaning on quick
#else
    pqueue_clean(tid);
#endif
}

void
post(void)
{
    vpriority_t priority;
    vuintptr_t key;

    key = pqueue_deq(MAIN_TID, ds_idx, &priority);
    ASSERT(key == 2);
    ASSERT(priority == 2);
}
#endif
