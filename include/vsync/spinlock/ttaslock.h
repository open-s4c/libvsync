/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TTAS_H
#define VSYNC_TTAS_H
/*******************************************************************************
 * @file  ttaslock.h
 * @brief Test, Test and Set lock.
 *
 * @example
 * @include eg_ttaslock.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming  7.3]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/vtypes.h>

typedef struct ttaslock_s {
    vatomic32_t state;
} ttaslock_t;

/** Initializer of `ttaslock_t`. */
#define TTASLOCK_INIT()                                                        \
    {                                                                          \
        .state = VATOMIC_INIT(0)                                               \
    }

/**
 * Initializes the TTAS lock.
 *
 * @param l address of ttaslock_t object.
 *
 * @note alternatively use `TTASLOCK_INIT`.
 */
static inline void
ttaslock_init(ttaslock_t *l)
{
    vatomic32_init(&l->state, 0);
}
/**
 * Acquires the TAS lock.
 *
 * @param l address of ttaslock_t object.
 */
static inline void
ttaslock_acquire(ttaslock_t *l)
{
    while (true) {
        vatomic32_await_eq_rlx(&l->state, 0);
        if (!vatomic32_xchg_acq(&l->state, 1)) {
            return;
        }
    }
}
/**
 * Tries to acquire the TTAS lock.
 *
 * @param l address of ttaslock_t object.
 * @return true, if lock is acquired successfully.
 * @return false, if failed to acquire the lock.
 */
static inline vbool_t
ttaslock_tryacquire(ttaslock_t *l)
{
    return vatomic32_xchg_acq(&l->state, 1) == 0;
}
/**
 * Releases the TTAS lock.
 *
 * @param l address of ttaslock_t object.
 */
static inline void
ttaslock_release(ttaslock_t *l)
{
    vatomic32_write_rel(&l->state, 0);
}
#endif
