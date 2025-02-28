/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTHREAD_FUTEX_H
#define VTHREAD_FUTEX_H
/*******************************************************************************
 * futex syscalls
 *
 * If the macros are not defined, we mock the futex syscall with a simple
 * spinning mechanism.
 ******************************************************************************/
#include <vsync/atomic.h>
#include <limits.h>

#define FUTEX_WAKE_ALL INT_MAX
#define FUTEX_WAKE_ONE 1

/* at the moment we do not support macos futex (__ulock) */
#if (defined(__NetBSD__) || defined(__APPLE__)) && !defined(FUTEX_USERSPACE)
    #define FUTEX_USERSPACE
#endif

#if defined(VSYNC_VERIFICATION) || defined(FUTEX_USERSPACE)

static vatomic32_t g_signal;

static inline void
vfutex_wait(vatomic32_t *m, vuint32_t v)
{
    vuint32_t s = vatomic32_read_acq(&g_signal);
    if (vatomic32_read_rlx(m) != v)
        return;
    vatomic32_await_neq_rlx(&g_signal, s);
}

static inline void
vfutex_wake(vatomic32_t *m, vuint32_t v)
{
    vatomic32_inc_rel(&g_signal);
}

#elif defined(__linux__)

    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif /* _GNU_SOURCE */
    #include <errno.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <sys/time.h>
    #include <sys/syscall.h>
    #include <linux/futex.h>

static inline long
_vfutex_call(int *uaddr, int futex_op, int val)
{
    return syscall(SYS_futex, uaddr, futex_op, val, NULL, NULL, 0);
}

static inline void
vfutex_wait(vatomic32_t *m, vuint32_t v)
{
    long s = 0;
    s      = _vfutex_call((int *)m, FUTEX_WAIT, (int)v);

    if (s == -1 && errno == EAGAIN) {
        return;
    }

    if (s == -1 && errno != EAGAIN) {
        perror("futex_wait failed");
        exit(EXIT_FAILURE);
    }
}

static inline void
vfutex_wake(vatomic32_t *m, vuint32_t nthreads)
{
    long s = 0;
    s      = _vfutex_call((int *)m, FUTEX_WAKE, (int)nthreads);

    if (s == -1) {
        perror("futex_wake failed");
        exit(EXIT_FAILURE);
    }
}

#else /* expect user to provide the interface */

void vfutex_wait(vatomic32_t *m, vuint32_t v);
void vfutex_wake(vatomic32_t *m, vuint32_t v);

#endif

#endif /* VTHREAD_FUTEX_H */
