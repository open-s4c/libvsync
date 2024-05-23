/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTHREAD_MUTEX_3STATE_H
#define VTHREAD_MUTEX_3STATE_H
/*******************************************************************************
 * @file tristate.h
 * @brief 3-state mutex.
 *
 * @example
 * @include eg_mutex.c
 *
 * @note replace `#include <vsync/thread/mutex.h>` with
 *   `#include <vsync/thread/mutex/tristate.h>` in the example above.
 *
 * @cite [Ulrich Drepper - Futexes Are Tricky]
 * (https://cis.temple.edu/~ingargio/old/cis307s07/readings/futex.pdf)
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
    vatomic32_init(m, 0U);
}
/**
 * Acquires the mutex `m`.
 *
 * @param m address of vmutex_t object.
 */
static inline void
vmutex_acquire(vmutex_t *m)
{
    if (vatomic32_cmpxchg_acq(m, 0, 1U) == 0U) {
        return;
    }

    do {
        vatomic32_cmpxchg_rlx(m, 1U, 2U);
        vfutex_wait(m, 2U);
    } while (vatomic32_cmpxchg_acq(m, 0U, 2U) != 0U);
}
/**
 * Releases the mutex `m`.
 *
 * @param m address of vmutex_t object.
 */
static inline void
vmutex_release(vmutex_t *m)
{
    if (vatomic32_cmpxchg_rel(m, 1U, 0U) == 1U) {
        return;
    }

    vatomic32_write_rel(m, 0U);
    vfutex_wake(m, FUTEX_WAKE_ONE);
}

#endif
