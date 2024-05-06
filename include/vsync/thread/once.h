/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTHREAD_ONCE_H
#define VTHREAD_ONCE_H
/*******************************************************************************
 * @file once.h
 * @brief One-time initializer.
 *
 * Calls a callback exactly once even if concurrently called. Callback happens
 * before any thread returns vonce_call().
 *
 * @example
 * @include eg_once.c
 *
 * @cite [One-time initializer](https://www.remlab.net/op/futex-misc.shtml)
 *
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/thread/internal/futex.h>

typedef vatomic32_t vonce_t;

#define VONCE_INIT() VATOMIC_INIT(0)
#define VONCE_ZERO   0U
#define VONCE_CALL   1U
#define VONCE_DONE   2U

// vonce_cb takes some argument and returns a pointer
typedef void *(*vonce_cb)(void *arg);

#ifdef VSYNC_VERIFICATION
void *__once_verification_cb(void *);
#endif

/**
 * Calls `cb(arg)` once.
 *
 * The thread that actually executed the callback gets the return value of the
 * callback. Other threads get NULL back.
 *
 * @param o address of once_t object.
 * @param cb address of callback function.
 * @param arg argument of `cb`.
 * @return void* return value of `cb(arg)` if the thread actually called `cb`.
 * @return NULL if another thread already called `cb`.
 */
static inline void *
vonce_call(vonce_t *o, vonce_cb cb, void *arg)
{
    void *ret       = NULL;
    vuint32_t state = VONCE_ZERO;

    if (vatomic32_read_acq(o) == VONCE_DONE) {
        return NULL;
    }

    state = vatomic32_cmpxchg_acq(o, VONCE_ZERO, VONCE_CALL);
    if (state == VONCE_ZERO) {
#ifdef VSYNC_VERIFICATION
        ret = __once_verification_cb(arg);
#else
        ret = cb(arg);
#endif
        vatomic32_write_rel(o, VONCE_DONE);
        vfutex_wake(o, FUTEX_WAKE_ALL);
        return ret;
    }

    while (state == VONCE_CALL) {
        vfutex_wait(o, 1);
        state = vatomic32_read_acq(o);
    }
    return ret;
}

#undef VONCE_ZERO
#undef VONCE_CALL
#undef VONCE_DONE
#endif
