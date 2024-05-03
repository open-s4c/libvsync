/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <pthread.h>
#include <vsync/common/assert.h>
#include <test/thread_launcher.h>

#include <vsync/queue/bounded_locked.h>
#include <vsync/atomic.h>
#ifdef VSYNC_VERIFICATION_QUICK
    #define NWRITERS     2
    #define NREADERS     1
    #define NTHREADS     (NWRITERS + NREADERS)
    #define QUEUE_SIZE   1
    #define VALUES       1
    #define TOTAL_VALUES (VALUES * NWRITERS)
#else
    #define NWRITERS     2
    #define NREADERS     1
    #define NTHREADS     (NWRITERS + NREADERS)
    #define QUEUE_SIZE   1
    #define VALUES       2
    #define TOTAL_VALUES (VALUES * NWRITERS)
#endif

void *g_buf[QUEUE_SIZE];
bounded_locked_t g_queue;

vuint32_t g_val[TOTAL_VALUES];
vuint32_t g_ret[TOTAL_VALUES];
vatomic32_t g_cs_x;

void *
run(void *arg)
{
    vuint32_t tid = (vuint32_t)(vsize_t)(vuintptr_t)arg;

    if (tid < NWRITERS) {
        for (vuint32_t i = 0; i < VALUES; i++) {
            vuint32_t idx = tid * VALUES + i;
            g_val[idx]    = tid * 10U + i + 1U;
            await_while (bounded_locked_enq(&g_queue, &g_val[idx]) !=
                         QUEUE_BOUNDED_OK)
                ;
        }
    }
    await_do {
        void *r = NULL;
        while (bounded_locked_deq(&g_queue, &r) == QUEUE_BOUNDED_OK) {
            vuint32_t idx = vatomic32_get_inc_rlx(&g_cs_x);
            ASSERT(idx < TOTAL_VALUES);
            g_ret[idx] = *(vuint32_t *)r;
            ASSERT(g_ret[idx] != 0);
        }
    }
    while_await(vatomic32_read_rlx(&g_cs_x) != TOTAL_VALUES);

    return NULL;
}

int
main(void)
{
    vuint32_t x_val = 0;
    vbool_t empty   = false;

    bounded_locked_init(&g_queue, g_buf, QUEUE_SIZE);

    launch_threads(NTHREADS, run);

    // there are no duplicates in the read values
    vuint32_t found = 0;
    for (vuint32_t i = 0; i < TOTAL_VALUES; i++) {
        for (vuint32_t j = 0; j < TOTAL_VALUES; j++) {
            if (g_val[i] != 0 && g_val[i] == g_ret[j]) {
                g_val[i] = 0xFFFFFF;
                found++;
                break;
            }
        }
    }

    empty = bounded_locked_empty(&g_queue);

    ASSERT(empty);
    ASSERT(found == TOTAL_VALUES);

    x_val = vatomic32_read(&g_cs_x);
    ASSERT(x_val == TOTAL_VALUES);

    V_UNUSED(empty, x_val);
    return 0;
}
