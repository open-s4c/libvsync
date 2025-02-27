/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <vsync/atomic.h>
#include <vsync/common/assert.h>
#include <vsync/queue/chaselev.h>

#define NUM_ENTRIES 2000U
#define NUM_THREADS 100U

vdeque_t g_deque;
vuint32_t g_num_enqueue = 0;

static void *
stealer(void *args)
{
    V_UNUSED(args);
    void *v = NULL;
    while (vdeque_steal(&g_deque, &v) != VDEQUE_STATE_EMPTY) {}
    return NULL;
}

void
owner(void)
{
    vuint32_t v = 1;
    void *r     = NULL;
    vdeque_state_t status;
    for (vuint32_t i = 0; i < NUM_ENTRIES; i++) {
        status = vdeque_push_bottom(&g_deque, &v);
        ASSERT(status == VDEQUE_STATE_OK);
        g_num_enqueue++;
        if (i % 3 == 0) {
            vdeque_pop_bottom(&g_deque, &r);
        }
    }

    V_UNUSED(status);
}

int
main(void)
{
    vuint32_t array_size = vdeque_memsize(NUM_ENTRIES);
    vdeque_array_t *a    = (vdeque_array_t *)malloc(array_size);
    vdeque_init(&g_deque, a, NUM_ENTRIES);

    // Create threads
    pthread_t threads[NUM_THREADS];
    for (vuint32_t i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, stealer, NULL);
    }
    owner();
    // Wait for threads to complete
    for (vuint32_t i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    ASSERT(g_num_enqueue == NUM_ENTRIES);
    return 0;
}
