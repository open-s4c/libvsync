/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
#if defined(VQUEUE_PRIO_SKIPLIST_BASED) && defined(VSYNC_VERIFICATION_IMM) &&  \
    defined(VSYNC_VERIFICATION_QUICK)

    #define SHORTEN_TEST_CASE
    /* because 5 will not be added */
    #define SKIP 5
#else
    /* does not exist */
    #define SKIP 7
#endif

/**
 * Pre: add nodes with priorities [1:4]
 * T0: enq priority 5 (on heap based this is expected to fail because the queue
 * is full), deq (expect priority 1)
 * T1: enq priority 6
 * T2: do nothing
 * Post: expect the queue to have nodes with priorities [2:6]
 *
 */

vbool_t enq_msg = false;
vbool_t deq_msg = false;
vsize_t ds_idx  = 0;

#define INIT_LEN 4

void
pre(void)
{
    for (vuint32_t i = 1; i <= INIT_LEN; i++) {
        pqueue_enq(MAIN_TID, ds_idx, i, i);
    }
}

void
t0(vsize_t tid)
{
    vuintptr_t key;
    vpriority_t priority;

#if defined(SHORTEN_TEST_CASE)
        // don't do the extra enqueue
#else
    vbool_t success = pqueue_enq(tid, ds_idx, 5, 5);
    #if defined(VQUEUE_PRIO_HEAP_BASED)
    ASSERT(!success && "boundary is reached this should not succeed");
    #else
    V_UNUSED(success);
    #endif
#endif
    deq_msg = true;
    key     = pqueue_deq(tid, ds_idx, &priority);
    ASSERT(key == 1);
    ASSERT(priority == 1);
}

void
t1(vsize_t tid)
{
    vbool_t success = pqueue_enq(tid, ds_idx, 6, 6);
#if defined(VQUEUE_PRIO_HEAP_BASED)
    /* can succeed iff someone dequeued and made room */
    if (success) {
        ASSERT(deq_msg);
    }

    enq_msg = success;
#else
    V_UNUSED(success);
    enq_msg = true;
#endif
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
    vuintptr_t key       = 0;

    vuintptr_t expected_key       = 0;
    vpriority_t expected_priority = 0;

#if defined(VQUEUE_PRIO_HEAP_BASED)
    ASSERT(VQUEUE_PRIO_HEAP_CAPACITY == INIT_LEN &&
           "this test case makes sense only if the capacity is equal to the "
           "inital size ");
#endif

    for (vuintptr_t k = 2; k <= 6; k++) {
        if (k == SKIP)
            continue;
        expected_key      = k;
        expected_priority = (vpriority_t)k;

        key = pqueue_deq(MAIN_TID, ds_idx, &priority);

#if defined(VQUEUE_PRIO_HEAP_BASED)
        /* shorten should be inactive here */
        if (k == 5 && enq_msg) {
            expected_key      = 6;
            expected_priority = 6;
        } else if (k == 6 || k == 5) {
            expected_key      = 0;
            expected_priority = 0;
        }
#endif
        ASSERT(key == expected_key);
        ASSERT(priority == expected_priority);
    }
}
#endif
