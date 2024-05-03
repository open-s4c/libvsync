/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef XBO_H
#define XBO_H
/*******************************************************************************
 * @file xbo.h
 * @brief Exponential backoff mechanism
 *
 * xbo.h provides a backoff mechanism that calls a nop callback to delay the
 * thread calling xbo_backoff. The number of times the nop function is called
 * increases exponentially (by a configurable factor) every time xbo_backoff is
 * called. Once a maximum number of nop calls is reached, an additional callback
 * function is called to, for example, preempt the thread.
 *
 * xbo is intended to be used per thread (it is not thread-safe).
 *
 * This is similar to backoff.h. @todo We should converge to a single
 * implementation.
 *
 * @example
 *
 * The following example backs off calling nop from 0 times to 128 times. Once
 * the backoff value is 128, pthread_yield is additionally called.
 *
 * ```
 * xbo_t x = XBO_INIT(0, 128, 2);
 *
 * while (!try_action()) xbo_backoff(&x, xbo_nop, pthread_yield);
 * ```
 *
 * The following example only employs spinning backoff:
 *
 * ```
 * while (!try_action()) xbo_backoff(&x, xbo_nop, xbo_nop);
 * ```
 ******************************************************************************/
#include <vsync/atomic.h>

/** Callback type to be called when backing off. The return value is ignored. */
typedef int xbo_cb(void);

typedef struct xbo_s {
    vuint32_t min, max;
    vuint32_t factor;
    vuint32_t value;
} xbo_t;

#define XBO_INIT(min, max, factor)                                             \
    {                                                                          \
        .min = (min), .max = (max), .value = (min), .factor = (factor)         \
    }

/**
 * Initializes the xbo datastructure.
 *
 * @param xbo exponential backoff datastructure
 * @param min minimum number of nop calls
 * @param max maximum number of nop calls
 * @param factor factor to increase the current number of nop calls
 */
static inline void
xbo_init(xbo_t *xbo, vuint32_t min, vuint32_t max, vuint32_t factor)
{
#ifndef VSYNC_VERIFICATION
    xbo->min = xbo->value = min;
    xbo->max              = max;
    xbo->factor           = factor;
#endif
}

/**
 * Backs off for a number of nop calls, exponentially increasing this number
 * every time the function is called. Once the maximum number of nop calls is
 * reached, additionally calls cb.
 *
 * @param xbo exponential backoff datastructure
 * @param nop a callback that is called a number of times when backing off
 * @param cb a callback that is called once max is reached
 */
static inline void
xbo_backoff(xbo_t *xbo, xbo_cb *nop, xbo_cb *cb)
{
#ifndef VSYNC_VERIFICATION
    for (vuint32_t i = 0; i < xbo->max; ++i) {
        nop();
    }

    xbo->value = xbo->value == 0 ? 1 : xbo->value * xbo->factor;

    if (xbo->value > xbo->max) {
        xbo->value = xbo->max;
        cb();
    }
#endif
}

/**
 * Resets the current number of nop calls.
 *
 * @param xbo exponential backoff datastructure
 */
static inline void
xbo_reset(xbo_t *xbo)
{
#ifndef VSYNC_VERIFICATION
    xbo->value = xbo->min;
#endif
}

/** A simple nop callback. */
static inline int
xbo_nop(void)
{
    volatile int k = 0;
    return k;
}

#endif
