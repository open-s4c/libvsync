/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTHREAD_FUTEX_MOCK_LIVENESS_H
#define VTHREAD_FUTEX_MOCK_LIVENESS_H

#include <vsync/common/assert.h>
#include <vsync/atomic.h>
#include <vsync/spinlock/caslock.h>

/*******************************************************************************
 * futex mock for verifying safety and liveness.
 *
 * This is a mock that accurately models the behavior of futex operations inside
 * kernel, but at the cost of higher overhead for model checkers.
 *
 ******************************************************************************/

typedef struct vfutex_waiter_s {
    vatomic32_t state;
    vatomic32_t *futex;
    struct vfutex_waiter_s *next;
} vfutex_waiter_t;

static caslock_t g_futex_lock;

#define VFUTEX_NUM_HEADS 16
vfutex_waiter_t *g_futex_head[VFUTEX_NUM_HEADS];

static inline void
vfutex_wait(vatomic32_t *m, vuint32_t v)
{
    vuintptr_t head = ((vuintptr_t)m) % VFUTEX_NUM_HEADS;
    vbool_t done    = false;

    vfutex_waiter_t w;
    vatomic32_write(&w.state, 1);
    w.futex = m;
    w.next  = NULL;

    caslock_acquire(&g_futex_lock);
    vuint32_t old = vatomic32_read_acq(m);
    if (old == v) {
        w.next             = g_futex_head[head];
        g_futex_head[head] = &w;
    } else {
        done = true;
    }
    caslock_release(&g_futex_lock);
    if (!done) {
        vatomic32_await_neq(&w.state, 1);
    }
}

static inline void
vfutex_wake(vatomic32_t *m, vuint32_t v)
{
    vuintptr_t head     = ((vuintptr_t)m) % VFUTEX_NUM_HEADS;
    vfutex_waiter_t **h = &g_futex_head[head];

    vuint32_t cnt = v;

    caslock_acquire(&g_futex_lock);
    while (*h != NULL && cnt != 0) {
        vfutex_waiter_t *w = *h;

        if (w->futex == m) {
            cnt--;
            *h = w->next;
            vatomic32_write(&w->state, 0);
        } else {
            h = &(w->next);
        }
    }
    caslock_release(&g_futex_lock);
}

static inline void
vfutex_requeue(vatomic32_t *m, vuint32_t val, vuint32_t req, vatomic32_t *n)
{
    vfutex_waiter_t **h = &g_futex_head[((vuintptr_t)m) % VFUTEX_NUM_HEADS];
    const vuintptr_t ins_head = ((vuintptr_t)n) % VFUTEX_NUM_HEADS;

    vuint32_t towake = val;
    vuint32_t tomove = req;

    caslock_acquire(&g_futex_lock);
    while (*h != NULL && (towake != 0 || tomove != 0)) {
        vfutex_waiter_t *w = *h;

        if (w->futex != m) {
            h = &(w->next);
        } else if (w->futex == m && towake != 0) {
            towake--;
            *h = w->next;
            vatomic32_write(&w->state, 0);
        } else if (w->futex == m) {
            ASSERT(towake == 0);
            ASSERT(tomove != 0);
            tomove--;
            *h                     = w->next;
            w->futex               = n;
            w->next                = g_futex_head[ins_head];
            g_futex_head[ins_head] = w;
        } else {
            ASSERT(0 && "Impossible situation (towake == 0 && tomove == 0).");
        }
    }
    caslock_release(&g_futex_lock);
}

#endif /* VTHREAD_FUTEX_MOCK_LIVENESS_H */
