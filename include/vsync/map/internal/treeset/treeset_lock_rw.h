/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TREESET_LOCK_RW
#define VSYNC_TREESET_LOCK_RW

#include <vsync/spinlock/rwlock.h>

#define TREESET_RW_LOCK_DEFINED

typedef struct lock_s {
    rwlock_t lock;
} lock_t;

static inline void
l_init(lock_t *lock)
{
    rwlock_init(&lock->lock);
}

static inline void
l_destroy(lock_t *lock)
{
    (void)lock;
}

static inline void
l_acquire(lock_t *lock)
{
    rwlock_write_acquire(&lock->lock);
}

static inline void
l_release(lock_t *lock)
{
    rwlock_write_release(&lock->lock);
}

static inline void
l_reader_acquire(lock_t *lock)
{
    rwlock_read_acquire(&lock->lock);
}

static inline void
l_reader_release(lock_t *lock)
{
    rwlock_read_release(&lock->lock);
}

#endif
