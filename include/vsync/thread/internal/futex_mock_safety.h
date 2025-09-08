/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTHREAD_FUTEX_MOCK_SAFETY_H
#define VTHREAD_FUTEX_MOCK_SAFETY_H

/*******************************************************************************
 * futex mock for verifying safety property only.
 *
 * This mock always wakes up every waiting thread, so it cannot be used for
 * verifying liveness. If there is more than one futex or waiter for the mutex,
 * consider using a different mock.
 *
 ******************************************************************************/

#include <vsync/atomic.h>

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
    (void)m;
    (void)v;
    vatomic32_inc_rel(&g_signal);
}

#endif /* VTHREAD_FUTEX_MOCK_SAFETY_H */
