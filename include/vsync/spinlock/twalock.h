/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TWALOCK_H
#define VSYNC_TWALOCK_H
/*******************************************************************************
 * @file twalock.h
 * @ingroup fair_lock
 * @brief Ticketlock with waiting array (TWA).
 *
 * To use the TWA lock, one must declare the global waiting array once in
 * the program. Use `TWALOCK_ARRAY_DECL` to declare the array where convenient.
 *
 * @example
 * @include eg_twalock.c
 *
 * @cite
 * Dice and Kogan -
 * [TWA - Ticket Locks Augmented with a Waiting Array, EuroPar-19]
 *
 ******************************************************************************/

#include <vsync/common/cache.h>
#include <vsync/atomic.h>

#define TWA_L 1U
#ifndef TWA_A
    #define TWA_A 4096U
#endif
#define TWA_DIFF(a, b) ((a) - (b))
#define TWA_HASH(t)    ((t) % TWA_A)

typedef struct twa_counter_s {
    vatomic32_t val;
} VSYNC_CACHEALIGN twa_counter_t;

/** @cond DO_NOT_DOCUMENT */

/** __twa_array is a waiting array shared among all TWA lock instances. To use
 * the TWA lock, one must declare the array once in the program. */
extern twa_counter_t __twa_array[TWA_A];

/** TWA_ARRAY(i) internal macro to access the i-th value of the array. */
#define TWA_ARRAY(i) &__twa_array[i].val

/** @endcond */

/** TWALOCK_ARRAY_DECL declares the global __twa_array variable. */
#define TWALOCK_ARRAY_DECL twa_counter_t __twa_array[TWA_A] VSYNC_CACHEALIGN;

typedef struct twalock_s {
    vatomic32_t ticket;
    vatomic32_t grant;
} twalock_t;

/** Initializer of `twalock_t`. */
#define TWALOCK_INIT()                                                         \
    {                                                                          \
        .ticket = VATOMIC_INIT(0), .grant = VATOMIC_INIT(0)                    \
    }

static inline void _twalock_acquire_slowpath(twalock_t *l, vuint32_t t);

/**
 * Initializes the given TWA lock.
 *
 * @param l address of twalock_t object.
 * @note alternatively use TWALOCK_INIT
 */
static inline void
twalock_init(twalock_t *l)
{
    vatomic32_init(&l->ticket, 0);
    vatomic32_init(&l->grant, 0);
}
/**
 * Acquires the given TWA lock.
 *
 * @param l address of twalock_t object.
 */
static inline void
twalock_acquire(twalock_t *l)
{
    vuint32_t tx = vatomic32_get_inc_rlx(&l->ticket);
    vuint32_t dx = TWA_DIFF(tx, vatomic32_read_acq(&l->grant));

    if (dx == 0) {
        return;
    }

    if (dx > TWA_L) {
        _twalock_acquire_slowpath(l, tx);
    }
    vatomic32_await_eq_acq(&l->grant, tx);
}
/**
 * Releases the given TWA lock.
 *
 * @param l address of twalock_t object.
 */
static inline void
twalock_release(twalock_t *l)
{
    vuint32_t k = vatomic32_read_rlx(&l->grant) + 1;
    vatomic32_write_rel(&l->grant, k);
    vatomic32_inc_rel(TWA_ARRAY(TWA_HASH(k + TWA_L)));
}
/**
 * Tries to acquire the given TWA lock.
 *
 * @param l address of twalock_t object.
 * @return true on success.
 * @return false on failure.
 */
static inline vbool_t
twalock_tryacquire(twalock_t *l)
{
    vuint32_t k = vatomic32_read_acq(&l->grant);
    return vatomic32_cmpxchg_rlx(&l->ticket, k, k + 1) == k;
}
/**
 * Acquires the lock via slow path.
 *
 * @param l address of twalock_t object.
 * @param t ticket number.
 */
static inline void
_twalock_acquire_slowpath(twalock_t *l, vuint32_t t)
{
    vuint32_t at = TWA_HASH(t);
    vuint32_t u  = vatomic32_read_acq(TWA_ARRAY(at));
    vuint32_t k  = vatomic32_read_rlx(&l->grant);
    vuint32_t dx = TWA_DIFF(t, k);

    while (dx > TWA_L) {
        u  = vatomic32_await_neq_acq(TWA_ARRAY(at), u);
        dx = TWA_DIFF(t, vatomic32_read_rlx(&l->grant));
    }
}

#undef TWA_L
#undef TWA_DIFF
#undef TWA_HASH
#undef TWA_ARRAY
#endif
