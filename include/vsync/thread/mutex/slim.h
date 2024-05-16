/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTHREAD_MUTEX_SLIM_H
#define VTHREAD_MUTEX_SLIM_H
/*******************************************************************************
 * @file slim.h
 * @brief Slim 3-state futex.
 *
 * @example
 * @include eg_mutex.c
 *
 * @note replace `#include <vsync/thread/mutex.h>` with
 *   `#include <vsync/thread/mutex/slim.h>` in the example above.
 *
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/thread/internal/futex.h>

typedef vatomic32_t vmutex_t;

/**
 * Initializes the mutex `m`.
 *
 * @param m address of vmutex_t object.
 */
static inline void
vmutex_init(vmutex_t *m)
{
    vatomic32_init(m, 0);
}
/**
 * Acquires the mutex `m`.
 *
 * @param m address of vmutex_t object.
 */
static inline void
vmutex_acquire(vmutex_t *m)
{
    if (vatomic32_cmpxchg_acq(m, 0U, 1U) == 0U) {
        return;
    }

    while (vatomic32_xchg_acq(m, 2U) != 0) {
        vfutex_wait(m, 2U);
    }
}
/**
 * Releases the mutex `m`.
 *
 * @param m address of vmutex_t object.
 */
static inline void
vmutex_release(vmutex_t *m)
{
    if (vatomic32_xchg_rel(m, 0U) == 1U) {
        return;
    }
    vfutex_wake(m, FUTEX_WAKE_ONE);
}
#endif
