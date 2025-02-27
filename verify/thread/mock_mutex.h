/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef _MOCK_MUTEX_
#define _MOCK_MUTEX_

#if 1
    #include <pthread.h>

typedef pthread_mutex_t vmutex_t;

static inline void
vmutex_init(vmutex_t *l)
{
    pthread_mutex_init(l, 0);
}

static inline void
vmutex_acquire(vmutex_t *l)
{
    pthread_mutex_lock(l);
}
static inline void
vmutex_release(vmutex_t *l)
{
    pthread_mutex_unlock(l);
}
#else
    #include <vsync/caslock.h>

typedef caslock_t vmutex_t;

vmutex_init(vmutex_t *l)
{
    caslock_init(l);
}

static inline void
vmutex_acquire(vmutex_t *l)
{
    caslock_acquire(l);
}
static inline void
vmutex_release(vmutex_t *l)
{
    caslock_release(l);
}
#endif
#endif
