/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <vsync/queue/chaselev.h>
#define STEALER_NUM 2
#include <pthread.h>
#include <stdio.h>

#define ARRAY_SIZE 4
#if defined(VSYNC_VERIFICATION_QUICK)
    #define STEAL_COUNT 1
#else
    #define STEAL_COUNT 2
#endif

vdeque_t g_vdeque;

vuint64_t enq_count                   = 0;
vuint64_t deq_count                   = 0;
vuint64_t deq_data_sum                = 0;
vuint64_t steal_count[STEALER_NUM]    = {0};
vuint64_t steal_data_sum[STEALER_NUM] = {0};

void *
steal_func(void *arg)
{
    vsize_t id    = *(vsize_t *)arg;
    vuint64_t buf = 0;
    for (vsize_t i = 0; i < STEAL_COUNT; i++) {
        if (vdeque_steal(&g_vdeque, (void **)&buf) == VDEQUE_STATE_OK) {
            steal_count[id]++;
            steal_data_sum[id] += buf;
        }
    }
    free(arg);
    return NULL;
}

void *
self_func(void *arg)
{
    V_UNUSED(arg);
    vuint64_t buf = 0;

    for (vsize_t i = 0; i < 1; i++) {
        if (vdeque_push_bottom(&g_vdeque, (void *)enq_count) ==
            VDEQUE_STATE_OK) {
            enq_count++;
        }
    }

    for (vsize_t i = 0; i < 3; i++) {
        if (vdeque_pop_bottom(&g_vdeque, (void **)&buf) == VDEQUE_STATE_OK) {
            deq_count++;
            deq_data_sum += buf;
        }
    }

    for (vsize_t i = 0; i < 1; i++) {
        if (vdeque_push_bottom(&g_vdeque, (void *)enq_count) ==
            VDEQUE_STATE_OK) {
            enq_count++;
        }
    }

    free(arg);
    return NULL;
}

static void
init(void)
{
    vuint64_t array_size = vdeque_memsize(ARRAY_SIZE);
    vdeque_array_t *a    = (vdeque_array_t *)malloc(array_size);
    vdeque_init(&g_vdeque, a, ARRAY_SIZE);
}

int
main(void)
{
    init();

    for (vsize_t i = 0; i < 3; i++) {
        if (vdeque_push_bottom(&g_vdeque, (void *)enq_count) ==
            VDEQUE_STATE_OK) {
            enq_count++;
        }
    }

    pthread_t stealer[STEALER_NUM], self;

    if (STEALER_NUM != 0) {
        for (vsize_t i = 0; i < STEALER_NUM; i++) {
            vsize_t *arg = (vsize_t *)malloc(sizeof(vsize_t));
            *arg         = i;
            pthread_create(&stealer[i], NULL, steal_func, (void *)arg);
        }
    }
    pthread_create(&self, NULL, self_func, NULL);

    if (STEALER_NUM != 0) {
        for (vsize_t i = 0; i < STEALER_NUM; i++) {
            pthread_join(stealer[i], NULL);
        }
    }
    pthread_join(self, NULL);

    vuint64_t buf = 0;

    /* dequeue the rest */
    for (vsize_t i = 0; i < 4; i++) {
        if (vdeque_pop_bottom(&g_vdeque, (void **)&buf) == VDEQUE_STATE_OK) {
            deq_count++;
            deq_data_sum += buf;
        }
    }

    /* check results */
    vuint64_t steal_count_all    = 0;
    vuint64_t steal_data_sum_all = 0;
    for (vsize_t i = 0; i < STEALER_NUM; i++) {
        steal_count_all += steal_count[i];
        steal_data_sum_all += steal_data_sum[i];
    }

    ASSERT(deq_count + steal_count_all == enq_count);
    ASSERT(deq_data_sum + steal_data_sum_all ==
           enq_count * (enq_count - 1) / 2);

    free((vdeque_array_t *)vatomicptr_read_rlx(&g_vdeque.array));
    return 0;
}
