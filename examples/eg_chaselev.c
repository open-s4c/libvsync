/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <vsync/atomic.h>
#include <vsync/queue/chaselev.h>

#define ARRAY_SIZE  5
#define NUM_THREADS 3

vdeque_t q;

typedef struct data_s {
    vsize_t id;
} data_t;

void
owner_thread(vsize_t tid)
{
    vsize_t d    = tid;
    data_t *data = NULL;

    for (vsize_t i = 0; i < NUM_THREADS; i++) {
        data     = malloc(sizeof(data_t));
        data->id = d++;

        if (vdeque_push_bottom(&q, data) != VDEQUE_STATE_OK) {
            // either full/ busy
            free(data);
        }
    }

    data = NULL;

    if (vdeque_pop_bottom(&q, (void **)&data) == VDEQUE_STATE_OK) {
        assert(data != NULL);
        printf("[T%zu] deq %zu\n", tid, data->id);
        free(data);
    }
}

void
stealing_thread(vsize_t tid)
{
    data_t *data = NULL;

    if (vdeque_steal(&q, (void **)&data) == VDEQUE_STATE_OK) {
        assert(data != NULL);
        printf("[T%zu] stole %zu\n", tid, data->id);
        free(data);
    }
}

void *
run(void *args)
{
    vsize_t i = (vsize_t)args;

    assert(i < NUM_THREADS);
    switch (i) {
        case 0:
            // Only one thread is owner
            owner_thread(i);
            break;
        default:
            stealing_thread(i);
            break;
    }
    return NULL;
}

int
main(void)
{
    pthread_t threads[NUM_THREADS];

    // Initialize the array struct
    vuint32_t array_size = vdeque_memsize(ARRAY_SIZE);
    vdeque_array_t *a    = (vdeque_array_t *)malloc(array_size);
    assert(a != NULL);

    // Initialize deque
    vdeque_init(&q, a, ARRAY_SIZE);

    // Create threads
    for (vsize_t i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    // Join threads
    for (vsize_t i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
