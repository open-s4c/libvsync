/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TREESET_LOCK_FAKE
#define VSYNC_TREESET_LOCK_FAKE

typedef struct lock_s {
    vbool_t locked;
} lock_t;

static inline void
l_init(lock_t *lock)
{
    lock->locked = false;
}

static inline void
l_destroy(lock_t *lock)
{
    ASSERT(!lock->locked);
}

static inline void
l_acquire(lock_t *lock)
{
    ASSERT(!lock->locked);
    lock->locked = true;
}

static inline void
l_release(lock_t *lock)
{
    ASSERT(lock->locked);
    lock->locked = false;
}

#endif
