/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <pthread.h>
#include <vsync/atomic.h>
#include <vsync/common/assert.h>
#include <vsync/queue/chaselev.h>

#define NUM_ENTRIES 2000U
#define NUM_THREADS 50U

vdeque_t g_vdeque;

void *
thread_function(void *args)
{
    vsize_t old_top = 0;
    vsize_t new_top = 0;

    for (vsize_t i = 0; i < NUM_ENTRIES; i++) {
        old_top               = vatomicsz_read_rlx(&g_vdeque.top);
        void *v1              = NULL;
        vdeque_state_t status = vdeque_steal(&g_vdeque, &v1);
        new_top               = vatomicsz_read_rlx(&g_vdeque.top);
        if (status == VDEQUE_STATE_OK) {
            ASSERT(new_top > old_top);
        }
    }
    V_UNUSED(args);
    return NULL;
}

int
main(void)
{
    vsize_t array_size = vdeque_memsize(NUM_ENTRIES);
    vdeque_array_t *a  = (vdeque_array_t *)malloc(array_size);

    ASSERT(a != NULL);
    vdeque_init(&g_vdeque, a, NUM_ENTRIES);

    // Create multiple threads to test concurrent operations
    vsize_t v = 1;
    vdeque_state_t status;

    // Create threads
    pthread_t threads[NUM_THREADS];
    for (vsize_t i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function, NULL);
    }

    for (vsize_t i = 0; i < NUM_ENTRIES; i++) {
        status = vdeque_push_bottom(&g_vdeque, &v);
        ASSERT(status == VDEQUE_STATE_OK);
    }

    // Wait for threads to complete
    for (vsize_t i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
