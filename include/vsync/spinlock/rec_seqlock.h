/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_REC_SEQLOCK_H
#define VSYNC_REC_SEQLOCK_H
/*******************************************************************************
 * @file rec_seqlock.h
 * @ingroup reentrant
 * @brief Recursive seqlock implementation using recursive.h
 *
 * In a rec_seqlock writers are only blocked by writers, not by readers.
 * Readers optimistically read the shared variables and subsequently
 * check their consistency. If any of the shared variables has been updated
 * while being read, the readers must retry
 *
 * This is a reentrant implementation of @see seqlock.h.
 *
 *
 * @example
 * @include  eg_rec_seqlock.c
 *
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/common/cache.h>
#include <vsync/spinlock/seqlock.h>
#include <vsync/utils/recursive_lock.h>

/** @cond DO_NOT_DOCUMENT */
DEF_RECURSIVE_LOCK(rec_seqlock, seqlock_t, seqlock_init, seqlock_acquire,
                   seqlock_release, WITHOUT_TRYACQUIRE, WITHOUT_CONTEXT)
/** @endcond */

/** Initializer of `rec_seqlock`. */
#define REC_SEQLOCK_INIT() RECURSIVE_LOCK_INIT(SEQ_LOCK_INIT())

/**
 * Initializes the given recursive seqlock.
 *
 * @param l address of rec_seqlock_t object.
 */
static inline void rec_seqlock_init(rec_seqlock_t *l);
/**
 * Acquires the given recursive seqlock.
 *
 * @param l address of rec_seqlock_t object.
 * @param id thread ID or core ID.
 *
 * @note called by writer threads.
 */
static inline void rec_seqlock_acquire(rec_seqlock_t *l, vuint32_t id);
/**
 * Releases the given recursive seqlock.
 *
 * @param l address of rec_seqlock_t object.
 *
 * @note called by writer threads.
 */
static inline void rec_seqlock_release(rec_seqlock_t *l);
/**
 * Marks beginning of reader critical section.
 *
 * Readers must call this function, before attempting to read. *
 * This function returns a value that must be later passed
 * to `rec_seqlock_rend`.
 *
 * @post readers must call `rec_seqlock_rend` once they are done accessing the
 * critical section.
 *
 * @param l address of rec_seqlock_t object.
 * @return `seqvalue_t` an unsigned integer value.
 *
 * @note called by reader threads.
 */
static inline seqvalue_t
rec_seqlock_rbegin(rec_seqlock_t *l)
{
    return seqlock_rbegin(&l->lock);
}
/**
 * Ends reader critical section.
 *
 * Users should rely on the return value to decide if repeating is necessary.
 *
 * @pre readers must call `rec_seqlock_rbegin` before reading critical section
 * data.
 *
 * @param l address of rec_seqlock_t object.
 * @param sv the value `rec_seqlock_rbegin` returned, before the read attempt
 * @return true read data is consistent.
 * @return false read data is inconsistent, reader must reattempt the read.
 *
 * @note called by reader threads.
 */
static inline vbool_t
rec_seqlock_rend(rec_seqlock_t *l, seqvalue_t sv)
{
    return seqlock_rend(&l->lock, sv);
}

#endif
