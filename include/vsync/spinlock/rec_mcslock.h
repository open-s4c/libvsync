/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_REC_MCSLOCK_H
#define VSYNC_REC_MCSLOCK_H
/*******************************************************************************
 * @file rec_mcslock.h
 * @ingroup fair_lock
 * @brief Recursive MCS lock implementation using recursive.h
 *
 * `rec_mcslock` is a MCS lock that supports recursion, i.e., is reentrant.
 *
 * see mcslock.h
 *
 * @example
 * @include eg_rec_mcslock.c
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/common/cache.h>
#include <vsync/spinlock/mcslock.h>
#include <vsync/utils/recursive_lock.h>

/** @cond DO_NOT_DOCUMENT */
DEF_RECURSIVE_LOCK(rec_mcslock, mcslock_t, mcslock_init, mcslock_acquire,
                   mcslock_release, WITH_TRYACQUIRE(mcslock_tryacquire),
                   WITH_CONTEXT(mcs_node_t))
/** @endcond */

/** Initializer of `rec_mcslock`. */
#define REC_MCSLOCK_INIT() RECURSIVE_LOCK_INIT(MCSLOCK_INIT())
/**
 * Initializes the recursive MCS lock.
 *
 * @param l address of rec_mcslock_t object.
 *
 * @note alternatively use `REC_MCSLOCK_INIT`.
 */
static inline void rec_mcslock_init(rec_mcslock_t *l);
/**
 * Acquires the recursive MCS lock.
 *
 * This function can be called multiple times by the same thread, consecutively.
 *
 * @param l address of rec_mcslock_t object.
 * @param id thread ID or core ID
 * @param ctx address of rec_mcslock_t object associated with the caller
 * thread/core.
 */
static inline void rec_mcslock_acquire(rec_mcslock_t *l, vuint32_t id,
                                       mcs_node_t *ctx);
/**
 * Releases the recursive MCS lock.
 *
 * To fully release the lock, this function should be called the same number of
 * times as the acquire was consecutively called by thread that currently owns
 * the lock.
 *
 * @param l address of rec_mcslock_t object.
 * @param ctx address of rec_mcslock_t object associated with the caller
 * thread/core.
 */
static inline void rec_mcslock_release(rec_mcslock_t *l, mcs_node_t *ctx);
/**
 * Tries to acquire the recursive mcslock.
 *
 * This can be called multiple times by the same thread, consecutively.
 *
 * @param l address of rec_mcslock_t object.
 * @param id thread ID or core ID.
 * @return true, if lock is acquired successfully.
 * @return false, if failed to acquire the lock.
 */
static inline vbool_t rec_mcslock_tryacquire(rec_mcslock_t *l, vuint32_t id,
                                             mcs_node_t *ctx);
#endif
