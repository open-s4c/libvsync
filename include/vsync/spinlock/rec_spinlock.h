/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_REC_SPINLOCK_H
#define VSYNC_REC_SPINLOCK_H
/*******************************************************************************
 * @file rec_spinlock.h
 * @ingroup reentrant
 * @brief Recursive spinlock implementation using recursive.h
 *
 * see caslock.h
 *
 * @example
 * @include eg_rec_spinlock.c
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/common/cache.h>
#include <vsync/spinlock/caslock.h>
#include <vsync/utils/recursive_lock.h>
#include <vsync/vtypes.h>

/** @cond DO_NOT_DOCUMENT */
DEF_RECURSIVE_LOCK(rec_spinlock, caslock_t, caslock_init, caslock_acquire,
                   caslock_release, WITH_TRYACQUIRE(caslock_tryacquire),
                   WITHOUT_CONTEXT)
/** @endcond */

/** Initializer of `rec_spinlock`. */
#define REC_SPINLOCK_INIT() RECURSIVE_LOCK_INIT(CASLOCK_INIT())
/**
 * Initializes the recursive spinlock.
 *
 * @param l address of rec_spinlock_t object.
 *
 * @note alternatively use `REC_SPINLOCK_INIT`.
 */
static inline void rec_spinlock_init(rec_spinlock_t *l);
/**
 * Tries to acquire the recursive spinlock.
 *
 * @param l address of rec_spinlock_t object.
 * @param id thread ID or core ID.
 * @return true, if lock is acquired successfully.
 * @return false, if failed to acquire the lock.
 */
static inline vbool_t rec_spinlock_tryacquire(rec_spinlock_t *l, vuint32_t id);
/**
 * Acquires the recursive spinlock.
 *
 * @param l  address of recursive spinlock lock object
 * @param id thread ID or core ID.
 */
static inline void rec_spinlock_acquire(rec_spinlock_t *l, vuint32_t id);
/**
 * Releases the recursive spinlock.
 *
 * @param l address of rec_spinlock_t object.
 */
static inline void rec_spinlock_release(rec_spinlock_t *l);
#endif
