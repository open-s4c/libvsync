/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TREESET_LOCK_H
#define VSYNC_TREESET_LOCK_H

#ifdef VSYNC_VERIFICATION
    #define TREESET_LOCK_PTHREAD
#endif

struct lock_s;

static inline void l_init(struct lock_s *lock);

static inline void l_destroy(struct lock_s *lock);

static inline void l_acquire(struct lock_s *lock);

static inline void l_release(struct lock_s *lock);

static inline void l_reader_acquire(struct lock_s *lock);

static inline void l_reader_release(struct lock_s *lock);

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
