/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#include <vsync/spinlock/ticketlock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

ticketlock_t g_lock = TICKETLOCK_INIT();
vuint32_t g_x       = 0;
vuint32_t g_y       = 0;

void *
run(void *args)
{
    ticketlock_acquire(&g_lock);
    g_x++;
    g_y++;
    ticketlock_release(&g_lock);

    (void)args;
    return NULL;
}

int
main(void)
{
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
