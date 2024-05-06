/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TREESET_LOCK_TTAS
#define VSYNC_TREESET_LOCK_TTAS

#include <vsync/spinlock/ttaslock.h>

typedef struct lock_s {
    ttaslock_t lock;
} lock_t;

static inline void
l_init(lock_t *lock)
{
    ttaslock_init(&lock->lock);
}

static inline void
l_destroy(lock_t *lock)
{
    (void)lock;
}

static inline void
l_acquire(lock_t *lock)
{
    ttaslock_acquire(&lock->lock);
}

static inline void
l_release(lock_t *lock)
{
    ttaslock_release(&lock->lock);
}

#endif
