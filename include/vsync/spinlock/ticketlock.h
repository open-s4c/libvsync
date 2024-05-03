/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TICKETLOCK_H
#define VSYNC_TICKETLOCK_H
/*******************************************************************************
 * @file ticketlock.h
 * @ingroup fair_lock
 * @brief Classic ticketlock with two 32-bit variables (owner and next).
 *
 * Ticketlock is a simple, fair lock (guarantees FIFO order).
 *
 * @example
 * @include eg_ticketlock.c
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/vtypes.h>

typedef struct ticketlock_s {
    vatomic32_t next;
    vatomic32_t owner;
} ticketlock_t;

/* Initializer of `ticketlock_t`. */
#define TICKETLOCK_INIT()                                                      \
    {                                                                          \
        .next = VATOMIC_INIT(0), .owner = VATOMIC_INIT(0)                      \
    }

/**
 * Initializes the ticketlock.
 *
 * @param l address of ticketlock_t object.
 *
 * @note alternatively use `TICKETLOCK_INIT`.
 */
static inline void
ticketlock_init(ticketlock_t *l)
{
    vatomic32_init(&l->next, 0);
    vatomic32_init(&l->owner, 0);
}
/**
 * Acquires the ticketlock.
 *
 * @param l address of ticketlock_t object.
 */
static inline void
ticketlock_acquire(ticketlock_t *l)
{
    vuint32_t ticket = vatomic32_get_inc_rlx(&l->next);
    vatomic32_await_eq_acq(&l->owner, ticket);
}
/**
 * Tries to acquire the ticketlock.
 *
 * @param l address of ticketlock_t object.
 * @return true, if lock is acquired successfully.
 * @return false, if failed to acquire the lock.
 */
static inline vbool_t
ticketlock_tryacquire(ticketlock_t *l)
{
    vuint32_t o = vatomic32_read_acq(&l->owner);
    vuint32_t n = vatomic32_cmpxchg_rlx(&l->next, o, o + 1);
    return n == o;
}
/**
 * Releases the ticketlock.
 *
 * @param l address of ticketlock_t object.
 */
static inline void
ticketlock_release(ticketlock_t *l)
{
    vuint32_t owner = vatomic32_read_rlx(&l->owner);
    vatomic32_write_rel(&l->owner, owner + 1);
}
/**
 * Checks whether there are threads waiting to acquire the lock.
 *
 * This can only be called by the thread holding the lock, ie, after acquire but
 * before release.
 *
 * @return true, if threads are waiting to acquire the lock.
 * @return false, otherwise.
 */
static inline vbool_t
ticketlock_has_waiters(ticketlock_t *l)
{
    return (vatomic32_read_rlx(&l->next) - vatomic32_read_rlx(&l->owner)) > 1;
}
#endif
