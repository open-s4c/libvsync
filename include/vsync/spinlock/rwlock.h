/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_RWLOCK_WP_H
#define VSYNC_RWLOCK_WP_H

/*******************************************************************************
 * @file rwlock.h
 * @brief Write-preferring rwlock based on semaphore and an atomic vbool_t.
 *
 * see vsync/spinlock/semaphore.h
 *
 * When acquiring a read lock, wait for writers and then reserve one resource.
 * When acquiring a write lock, lock writer bit and reserve all N resources.
 *
 * Algorithm:
 *
 * ```
 * write_acquire:
 *	await (wb = 0)
 *	wb = 1
 *
 * 	await (resources = N)
 * 	resources = 0
 *
 * write_release:
 * 	wb = 0
 * 	resources = N
 *
 * read_acquire:
 *	await (wb = 0)
 *
 * 	await (resources > 0)
 * 	resources--
 *
 * read_release:
 * 	resources++
 * ```
 *
 * @example
 * @include eg_rwlock.c
 *
 ******************************************************************************/

#include <vsync/spinlock/semaphore.h>
#include <vsync/atomic.h>
#include <vsync/vtypes.h>

typedef struct rwlock_s {
    vuint32_t n;
    vatomic32_t wb;
    semaphore_t rs;
} rwlock_t;

#define RWLOCK_N (1U << 30U)

/** Initializer of `rwlock_t`. */
#define RWLOCK_INITN(N)                                                        \
    {                                                                          \
        .n = (N), .wb = VATOMIC_INIT(0), .rs = SEMAPHORE_INIT((N)),            \
    }

/** Initializer of `rwlock_t` with the default number of resources. */
#define RWLOCK_INIT() RWLOCK_INITN(RWLOCK_N)

/**
 * Initializes the rwlock.
 *
 * @param l address of rwlock_t object.
 *
 * @note alternatively use `RWLOCK_INIT`.
 */
static inline void
rwlock_init(rwlock_t *l)
{
    l->n = RWLOCK_N;
    vatomic32_init(&l->wb, 0);
    semaphore_init(&l->rs, RWLOCK_N);
}
/**
 * Acquires the write lock.
 *
 * @param l address of rwlock_t object.
 */
static inline void
rwlock_write_acquire(rwlock_t *l)
{
    vatomic32_await_eq_set_rlx(&l->wb, 0, 1U);
    semaphore_acquire(&l->rs, l->n); // acquire fence
}
/**
 * Tries to acquire the write lock.
 *
 * @param l address of rwlock_t object.
 * @return true, if lock is acquired successfully.
 * @return false, if failed to acquire the lock.
 */
static inline vbool_t
rwlock_write_tryacquire(rwlock_t *l)
{
    if (vatomic32_cmpxchg_rlx(&l->wb, 0, 1U) != 0) {
        /* would block because of another writer */
        return false;
    }
    if (!semaphore_tryacquire(&l->rs, l->n)) {
        /* would block because of a reader */
        vatomic32_write_rlx(&l->wb, 0);
        return false;
    }
    return true;
}
/**
 * Releases the write lock.
 *
 * @param l address of rwlock_t object.
 */
static inline void
rwlock_write_release(rwlock_t *l)
{
    vatomic32_write_rlx(&l->wb, 0);  // relaxed store
    semaphore_release(&l->rs, l->n); // release fence
}
/**
 * Acquires the read lock.
 *
 * @param l address of rwlock_t object.
 */
static inline void
rwlock_read_acquire(rwlock_t *l)
{
    vatomic32_await_eq_rlx(&l->wb, 0);
    semaphore_acquire(&l->rs, 1U); // acquire fence
}
/**
 * Tries to acquire the read lock.
 *
 * @param l address of rwlock_t object.
 * @return true, if lock is acquired successfully.
 * @return false, if failed to acquire the lock.
 */
static inline vbool_t
rwlock_read_tryacquire(rwlock_t *l)
{
    if (vatomic32_read_rlx(&l->wb)) { // relaxed read
        return false;
    }
    return semaphore_tryacquire(&l->rs, 1U); // acquire fence
}
/**
 * Releases the read lock.
 *
 * @param l address of rwlock_t object.
 */
static inline void
rwlock_read_release(rwlock_t *l)
{
    semaphore_release(&l->rs, 1U); // release fence
}
/**
 * Returns true if a writer has acquired the lock, or waiting on the readers to
 * release it.
 *
 * @param l address of rwlock_t object.
 * @return true a writer has acquired or waiting on readers to release the lock.
 * @return false the lock is not acquired by a writer.
 */
static inline vbool_t
rwlock_acquired_by_writer(rwlock_t *l)
{
    return vatomic32_read(&l->wb) > 0;
}
/**
 * Returns true if the lock is acquired by readers.
 *
 * @param  l address of rwlock_t object.
 * @return true if the lock is acquired by readers.
 * @return false if the lock is not acquired by readers.
 */
static inline vbool_t
rwlock_acquired_by_readers(rwlock_t *l)
{
    vuint32_t s = vatomic32_read(&l->rs.s);
    return s > 0 && s != l->n;
}
#endif
