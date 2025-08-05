/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#include <vsync/spinlock/arraylock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

/**
 * It is required that the length of the flags array is
 * a power of two, greater or equal to the maximum number
 * of threads.
 * For N = 12, 16 is enough.
 */
#define NEXT_POWER_2 16U

arraylock_flag_t g_flags[NEXT_POWER_2];
arraylock_t g_lock;

vuint32_t g_x = 0;
vuint32_t g_y = 0;

void *
run(void *args)
{
    vuint32_t slot = 0;

    arraylock_acquire(&g_lock, &slot);
    g_x++;
    g_y++;
    arraylock_release(&g_lock, slot);

    (void)args;
    return NULL;
}

int
main(void)
{
    arraylock_init(&g_lock, g_flags, NEXT_POWER_2);

    pthread_t threads[N];

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    ASSERT(g_x == EXPECTED_VAL);
    ASSERT(g_x == g_y);
    printf("Final value %u\n", g_x);
    return 0;
}
