/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTHREAD_MUTEX_MUSL_H
#define VTHREAD_MUTEX_MUSL_H

/*******************************************************************************
 * @file musl.h
 * @brief A simplified version of the mutex algorithm in musl libc.
 *
 * @example
 * @include eg_mutex.c
 *
 * @note replace `#include <vsync/thread/mutex.h>` with
 *   `#include <vsync/thread/mutex/musl.h>` in the example above.
 *
 * @cite [Check mutex implementation in libc]
 * (http://musl.libc.org)
 *
 * @note It is the normal mutex without support for reentrance.
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/thread/internal/futex.h>

/**
 * @def MUSL_MAX_SPIN
 * @brief times of spinning before going to the macro.
 *
 * default value is 100, compile with -DMUSL_MAX_SPIN=N to
 * overwrite the default.
 *
 * @note spinning is deactivated on verification.
 */
#ifndef MUSL_MAX_SPIN
    #define MUSL_MAX_SPIN 100
#endif

typedef struct {
    vatomic32_t lock;
    vatomic32_t waiters;
} vmutex_t;

/**
 * Initializes the mutex `m`.
 *
 * @param m address of vmutex_t object.
 */
static inline void
vmutex_init(vmutex_t *m)
{
    vatomic32_init(&m->lock, 0U);
    vatomic32_init(&m->waiters, 0U);
}
/**
 * Acquires the mutex `m`.
 *
 * @param m address of vmutex_t object.
 */
static inline void
vmutex_acquire(vmutex_t *m)
{
    if (vatomic32_cmpxchg_acq(&m->lock, 0U, 1U) == 0U) {
        return;
    }
    vatomic_fence_rlx(); // a_barrier of a_cas failure

#if !defined(VSYNC_VERIFICATION)
    vint32_t spin = MUSL_MAX_SPIN;
    while (spin-- && vatomic32_read_rlx(&m->lock) &&
           !vatomic32_read_rlx(&m->waiters)) {
        vatomic_cpu_pause();
    }
#endif

    while (vatomic32_cmpxchg_acq(&m->lock, 0U, 1U) != 0U) {
        vatomic_fence_rlx(); // a_cas failure
        vatomic32_inc_rlx(&m->waiters);
        if (vatomic32_cmpxchg_rlx(&m->lock, 1U, 2U) != 1U) {
            vatomic_fence_rlx(); // a_cas failure
        }
        vfutex_wait(&m->lock, 2U);
        vatomic32_dec_rlx(&m->waiters);
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
    vuint32_t old = vatomic32_xchg_rel(&m->lock, 0U);
    if (vatomic32_read_rlx(&m->waiters) > 0U || old != 1U) {
        vfutex_wake(&m->lock, FUTEX_WAKE_ONE);
    }
}
#endif
