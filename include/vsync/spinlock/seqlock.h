/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_SEQLOCK_H
#define VSYNC_SEQLOCK_H

/*******************************************************************************
 * @file  seqlock.h
 * @brief Multi-writer multi-reader optimistic lock.
 *
 * In seqlock writers are only blocked by writers but not by readers.
 * Readers optimistically read shared variables and subsequently
 * check their consistency. If any of the shared variables has been updated
 * while being read, the readers must retry.
 *
 * @example
 * @include eg_seqlock.c
 *
 * @note If you have a single writer seqcount.h might be enough.
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/common/assert.h>
#include <vsync/utils/math.h>
#include <vsync/vtypes.h>

typedef vuint32_t seqvalue_t;

typedef struct seqlock_s {
    vatomic32_t
        seqcount; /* odd value indicates that a writer has acquired the lock */
} seqlock_t;

/** Initializer of `seqlock_t`. */
#define SEQ_LOCK_INIT()                                                        \
    {                                                                          \
        .seqcount = VATOMIC_INIT(0)                                            \
    }

/**
 * Awaits the sequence value to become even.
 *
 * @param seq  address of seqvalue_t object.
 * @return seqvalue_t current sequence value.
 */
static inline seqvalue_t
_seqlock_await_even(seqlock_t *seq)
{
    seqvalue_t count = vatomic32_read_acq(&seq->seqcount);
    while (VIS_ODD(count)) {
        count = vatomic32_await_neq_acq(&seq->seqcount, count);
    }
    ASSERT(VIS_EVEN(count));
    return count;
}
/**
 * Initializes the seqlock.
 *
 * @param seq address of seqlock_t object.
 *
 * @note alternatively use `SEQ_LOCK_INIT`.
 */
static inline void
seqlock_init(seqlock_t *seq)
{
    vatomic32_write_rlx(&seq->seqcount, 0);
}
/**
 * Acquires the lock in write-mode.
 *
 * @param seq address of seqlock_t object.
 *
 * @post once writers are done accessing the critical section, they
 * must call `seqlock_release`.
 *
 * @note only writers need to call this function, in order to enter the
 * critical section.
 */
static inline void
seqlock_acquire(seqlock_t *seq)
{
    seqvalue_t count   = 0;
    seqvalue_t o_count = 0;

    do {
        /* wait for the value to become even */
        count = _seqlock_await_even(seq);

        /* attempt to increment to an odd value, to acquire the lock */
        o_count = vatomic32_cmpxchg_acq(&seq->seqcount, count, count + 1);
        /* if the cas succeeds then we are done, otherwise, it means another
         * writer was faster in acquiring the lock */
        if (o_count == count) {
            break;
        }
    } while (true);

    vatomic_fence_rel();
}
/**
 * Releases the lock in write-mode.
 *
 * @param seq address of seqlock_t object.
 *
 * @pre prior to accessing the critical section, writers must call
 * `seqlock_acquire`.
 *
 * @note only writers need to call this function, once they are done accessing
 * the critical section.
 */
static inline void
seqlock_release(seqlock_t *seq)
{
    /* we read the current seqcount (expected to be odd), and then increment it
     * to make it even */
    seqvalue_t cur_val = vatomic32_read_rlx(&seq->seqcount);
    ASSERT((cur_val & 0x1UL) == 0x1UL);
    vatomic32_write_rel(&seq->seqcount, cur_val + 1);
    /* @TODO: check if changing this to vatomic32_inc would degrade the
     * performance */
}
/**
 * Begins reader critical section.
 *
 * This function returns a value that must be later passed
 * to `seqlock_rend`.
 *
 * @param seq address of seqlock_t object.
 *
 * @return `seqvalue_t` an unsigned integer value.
 *
 * @post readers must call `seqlock_rend` once they are done accessing the
 * critical section.
 */
static inline seqvalue_t
seqlock_rbegin(seqlock_t *seq)
{
    return _seqlock_await_even(seq);
}
/**
 * Ends reader critical section.
 *
 * @param seq address of seqlock_t object.
 * @param sv the value `seqlock_rbegin` returned, before the read attempt
 * @return true, if no write happened in between.
 * @return false, if a write happened in between, and repeating the CS is
 * required to read consistent data.
 *
 * @pre readers must call `seqlock_rbegin` before reading critical section
 * data.
 */
static inline vbool_t
seqlock_rend(seqlock_t *seq, seqvalue_t sv)
{
    ASSERT(VIS_EVEN(sv));
    vatomic_fence_acq();
    /* If the seqcount is still the same, it means that no writer has
     * attempted to write and thus the read data is consistent  */
    return vatomic32_read_rlx(&seq->seqcount) == sv;
}
#endif
