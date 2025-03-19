/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <pthread.h>
#include <vsync/atomic.h>
#include <vsync/common/assert.h>
#include <vsync/queue/chaselev.h>
#include <test/thread_launcher.h>

#define ARRAY_SIZE  2
#define NUM_THREADS 8

/* Tests the validity of deque operations dequeue and steal when the deque is
 * empty, by asserting the EMPTY status for all operations. In the owner thread,
 * only dequeue will be executed, and in the other threads only steal will be
 * executed. */

vdeque_t g_vdeque;
vdeque_array_t *g_arr;

static void
run_owner(void)
{
    void *r = NULL;
    vdeque_state_t status;

    // Owner checks validity of dequeue
    status = vdeque_pop_bottom(&g_vdeque, &r);
    ASSERT(status == VDEQUE_STATE_EMPTY);
    status = vdeque_pop_bottom(&g_vdeque, &r);
    ASSERT(status == VDEQUE_STATE_EMPTY);

    V_UNUSED(status);
}

static void
run_stealer(void)
{
    void *v = NULL;
    vdeque_state_t status;

    // Checks validity of dequeue - steal
    status = vdeque_steal(&g_vdeque, &v);
    ASSERT(status == VDEQUE_STATE_EMPTY);
    status = vdeque_steal(&g_vdeque, &v);
    ASSERT(status == VDEQUE_STATE_EMPTY);

    V_UNUSED(status);
}

static void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;
    ASSERT(tid < NUM_THREADS + 1);
    switch (tid) {
        case 0:
            // Only one thread is owner
            run_owner();
            break;
        default:
            run_stealer();
            break;
    }
    return NULL;
}

/* Initializes global chase-lev deque */
static void
init(void)
{
    vsize_t array_size = vdeque_memsize(ARRAY_SIZE);
    g_arr              = (vdeque_array_t *)malloc(array_size);
    vdeque_init(&g_vdeque, g_arr, ARRAY_SIZE);
}

/* Checks validity of global chase-lev deque */
static void
pre(void)
{
    vsize_t b = vatomicsz_read_rlx(&g_vdeque.bottom);
    vsize_t t = vatomicsz_read_rlx(&g_vdeque.top);
    ASSERT(t == 0);
    ASSERT(b == 0);
}

int
main(void)
{
    init();
    pre();
    // Launch threads
    launch_threads(NUM_THREADS + 1, run);
    free(g_arr);
    g_arr = NULL;
    return 0;
}
