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
#include <vsync/common/dbg.h>
#include <vsync/queue/chaselev.h>
#include <test/thread_launcher.h>

#define NUM_ENTRIES 200U
#define NUM_THREADS 15U

vdeque_t g_vdeque;
vuint32_t g_num_push = 0;
vuint32_t g_num_pop  = 0;
vatomic32_t g_num_steal;

typedef struct data_s {
    vsize_t id;
} data_t;

void
owner_thread(vsize_t tid)
{
    vsize_t d    = tid;
    data_t *data = NULL;

    for (vsize_t i = 0; i < NUM_ENTRIES; i++) {
        data     = malloc(sizeof(data_t));
        data->id = d++;

        if (vdeque_push_bottom(&g_vdeque, data) != VDEQUE_STATE_OK) {
            // either full/ busy
            free(data);
        } else {
            g_num_push++;
        }
    }

    data              = NULL;
    data_t *prev_data = NULL;
    vdeque_state_t status;

    while (true) {
        status = vdeque_pop_bottom(&g_vdeque, (void **)&data);
        if (status != VDEQUE_STATE_OK) {
            break;
        }

        assert(data != NULL);
        // Check LIFO property of deque
        if (prev_data != NULL) {
            assert(data->id < prev_data->id);
        }
        g_num_pop++;
        DBG_YELLOW("[T%zu] deq %zu\n", tid, data->id);
        prev_data = data;
        if (status == VDEQUE_STATE_EMPTY) {
            free(data);
        }
    }
}

void
stealing_thread(vsize_t tid)
{
    data_t *data      = NULL;
    data_t *prev_data = NULL;
    vdeque_state_t status;

    while (true) {
        status = vdeque_steal(&g_vdeque, (void **)&data);
        if (status != VDEQUE_STATE_OK) {
            break;
        }
        assert(data != NULL);
        // Check FIFO property of steal
        // This property does not hold when there is more than one owner
        if (prev_data != NULL) {
            assert(data->id > prev_data->id);
        }
        vatomic32_inc_rlx(&g_num_steal);
        DBG_YELLOW("[T%zu] stole %zu\n", tid, data->id);
        prev_data = data;
        if (status == VDEQUE_STATE_EMPTY) {
            free(data);
        }
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
    // Initialize the array struct
    vuint32_t array_size = vdeque_memsize(NUM_ENTRIES);
    vdeque_array_t *a    = (vdeque_array_t *)malloc(array_size);
    assert(a != NULL);

    // Initialize deque
    vdeque_init(&g_vdeque, a, NUM_ENTRIES);

    // Create, start, join threads
    launch_threads(NUM_THREADS, run);

    assert(g_num_push == NUM_ENTRIES);
    assert(g_num_push == g_num_pop + vatomic32_read_rlx(&g_num_steal));

    return 0;
}
