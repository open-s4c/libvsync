/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTHREAD_COND_H
#define VTHREAD_COND_H
/*******************************************************************************
 * @file cond.h
 * @brief Condition variable
 *
 * A very simple condition variable.
 *
 * @example
 * @include eg_cond.c
 *
 * @note include mutex.h from libvsync before including cond.h. Alternatively,
 * users can implement the same interface with pthread_mutex_t or similar and
 * include that to be used by cond.h.
 *
 * @cite [Condition variable with futex]
 * (https://www.remlab.net/op/futex-condvar.shtml)
 *
 ******************************************************************************/

#include <vsync/vtypes.h>
#include <vsync/atomic.h>
#include <vsync/thread/internal/futex.h>

typedef struct vcond_s {
    vatomic32_t value;
} vcond_t;

/**
 * Initializes the given condition variable.
 *
 * @param c address of vcond_t object.
 */
static inline void
vcond_init(vcond_t *c)
{
    vatomic32_init(&c->value, 0);
}
/**
 * Waits on the given condition variable.
 *
 * Releases the mutex and waits till the condition variable is signaled, then
 * reacquires the mutex.
 *
 * @param c address of vcond_t object.
 * @param m address of vmutex_t object.
 */
static inline void
vcond_wait(vcond_t *c, vmutex_t *m)
{
    vuint32_t val = vatomic32_read_rlx(&c->value);
    vmutex_release(m);
    vfutex_wait(&c->value, val);
    vmutex_acquire(m);
}
/**
 * Signals the condition variable.
 *
 * Wakes up one sleeping thread waiting on the condition.
 *
 * @param c address of vcond_t object.
 */
static inline void
vcond_signal(vcond_t *c)
{
    vatomic32_inc_rlx(&c->value);
    vfutex_wake(&c->value, FUTEX_WAKE_ONE);
}

#endif
