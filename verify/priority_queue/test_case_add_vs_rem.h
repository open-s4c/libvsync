/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
vsize_t ds_idx = 0;
#define INIT_LEN 3
void
pre(void)
{
    for (vpriority_t i = 1; i <= INIT_LEN; i++) {
        pqueue_enq(MAIN_TID, ds_idx, i, i);
    }
}

void
t0(vsize_t tid)
{
    /* replace the item that is going to be dequeued */

    vpriority_t p = 1;
    vuintptr_t k  = 1;
    pqueue_enq(tid, ds_idx, k, p);
}

void
t1(vsize_t tid)
{
    /* remove minimum priority should be one */
    vpriority_t priority = 0;
    vuintptr_t key       = 0;
    key                  = pqueue_deq(tid, ds_idx, &priority);
    ASSERT(key == 1);
    ASSERT(priority == 1);
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
    vpriority_t priority;
    vuintptr_t key;
    vuintptr_t start = 1;
    vuintptr_t end   = INIT_LEN;

    for (vuintptr_t i = start; i <= end; i++) {
        key = pqueue_deq(MAIN_TID, ds_idx, &priority);
        ASSERT(key == i);
        ASSERT(priority == (vuint32_t)i);
    }
}
#endif
