/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <vsync/common/assert.h>

#include <vsync/queue/bounded_mpmc.h>
#include <vsync/utils/xbo.h>
#include <vsync/atomic.h>
#include <vsync/common/verify.h>
#include <vsync/common/assert.h>
#include <test/thread_launcher.h>

#ifdef VSYNC_VERIFICATION
    #define NWRITERS 2
    #define VALUES   2

int
sched_yield(void)
{
    return 0;
}

#else
    #define NWRITERS 6
    #define VALUES   1500
#endif

#define TOTAL_VALUES             (VALUES * NWRITERS)
#define QUEUE_SIZE               TOTAL_VALUES
#define WRITER_TID_MULTIPLICATOR 10U
#define XBO_MAX                  100U
#define XBO_FACTOR               2U

void *g_buf[QUEUE_SIZE];
bounded_mpmc_t g_queue;
vsize_t g_val[TOTAL_VALUES];

void *
writer(void *arg)
{
    vsize_t tid = (vsize_t)(vuintptr_t)arg;

    xbo_t xbo;
    xbo_init(&xbo, 0, XBO_MAX, XBO_FACTOR);

    for (vsize_t i = 0; i < VALUES; i++) {
        vsize_t idx = tid * VALUES + i;
        g_val[idx]  = tid * WRITER_TID_MULTIPLICATOR + i + 1;
        while (bounded_mpmc_enq(&g_queue, &g_val[idx]) != QUEUE_BOUNDED_OK) {
            xbo_backoff(&xbo, xbo_nop, sched_yield);
        }
        xbo_reset(&xbo);
    }
    return NULL;
}

int
main(void)
{
    bounded_ret_t ret = QUEUE_BOUNDED_OK;

    bounded_mpmc_init(&g_queue, g_buf, QUEUE_SIZE);

    launch_threads(NWRITERS, writer);

    void *r = NULL;
    // dequeue last value
    ret = bounded_mpmc_deq(&g_queue, &r);
    ASSERT(ret == QUEUE_BOUNDED_OK);
    // The first value written by writer tid is tid * WRITER_TID_MULTIPLICATOR +
    // 1
    vsize_t dequeued = *((vsize_t *)r);

    ASSERT((dequeued % WRITER_TID_MULTIPLICATOR) == 1);
    ASSERT(dequeued <= (NWRITERS * WRITER_TID_MULTIPLICATOR + 1));

    // enqueue it back
    ret = bounded_mpmc_enq(&g_queue, &r);
    ASSERT(ret == QUEUE_BOUNDED_OK);

    // now queue is full
    ret = bounded_mpmc_enq(&g_queue, &r);
    ASSERT(ret == QUEUE_BOUNDED_FULL);

    V_UNUSED(ret, dequeued);

    return 0;
}
