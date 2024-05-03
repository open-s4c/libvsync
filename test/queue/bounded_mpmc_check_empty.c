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

#ifdef VSYNC_VERIFICATION
    #define NWRITERS   2
    #define NREADERS   2
    #define QUEUE_SIZE 4
    #define VALUES     2

int
sched_yield(void)
{
    return 0;
}

#else
    #define NWRITERS   2
    #define NREADERS   2
    #define QUEUE_SIZE 40
    #define VALUES     20
#endif
#define XBO_MAX    100U
#define VAL_FACTOR 10U
#define XBO_FACTOR 2U

#define NTHREADS     (NWRITERS + NREADERS)
#define TOTAL_VALUES (VALUES * NWRITERS)

#if QUEUE_SIZE < TOTAL_VALUES
    #error "QUEUE_SIZE should be at least VALUES * NWRITERS"
#endif


void *g_buf[QUEUE_SIZE];
bounded_mpmc_t g_queue;

vsize_t g_val[TOTAL_VALUES];
vsize_t g_ret[TOTAL_VALUES];
vatomic32_t g_cs_x;

void *
writer(void *arg)
{
    vuintptr_t tid = (vuintptr_t)arg;

    xbo_t xbo;
    xbo_init(&xbo, 0, XBO_MAX, XBO_FACTOR);

    for (vsize_t i = 0; i < VALUES; i++) {
        vsize_t idx = tid * VALUES + i;
        g_val[idx]  = tid * VAL_FACTOR + i + 1;
        while (bounded_mpmc_enq(&g_queue, &g_val[idx]) != QUEUE_BOUNDED_OK) {
            // Consider executions where the enqueue succeeded
            verification_ignore();
        }
        xbo_reset(&xbo);
    }
    return NULL;
}

void *
reader(void *arg)
{
    vuintptr_t tid = (vuintptr_t)arg;
    V_UNUSED(tid);

    xbo_t xbo;
    xbo_init(&xbo, 0, XBO_MAX, XBO_FACTOR);

    do {
        void *r = NULL;
        if (bounded_mpmc_deq(&g_queue, &r) == QUEUE_BOUNDED_OK) {
            vuint32_t idx = vatomic32_get_inc_rlx(&g_cs_x);
            ASSERT(idx < TOTAL_VALUES);
            g_ret[idx] = *(vsize_t *)r;
            ASSERT(g_ret[idx] != 0);
        } else {
            // Consider executions where the dequeue succeeded
            verification_ignore();
        }
        xbo_reset(&xbo);
    } while (vatomic32_read_rlx(&g_cs_x) != TOTAL_VALUES);

    return NULL;
}

int
main(void)
{
    bounded_mpmc_init(&g_queue, g_buf, QUEUE_SIZE);

    pthread_t t[NTHREADS];

    for (vuintptr_t i = 0; i < NWRITERS; i++) {
        (void)pthread_create(&t[i], 0, writer, (void *)i);
    }
    for (vuintptr_t i = 0; i < NREADERS; i++) {
        (void)pthread_create(&t[NWRITERS + i], 0, reader, (void *)i);
    }

    for (vuintptr_t i = 0; i < NTHREADS; i++) {
        (void)pthread_join(t[i], NULL);
    }

    // there are no duplicates in the read values
    int found = 0;
    for (int i = 0; i < TOTAL_VALUES; i++) {
        for (int j = 0; j < TOTAL_VALUES; j++) {
            if (g_val[i] == g_ret[j]) {
                g_val[i] = 0xFFFFFFU;
                found++;
                break;
            }
        }
    }
    ASSERT(found == TOTAL_VALUES);
    vuint32_t x = vatomic32_read(&g_cs_x);
    ASSERT(x == TOTAL_VALUES);
    // the queue is empty
    void *r           = NULL;
    bounded_ret_t ret = bounded_mpmc_deq(&g_queue, &r);
    ASSERT(ret == QUEUE_BOUNDED_EMPTY);

    V_UNUSED(r, ret, x);

    return 0;
}
