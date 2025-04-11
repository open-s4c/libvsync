/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTHREAD_FUTEX_MOCK_LIVENESS_SIMPLE_H
#define VTHREAD_FUTEX_MOCK_LIVENESS_SIMPLE_H

#include <vsync/common/assert.h>
#include <vsync/atomic.h>
#include <vsync/spinlock/caslock.h>
#include <vsync/utils/math.h>

/*******************************************************************************
 * A simple futex mock for verifying safety and liveness.
 *
 * This mock has less overhead than `futex_mock_liveness.h` one, but at the cost
 * of not supporting futex_requeue operations and verification failures when
 * two futexes hash to the same bucket.
 *
 ******************************************************************************/

typedef struct vfutex_bucket_s {
    void *addr;
    vatomic32_t grant;
    vuint32_t ticket;
} vfutex_bucket_t;

#define VFUTEX_BUCKET_COUNT  16
#define VFUTEX_ADDR_HASH_DIV sizeof(vatomic32_t)

static vfutex_bucket_t g_vfutex_buckets[VFUTEX_BUCKET_COUNT];
static caslock_t g_vfutex_lock;

static inline void
vfutex_wait(vatomic32_t *a, vuint32_t v)
{
    vuintptr_t b_idx =
        ((vuintptr_t)a / VFUTEX_ADDR_HASH_DIV) % VFUTEX_BUCKET_COUNT;
    vfutex_bucket_t *b = &g_vfutex_buckets[b_idx];
    caslock_acquire(&g_vfutex_lock);
    if (vatomic32_read(a) != v) {
        caslock_release(&g_vfutex_lock);
        return;
    }

    if (b->addr == NULL) {
        b->addr = (void *)a;
    }
    ASSERT(b->addr == (void *)a &&
           "Collision of futex address hashes -- aborting.");

    vuint32_t t = b->ticket++;
    caslock_release(&g_vfutex_lock);
    vatomic32_await_ge(&b->grant, t);
}

static inline void
vfutex_wake(void *a, vuint32_t v)
{
    vuintptr_t b_idx =
        ((vuintptr_t)a / VFUTEX_ADDR_HASH_DIV) % VFUTEX_BUCKET_COUNT;
    vfutex_bucket_t *b = &g_vfutex_buckets[b_idx];
    caslock_acquire(&g_vfutex_lock);
    if (b->addr) {
        ASSERT(b->addr == a);
        vuint32_t g = vatomic32_read(&b->grant);
        if (b->ticket > g) {
            vatomic32_write(&b->grant, g + VMIN(b->ticket - g, v));
        }
    }
    caslock_release(&g_vfutex_lock);
}

#endif /* VTHREAD_FUTEX_MOCK_LIVENESS_SIMPLE_H */
