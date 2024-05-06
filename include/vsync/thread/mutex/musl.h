/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTHREAD_MUTEX_MUSL_H
#define VTHREAD_MUTEX_MUSL_H

/*******************************************************************************
 * This is a simplified version of the mutex algorithm in musl libc
 *   http://musl.libc.org
 *
 * It is the normal mutex without support for reentrance.
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/thread/internal/futex.h>

/* MUSL_ORIG - mimics the barriers of the original version of musl mutex. */
#ifdef MUSL_ORIG
    #undef vatomic32_cmpxchg_acqG
    #undef vatomic32_cmpxchg_rlx
    #undef vatomic32_xchg_rel
    #undef vatomic32_inc_rlx
    #undef vatomic32_dec_rlx
    #undef vatomic_fence_rlx
    #define vatomic32_cmpxchg_acq vatomic32_cmpxchg
    #define vatomic32_cmpxchg_rlx vatomic32_cmpxchg
    #define vatomic32_xchg_rel    vatomic32_xchg
    #define vatomic32_inc_rlx     vatomic32_inc
    #define vatomic32_dec_rlx     vatomic32_dec
    #define vatomic_fence_rlx     vatomic_fence
#endif

/* MUSL_MAX_SPIN - Before going to the kernel, first spin for a while. When
 *                 doing verification, no spin is necessary.
 */
#ifndef MUSL_MAX_SPIN
    #define MUSL_MAX_SPIN 100
#endif

typedef struct {
    vatomic32_t lock;
    vatomic32_t waiters;
} vmutex_t;

static inline void
vmutex_init(vmutex_t *m)
{
    vatomic32_init(&m->lock, 0);
    vatomic32_init(&m->waiters, 0);
}

static inline void
vmutex_acquire(vmutex_t *m)
{
    if (vatomic32_cmpxchg_acq(&m->lock, 0, 1) == 0) {
        return;
    }
    vatomic_fence_rlx(); // a_barrier of a_cas failure

#ifndef VSYNC_VERIFICATION
    int spin = MUSL_MAX_SPIN;
    while (spin-- && vatomic32_read_rlx(&m->lock) &&
           !vatomic32_read_rlx(&m->waiters)) {
        vatomic_cpu_pause();
    }
#endif

    while (vatomic32_cmpxchg_acq(&m->lock, 0, 1) != 0) {
        vatomic_fence_rlx(); // a_cas failure
        vatomic32_inc_rlx(&m->waiters);
        if (vatomic32_cmpxchg_rlx(&m->lock, 1, 2) != 1) {
            vatomic_fence_rlx(); // a_cas failure
        }
        vfutex_wait(&m->lock, 2);
        vatomic32_dec_rlx(&m->waiters);
    }
}

static inline void
vmutex_release(vmutex_t *m)
{
    vuint32_t old = vatomic32_xchg_rel(&m->lock, 0);
    if (vatomic32_read_rlx(&m->waiters) > 0 || old != 1) {
        vfutex_wake(&m->lock, 1);
    }
}

#endif
