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
}
void
t0(vsize_t tid)
{
    pqueue_enq(tid, ds_idx, 5, 1);
}

void
t1(vsize_t tid)
{
    pqueue_enq(tid, ds_idx, 6, 1);
}

void
t2(vsize_t tid)
{
    V_UNUSED(tid);
}

void
post(void)
{
    vpriority_t priority = 0;
    vuintptr_t key1      = 0;
    vuintptr_t key2      = 0;

    key1 = pqueue_deq(MAIN_TID, ds_idx, &priority);
    ASSERT(priority == 1);
    ASSERT(key1 == 5 || key1 == 6);

    priority = 0;
    key2     = pqueue_deq(MAIN_TID, ds_idx, &priority);
    ASSERT(priority == 1);

    if (key1 == 5)
        ASSERT(key2 == 6);
    else
        ASSERT(key2 == 5);
}
#endif
