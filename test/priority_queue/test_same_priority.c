/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <test/prio/pqueue_interface.h>
#include <test/thread_launcher.h>
#include <test/rand.h>
#include <vsync/utils/math.h>

#ifndef NTHREADS
    #error "undefined number of threads"
#endif

#ifndef IT
    #error "undefined number of iterations"
#endif

#define NUM_ITERATIONS IT

/* goal: nothing gets lost under same priority */
#define PRIORITY 0

#ifndef PRE_EXISTING_NODES_COUNT
    #define PRE_EXISTING_NODES_COUNT 10
#endif

vatomic64_t g_key = VATOMIC_INIT(1);

#define DS_IDX 0

void
pre_populate(vsize_t count)
{
    vsize_t i      = 0;
    vuintptr_t key = 0;

    for (i = 0; i < count; i++) {
        key = vatomic64_get_inc_rlx(&g_key);
        pqueue_enq(MAIN_TID, DS_IDX, key, PRIORITY);
    }
}

void
enqer(vsize_t tid)
{
    vsize_t i      = 0;
    vuintptr_t key = 0;

    for (i = 0; i < NUM_ITERATIONS; i++) {
        key = vatomic64_get_inc_rlx(&g_key);
        pqueue_enq(tid, DS_IDX, key, PRIORITY);
    }
}

void
deqer(vsize_t tid)
{
    vsize_t i = 0;

    for (i = 0; i < NUM_ITERATIONS; i++) {
        pqueue_deq(tid, DS_IDX, NULL);
    }
}

void *
run(void *arg)
{
    vsize_t tid = ((vsize_t)arg);

    pqueue_reg(tid);

    if (VIS_EVEN(tid)) {
        enqer(tid);
    } else {
        deqer(tid);
    }

    pqueue_dereg(tid);

    return NULL;
}

int
main(void)
{
    pqueue_init();
    pre_populate(PRE_EXISTING_NODES_COUNT);
    launch_threads(NTHREADS, run);
    pqueue_destroy();
    return 0;
}
