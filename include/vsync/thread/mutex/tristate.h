/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTHREAD_MUTEX_3STATE_H
#define VTHREAD_MUTEX_3STATE_H
/*******************************************************************************
 * @file tristate.h
 * @brief 3-state mutex
 *
 * @example
 * ```c
 * 	#include <vsync/thread/mutex/tristate.h>
 *	vmutex_t mutex;
 *	void run(void) {
 *		vmutex_acquire(&mutex);
 *		...
 *		vmutex_release(&mutex);
 *	}
 * ```
 *
 * @cite Ulrich Drepper -- Futexes Are Tricky. 2004
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

    do {
        vatomic32_cmpxchg_rlx(m, 1, 2);
        vfutex_wait(m, 2);
    } while (vatomic32_cmpxchg_acq(m, 0, 2) != 0);
}

static inline void
vmutex_release(vmutex_t *m)
{
    if (vatomic32_cmpxchg_rel(m, 1, 0) == 1) {
        return;
    }

    vatomic32_write_rel(m, 0);
    vfutex_wake(m, FUTEX_WAKE_ONE);
}

#endif
