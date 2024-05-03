/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_SEMAPHORE_H
#define VSYNC_SEMAPHORE_H
/*******************************************************************************
 * @file  semaphore.h
 * @brief Counting Semaphore.
 *
 * see vsync/spinlock/rwlock.h
 *
 * @example
 * @include eg_semaphore.c
 *
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/vtypes.h>

typedef struct semaphore_s {
    vatomic32_t s;
} semaphore_t;

/** Initializer of `semaphore_t` with n resources. */
#define SEMAPHORE_INIT(n)                                                      \
    {                                                                          \
        .s = VATOMIC_INIT(n)                                                   \
    }

/**
 * Initializes the semaphore.
 *
 * @param s address of semaphore_t object.
 * @param n number of resources.
 *
 * @note alternatively use `SEMAPHORE_INIT`
 */
static inline void
semaphore_init(semaphore_t *s, vuint32_t n)
{
    vatomic32_write(&s->s, n);
}
/**
 * Acquires i resources of the semaphore if available.
 *
 * other blocks until enough resources are available in the semaphore.
 *
 * @param s address of semaphore_t object.
 * @param i number of resources to acquire.
 *
 * @pre call `semaphore_acquire` with i <= N.
 */
static inline void
semaphore_acquire(semaphore_t *s, vuint32_t i)
{
    // awaits the subtraction does not overflow, then subtract.
    // This could be implemented with a cmpxchg loop as well, but
    // vatomic32_await_ge_sub politely waits with WFE instruction.
    vatomic32_await_ge_sub_acq(&s->s, i, i);
}
/**
 * Tries to acquire i resources of the semaphore if available.
 *
 * @param s address of semaphore_t object.
 * @param i number of resources to acquire.
 * @return true, if i resources were acquired.
 * @return false, if failed to acquire i resources.
 *
 * @pre call `semaphore_tryacquire` with i <= N.
 */
static inline vbool_t
semaphore_tryacquire(semaphore_t *s, vuint32_t i)
{
    vuint32_t old = vatomic32_read_rlx(&s->s);
    if (old < i) {
        return false;
    }
    return vatomic32_cmpxchg_acq(&s->s, old, old - i) == old;
}
/**
 * Releases i resources of the semaphore.
 *
 * @param s address of semaphore_t object.
 * @param i number of resources to release.
 */
static inline void
semaphore_release(semaphore_t *s, vuint32_t i)
{
    vatomic32_add_rel(&s->s, i);
}
#endif
