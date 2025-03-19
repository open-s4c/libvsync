/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/vtypes.h>
#include <stdio.h>
#include <time.h>
#include <test/prio/pqueue_interface.h>
#include <test/thread_launcher.h>

// higher priorities lower values
#define HIGHEST_PRIORITY 0
#define LOWEST_PRIORITY  3
#define RANGE            (LOWEST_PRIORITY - HIGHEST_PRIORITY)

#define NUM_KEYS       10
#define PRIORITY_RANGE (LOWEST_PRIORITY - HIGHEST_PRIORITY + 1)

void
single_threaded_test(void)
{
    vuintptr_t k                       = 0;
    int p                              = 0;
    vuintptr_t key                     = 0;
    vuint32_t keys_count[NUM_KEYS + 1] = {0};
    vpriority_t priority               = 0;
    vsize_t tid                        = MAIN_TID;

    pqueue_init();

    for (k = 1; k <= NUM_KEYS; k++) {
        for (p = LOWEST_PRIORITY; p >= HIGHEST_PRIORITY; p--) {
            pqueue_enq(tid, 0, k, p);
        }
    }

    for (p = HIGHEST_PRIORITY; p <= LOWEST_PRIORITY; p++) {
        for (k = 1; k <= NUM_KEYS; k++) {
            key = pqueue_deq(tid, 0, &priority);
            ASSERT(key != 0);
            ASSERT(key <= NUM_KEYS);
            keys_count[key]++;
            ASSERT(priority == (vuint32_t)p);
        }
    }

    k = pqueue_deq(tid, 0, &priority);
    ASSERT(k == 0);

    // confirm every thing is dequeued, and nothing is redundant
    for (k = 1; k <= NUM_KEYS; k++) {
        ASSERT(keys_count[key] == PRIORITY_RANGE);
    }
    pqueue_destroy();
}

#include <vsync/atomic.h>
vatomic32_t g_counter = VATOMIC_INIT(0);

void *
run(void *arg)
{
    vsize_t tid               = (vsize_t)(vuintptr_t)arg;
    int p                     = 0;
    vuintptr_t key            = 0;
    vpriority_t priority      = 0;
    vpriority_t last_priority = 0;

    pqueue_reg(tid);

    for (p = LOWEST_PRIORITY; p >= HIGHEST_PRIORITY; p--) {
        key = tid + 1;
        pqueue_enq(tid, 0, key, p);
    }

    vatomic32_inc(&g_counter);
    while (vatomic32_read(&g_counter) != NTHREADS) {}

    while (true) {
        key = pqueue_deq(tid, 0, &priority);
        if (key == 0) {
            break;
        }
        ASSERT(last_priority <= priority);
        last_priority = priority;
    }
    pqueue_dereg(tid);

    return NULL;
}

void
multi_threaded_test(void)
{
    pqueue_init();
    launch_threads(NTHREADS, run);
    pqueue_destroy();
}

int
main(void)
{
    single_threaded_test();
    multi_threaded_test();
}
