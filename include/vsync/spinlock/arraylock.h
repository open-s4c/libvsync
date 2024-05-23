/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_ARRAYLOCK_H
#define VSYNC_ARRAYLOCK_H
/*******************************************************************************
 * @file arraylock.h
 * @ingroup fair_lock
 * @brief Simple array-based queue lock
 *
 * The array lock has an array of flags, each slot is associated with a  thread.
 * If we have `N` threads then the array has `N` slots. Each slot
 * represents a boolean flag indicating the associated thread's permission to
 * acquire the lock. The thread waits on its flag to become `true` to acquire
 * the lock. The thread releases the lock, by giving away its permission to the
 * next thread, i.e. sets its own flag to `false` and the flag of the one next
 * in line to `true`.
 *
 * Initially the first flag is set to `true` and the rest to `false`, and the
 * tail holds the index of the first slot.
 *
 * @example
 * @include eg_arraylock.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 7.5.1]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/common/cache.h>
#include <vsync/utils/math.h>

typedef struct arraylock_flag_s {
    vatomic8_t flag;
} VSYNC_CACHEALIGN arraylock_flag_t;

typedef struct arraylock_s {
    vatomic8_t tail; /* The virtual index of the slot that belongs to the
                         current thread */
    arraylock_flag_t *flags; /* An array that holds a slot for each thread,
                                indicating its permission */
    vuint32_t flags_len;     /* length of flags */
} arraylock_t;

/**
 * Initializes the given lock object.
 *
 * @param lock address of arraylock_t object.
 * @param flags address of arraylock_flag_t object.
 * @param len next power of two greater or equal to the maximum number of
 * threads. Length of `flags`array.
 *
 * @note `len` MUST BE A POWER OF TWO. This is required because we use a
 * circular array and take advantage of unsigned integers "overflow".
 */
static inline void
arraylock_init(arraylock_t *lock, arraylock_flag_t *flags, const vuint32_t len)
{
    vuint32_t i = 0;

    vatomic8_init(&lock->tail, 0);
    ASSERT(len > 0);
    ASSERT(V_IS_POWER_OF_TWO(len));
    lock->flags_len = len;
    lock->flags     = flags;

    /* first slot init as one */
    vatomic8_init(&lock->flags[0].flag, 1);

    /* rest init as zeros */
    for (i = 1; i < lock->flags_len; i++) {
        vatomic8_init(&lock->flags[i].flag, 0);
    }
}
/**
 * Acquires the given lock.
 *
 * @param lock address of arraylock_t object.
 * @param slot output parameter. Stores the index of the slot associates with
 * the calling thread.
 *
 * @note the same value returned in `slot` must be passed intact to
 * `arraylock_release`.
 */
static inline void
arraylock_acquire(arraylock_t *lock, vuint32_t *slot)
{
    // we take the slot with the tail index, and increment the tail by 1
    // note that if we reached the maximum index we will go back to index 0
    // thus slot = tail % lock->size. Because of unsigned overflow, lock->size
    // must be a power of two, and this was ensured in the initialization.

    vuint32_t tail = vatomic8_get_inc(&lock->tail);
    *slot          = tail % lock->flags_len;

    // wait until you have permission
    await_while (vatomic8_read_acq(&lock->flags[*slot].flag) == 0U) {}
}
/**
 * Releases the given lock.
 *
 * @param lock address of arraylock_t object.
 * @param slot index of the slot associates with the calling thread.
 */
static inline void
arraylock_release(arraylock_t *lock, vuint32_t slot)
{
    vuint32_t next = 0;
    // Give away permission
    vatomic8_write_rlx(&lock->flags[slot].flag, 0);
    // Calculate the index of the next in line thread's slot
    next = (slot + 1) % lock->flags_len;
    // Grant the permission to the next in line thread
    vatomic8_write_rel(&lock->flags[next].flag, 1);
}
#endif
