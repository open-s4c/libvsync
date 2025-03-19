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
    pqueue_enq(tid, ds_idx, 2, 2);
}

void
t1(vsize_t tid)
{
    pqueue_enq(tid, ds_idx, 1, 1);
}

void
t2(vsize_t tid)
{
    V_UNUSED(tid);
}

void
post(void)
{
    vpriority_t priority;
    vuintptr_t key;
    key = pqueue_deq(MAIN_TID, ds_idx, &priority);
    ASSERT(key == 1);
    ASSERT(priority == 1);
}
#endif
