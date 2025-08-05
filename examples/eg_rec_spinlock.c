/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#include <vsync/spinlock/rec_spinlock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

rec_spinlock_t g_lock = REC_SPINLOCK_INIT();
vuint32_t g_x         = 0;
vuint32_t g_y         = 0;

void *
run(void *args)
{
    vuint32_t tid = (vuint32_t)(vsize_t)args;
    // can be acquired multiple times without causing dead-lock
    rec_spinlock_acquire(&g_lock, tid);
    rec_spinlock_acquire(&g_lock, tid);
    g_x++;
    g_y++;
    rec_spinlock_release(&g_lock);
    rec_spinlock_release(&g_lock);

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
