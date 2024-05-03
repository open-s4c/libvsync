/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_REC_TICKETLOCK_H
#define VSYNC_REC_TICKETLOCK_H
/*******************************************************************************
 * @file rec_ticketlock.h
 * @ingroup fair_lock reentrant
 * @brief Recursive ticketlock implementation using recursive.h
 *
 * `rec_ticketlock` is a ticketlock that supports recursion, i.e., is reentrant.
 *
 * see vsync/spinlock/ticketlock.h
 *
 * @example
 * @include eg_rec_ticketlock.c
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/common/cache.h>
#include <vsync/spinlock/ticketlock.h>
#include <vsync/utils/recursive_lock.h>
#include <vsync/vtypes.h>

/** @cond DO_NOT_DOCUMENT */
DEF_RECURSIVE_LOCK(rec_ticketlock, ticketlock_t, ticketlock_init,
                   ticketlock_acquire, ticketlock_release,
                   WITH_TRYACQUIRE(ticketlock_tryacquire), WITHOUT_CONTEXT)
/** @endcond */

/** Initializer of `rec_ticketlock`. */
#define REC_TICKETLOCK_INIT() RECURSIVE_LOCK_INIT(TICKETLOCK_INIT())

/**
 * Initializes the recursive ticketlock.
 *
 * @param l address of rec_ticketlock_t object.
 *
 * @note alternatively use `REC_TICKETLOCK_INIT`.
 */
static inline void rec_ticketlock_init(rec_ticketlock_t *l);
/**
 * Acquires the recursive ticketlock.
 *
 * This function can be called multiple times by the same thread, consecutively.
 *
 * @param l address of rec_ticketlock_t object.
 * @param id thread ID or core ID.
 */
static inline void rec_ticketlock_acquire(rec_ticketlock_t *l, vuint32_t id);
/**
 * Releases the recursive ticketlock.
 *
 * To fully release the lock, this function should be called the same number of
 * times as the acquire was consecutively called by thread that currently owns
 * the lock.
 *
 * @param l address of rec_ticketlock_t object.
 */
static inline void rec_ticketlock_release(rec_ticketlock_t *l);
/**
 * Tries to acquire the recursive ticketlock.
 *
 * This can be called multiple times by the same thread, consecutively.
 *
 * @param l address of rec_ticketlock_t object.
 * @param id thread ID or core ID.
 * @return true, if lock is acquired successfully.
 * @return false, if failed to acquire the lock.
 */
static inline vbool_t rec_ticketlock_tryacquire(rec_ticketlock_t *l,
                                                vuint32_t id);
#endif
