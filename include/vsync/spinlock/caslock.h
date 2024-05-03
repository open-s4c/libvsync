/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_CAS_H
#define VSYNC_CAS_H
/*******************************************************************************
 * @file caslock.h
 * @brief Simple spinlock based on compare-and-swap (CAS).
 *
 * CAS is implemented with vatomic32_cmpxchg or a variant.
 *
 * @example
 * @include eg_caslock.c
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/vtypes.h>

typedef struct caslock_s {
    vatomic32_t lock;
} caslock_t;

/** Initializer of `caslock_t`. */
#define CASLOCK_INIT()                                                         \
    {                                                                          \
        .lock = VATOMIC_INIT(0)                                                \
    }
/**
 * Initializes the CAS lock.
 *
 * @param l address of caslock_t object.
 *
 * @note alternatively use `CASLOCK_INIT`.
 */
static inline void
caslock_init(caslock_t *l)
{
    vatomic32_init(&l->lock, 0);
}
/**
 * Acquires the CAS lock.
 *
 * @param l address of caslock_t object.
 */
static inline void
caslock_acquire(caslock_t *l)
{
    vatomic32_await_eq_set_acq(&l->lock, 0, 1);
}
/**
 * Tries to acquire the CAS lock.
 *
 * @param l address of caslock_t object.
 * @return true, if lock is acquired successfully.
 * @return false, if failed to acquire the lock.
 */
static inline vbool_t
caslock_tryacquire(caslock_t *l)
{
    return vatomic32_cmpxchg_acq(&l->lock, 0, 1) == 0;
}
/**
 * Releases the CAS lock.
 *
 * @param l address of caslock_t object.
 */
static inline void
caslock_release(caslock_t *l)
{
    vatomic32_write_rel(&l->lock, 0);
}
#endif
