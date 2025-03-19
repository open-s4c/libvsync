/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <test/prio/pqueue_interface.h>
#include <test/thread_launcher.h>
#include <test/rand.h>

#ifndef NTHREADS
    #error "undefined number of threads"
#endif

#ifndef IT
    #error "undefined number of iterations"
#endif

#if !defined(VQUEUE_PRIO_PRIORITY_RANGE)
    /* only for heap is needed! */
    #define VQUEUE_PRIO_PRIORITY_RANGE 8
#endif

#define VQUEUE_PRIO_MIN_PRIORITY 0
#define VQUEUE_PRIO_MAX_PRIORITY (VQUEUE_PRIO_PRIORITY_RANGE - 1)

#define NUM_ITERATIONS IT

vatomic64_t g_key = VATOMIC_INIT(1);

vsize_t ds_idx = 0;

void
enqer(vsize_t tid)
{
    vsize_t i            = 0;
    vuintptr_t key       = 0;
    vpriority_t priority = 0;

    for (i = 0; i < NUM_ITERATIONS; i++) {
        key      = vatomic64_get_inc_rlx(&g_key);
        priority = random_thread_safe_get_next(VQUEUE_PRIO_MIN_PRIORITY,
                                               VQUEUE_PRIO_MAX_PRIORITY);
        pqueue_enq(tid, ds_idx, key, priority);
    }
    DBG_GREEN("T%zu finished with %d iterations", tid, NUM_ITERATIONS);
}

void
deqer(vsize_t tid)
{
    vsize_t i = 0;

    for (i = 0; i < NUM_ITERATIONS; i++) {
        pqueue_deq(tid, ds_idx, NULL);
    }
    DBG_GREEN("T%zu finished with %d iterations", tid, NUM_ITERATIONS);
}

void *
run(void *arg)
{
    vsize_t tid = ((vsize_t)arg);

    pqueue_reg(tid);

    if (tid % 2 == 0) {
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
    launch_threads(NTHREADS, run);
    pqueue_destroy();
    return 0;
}
