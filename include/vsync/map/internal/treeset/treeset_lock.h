/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TREESET_LOCK
#define VSYNC_TREESET_LOCK

#ifdef VSYNC_VERIFICATION
    #define TREESET_LOCK_PTHREAD
#endif

#if defined TREESET_LOCK_PTHREAD
    #include <vsync/map/internal/treeset/treeset_lock_pthread.h>
#elif defined TREESET_LOCK_TTAS
    #include <vsync/map/internal/treeset/treeset_lock_ttas.h>
#elif defined TREESET_LOCK_RW
    #include <vsync/map/internal/treeset/treeset_lock_rw.h>
#elif defined TREESET_LOCK_FAKE
    #include <vsync/map/internal/treeset/treeset_lock_fake.h>
#else
    #error "Choose lock implementation by setting TREESET_LOCK_*"
#endif

static inline void l_init(lock_t *lock);

static inline void l_destroy(lock_t *lock);

static inline void l_acquire(lock_t *lock);

static inline void l_release(lock_t *lock);

static inline void l_reader_acquire(lock_t *lock);

static inline void l_reader_release(lock_t *lock);

#ifndef TREESET_RW_LOCK_DEFINED

static inline void
l_reader_acquire(lock_t *lock)
{
    l_acquire(lock);
}

static inline void
l_reader_release(lock_t *lock)
{
    l_release(lock);
}

#endif

#endif
