/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_RWLOCK_WP_H
#define VSYNC_RWLOCK_WP_H

/*******************************************************************************
 * @file rwlock.h
 * @brief rwlock simulation for verification usage
 *
 ******************************************************************************/

#include <pthread.h>
#include <vsync/atomic.h>
#include <vsync/vtypes.h>

#if !defined(RWLOCK_MAX_READERS)
    #define RWLOCK_MAX_READERS 3U
#endif

typedef struct rwlock_s {
    pthread_mutex_t lock[RWLOCK_MAX_READERS];
    vatomic8_t writer_active;
    vatomic32_t idx;
} rwlock_t;


__thread vuint32_t g_tid = RWLOCK_MAX_READERS;
static inline vuint32_t _rwlock_get_tid(rwlock_t *l);


static inline void
rwlock_init(rwlock_t *l)
{
    for (vsize_t i = 0; i < RWLOCK_MAX_READERS; i++) {
        pthread_mutex_init(&l->lock[i], NULL);
    }
}
/**
 * Acquires the write lock.
 *
 * @param l address of rwlock_t object.
 */
static inline void
rwlock_write_acquire(rwlock_t *l)
{
    vatomic8_write_rlx(&l->writer_active, 1U);
    for (vsize_t i = 0; i < RWLOCK_MAX_READERS; i++) {
        pthread_mutex_lock(&l->lock[i]);
    }
}
/**
 * Releases the write lock.
 *
 * @param l address of rwlock_t object.
 */
static inline void
rwlock_write_release(rwlock_t *l)
{
    vatomic8_write_rlx(&l->writer_active, 0U);
    for (vsize_t i = 0; i < RWLOCK_MAX_READERS; i++) {
        pthread_mutex_unlock(&l->lock[i]);
    }
}
/**
 * Acquires the read lock.
 *
 * @param l address of rwlock_t object.
 */
static inline void
rwlock_read_acquire(rwlock_t *l)
{
    vuint32_t idx = _rwlock_get_tid(l);
    pthread_mutex_lock(&l->lock[idx]);
}
/**
 * Releases the read lock.
 *
 * @param l address of rwlock_t object.
 */
static inline void
rwlock_read_release(rwlock_t *l)
{
    vuint32_t idx = _rwlock_get_tid(l);
    pthread_mutex_unlock(&l->lock[idx]);
}

static inline vuint32_t
_rwlock_get_tid(rwlock_t *l)
{
    if (g_tid == RWLOCK_MAX_READERS) {
        g_tid = vatomic32_get_inc(&l->idx);
        ASSERT(g_tid < RWLOCK_MAX_READERS);
    }
    return g_tid;
}

static inline vbool_t
rwlock_acquired_by_writer(rwlock_t *l)
{
    return vatomic8_read_rlx(&l->writer_active) == 1U;
}

static inline vbool_t
rwlock_acquired_by_readers(rwlock_t *l)
{
    V_UNUSED(l);
    return true;
}

#endif
