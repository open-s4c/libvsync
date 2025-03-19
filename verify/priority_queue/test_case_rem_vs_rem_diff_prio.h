/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H

vsize_t ds_idx   = 0;
vpriority_t t0_p = 0;
vpriority_t t1_p = 0;

void
pre(void)
{
    for (vpriority_t i = 1; i <= 3U; i++) {
        pqueue_enq(MAIN_TID, ds_idx, i, i);
    }
}

void
t0(vsize_t tid)
{
    vuintptr_t key = 0;
    key            = pqueue_deq(tid, ds_idx, &t0_p);
    ASSERT(key == t0_p);
}

void
t1(vsize_t tid)
{
    vuintptr_t key = 0;

    key = pqueue_deq(tid, ds_idx, &t1_p);
    ASSERT(key == t1_p);
}

void
t2(vsize_t tid)
{
#if defined(VSYNC_VERIFICATION_QUICK)
    V_UNUSED(tid);
#else
    /* trigger smr cleaning if available */
    pqueue_clean(tid);
#endif
}

void
post(void)
{
    vpriority_t priority = 0;
    vuintptr_t key       = 0;
    key                  = pqueue_deq(MAIN_TID, ds_idx, &priority);
    ASSERT(key == 3);
    ASSERT(priority == 3);
    ASSERT((t0_p == 1 && t1_p == 2) || (t0_p == 2 && t1_p == 1));
}
#endif
