/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTHREAD_MUTEX_SLIM_H
#define VTHREAD_MUTEX_SLIM_H
/*******************************************************************************
 * @file slim.h
 * @brief Slim 3-state futex
 *
 * @example
 * @include eg_mutex.c
 *
 * @note change `#include <vsync/thread/mutex.h>` into
 *   `#include <vsync/thread/mutex/slim.h>` in the above example.
 *
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/thread/internal/futex.h>

typedef vatomic32_t vmutex_t;

static inline void
vmutex_init(vmutex_t *m)
{
    vatomic32_init(m, 0);
}

static inline void
vmutex_acquire(vmutex_t *m)
{
    if (vatomic32_cmpxchg_acq(m, 0, 1) == 0) {
        return;
    }

    while (vatomic32_xchg_acq(m, 2) != 0) {
        vfutex_wait(m, 2);
    }
}

static inline void
vmutex_release(vmutex_t *m)
{
    if (vatomic32_xchg_rel(m, 0) == 1) {
        return;
    }
    vfutex_wake(m, FUTEX_WAKE_ONE);
}

#endif
