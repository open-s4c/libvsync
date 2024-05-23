/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TREESET_LOCK_PTHREAD
#define VSYNC_TREESET_LOCK_PTHREAD

#include <pthread.h>

typedef struct lock_s {
    pthread_mutex_t lock;
} lock_t;

static inline void
l_init(lock_t *lock)
{
    pthread_mutex_init(&lock->lock, NULL);
}

static inline void
l_destroy(lock_t *lock)
{
    pthread_mutex_destroy(&lock->lock);
}

static inline void
l_acquire(lock_t *lock)
{
    pthread_mutex_lock(&lock->lock);
}

static inline void
l_release(lock_t *lock)
{
    pthread_mutex_unlock(&lock->lock);
}

#endif
