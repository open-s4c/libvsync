/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_BACKOFF_H
#define VSYNC_BACKOFF_H

/*******************************************************************************
 * @file backoff.h
 * @brief Implements exponential backoff
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 7.4]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 *****************************************************************************/

#include <vsync/common/assert.h>
#include <vsync/vtypes.h>
#include <vsync/common/compiler.h>
#include <vsync/common/verify.h>
#include <vsync/utils/math.h>
#include <vsync/atomic.h>

/**
 * A function pointer to a function that sleeps for the given microseconds.
 *
 * @param microseconds amount of sleep in microseconds.
 * @return 0, on no error.
 * @return -1, on error.
 *
 * @note Use usleep on platforms where `usleep` is available.
 */
typedef int (*backoff_usleep_fun_t)(vuint32_t microseconds);
/**
 * A function pointer to a function that generates a random number.
 *
 * The generated random number should be in `[min, max]`.
 *
 * @param min lower bound of the random number.
 * @param max upper bound of the random number.
 * @return the generated random number in `[min, max]`.
 */
typedef vuint32_t (*backoff_rand_fun_t)(vuint32_t min, vuint32_t max);

typedef struct backoff_s {
    vuint32_t max_delay;
    /* has to be atomic to avoid data-race and undefined behavior
     * @note in java defining this as atomic is not necessary
     */
    vatomic32_t limit;
    backoff_usleep_fun_t usleep_fun;
    backoff_rand_fun_t rand_fun;
} backoff_t;
/**
 * Initializes the given exponential `backoff` object.
 *
 * @param backoff address of backoff_t object.
 * @param min minimum amount of microseconds a thread is allowed to
 * sleep.
 * @param max maximum allowed amount of microseconds to sleep.
 * @param usleep_fun function pointer to a function that sleeps for the given
 * microseconds.
 * @param rand_fun function pointer to a function that generates a random
 * number.
 */
static inline void
xbobackoff_init(backoff_t *backoff, vuint32_t min, vuint32_t max,
                backoff_usleep_fun_t usleep_fun, backoff_rand_fun_t rand_fun)
{
    ASSERT(backoff);
    ASSERT(usleep_fun);
    ASSERT(min < max);

    backoff->max_delay  = max;
    backoff->usleep_fun = usleep_fun;
    backoff->rand_fun   = rand_fun;

    vatomic32_write_rlx(&backoff->limit, min);
}
/**
 * Puts the calling thread to sleep for a random number of microseconds.
 *
 * @note the sleep cannot exceed the given `max` in `xbobackoff_init`.
 * @param backoff address of backoff_t object
 */
static inline void
xbobackoff_backoff(backoff_t *backoff)
{
    vuint32_t limit     = vatomic32_read_rlx(&backoff->limit);
    vuint32_t delay     = backoff->rand_fun(0, limit);
    vuint32_t new_limit = VMIN(backoff->max_delay, (2 * limit));

    /* Update limit */
    vatomic32_write_rlx(&backoff->limit, new_limit);

    /* sleep in microseconds */
    backoff->usleep_fun(delay);
}
#endif
