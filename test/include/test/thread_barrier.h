/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef THREAD_BARRIER_H
#define THREAD_BARRIER_H

#include <pthread.h>
#include <vsync/vtypes.h>

#if defined(__APPLE__)

    #include <vsync/atomic.h>

typedef struct {
    vatomic32_t cnt;
    vuint32_t target;
} vthread_barrier_t;

typedef vuint32_t vthread_barrierattr_t;

int
vthread_barrier_init(vthread_barrier_t *barrier, vthread_barrierattr_t *attr,
                     vuint32_t count)
{
    barrier->target = count;
    vatomic32_init(&barrier->cnt, 0);
    V_UNUSED(attr);
    return 0;
}
int
vthread_barrier_wait(vthread_barrier_t *barrier)
{
    vatomic32_inc(&barrier->cnt);
    vatomic32_await_ge(&barrier->cnt, barrier->target);
    return 0;
}

int
vthread_barrier_destroy(vthread_barrier_t *barrier)
{
    V_UNUSED(barrier);
    return 0;
}

#else

/* LINUX */

typedef pthread_barrier_t vthread_barrier_t;

typedef pthread_barrierattr_t vthread_barrierattr_t;

int
vthread_barrier_init(vthread_barrier_t *barrier, vthread_barrierattr_t *attr,
                     vuint32_t count)
{
    return pthread_barrier_init(barrier, attr, count);
}
int
vthread_barrier_wait(vthread_barrier_t *barrier)
{
    return pthread_barrier_wait(barrier);
}

int
vthread_barrier_destroy(vthread_barrier_t *barrier)
{
    return pthread_barrier_destroy(barrier);
}
#endif
#endif
