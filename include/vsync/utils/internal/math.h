/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_UTILS_INTERNAL_MATH_H
#define VSYNC_UTILS_INTERNAL_MATH_H
#include <vsync/vtypes.h>

#ifdef VSYNC_VERIFICATION
static inline vuint32_t
v_log2(vuint32_t v)
{
    const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
    const unsigned int S[] = {1, 2, 4, 8, 16};
    int i;

    register unsigned int r = 0; // result of log2(v) will go here
    for (i = 4; i >= 0; i--)     // unroll for speed...
    {
        if (v & b[i]) {
            v >>= S[i];
            r |= S[i];
        }
    }
    return r;
}
/**
 * Returns the next power of two
 *
 * @param v value to round
 *
 * @see
 * https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2
 */
static inline vuint32_t
v_pow2_round_up(vuint32_t v)
{
    v--;
    /* this loop should shift  and or for 1, 2, 4, 8, 16 */
    for (vuint32_t shift_by = 1; shift_by <= 16; shift_by *= 2) {
        v |= v >> shift_by;
    }
    v++;
    return v;
}

static inline vuint32_t
v_pow2_round_down(vuint32_t v)
{
    /* this loop should shift  and or for 1, 2, 4, 8, 16 */
    for (vuint32_t shift_by = 1; shift_by <= 16; shift_by *= 2) {
        v |= v >> shift_by;
    }
    return v - (v >> 1);
}
#endif
#endif
