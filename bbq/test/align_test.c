/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <vsync/common/assert.h>
#include <vsync/vtypes.h>

uint32_t
log2_of(vuint32_t v)
{
#if 0
    return ((unsigned)(8 * sizeof(unsigned long long) - __builtin_clzll((v)) - 1));
#else

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
#endif
}

uint32_t
pow2_round_up(vuint32_t v)
{
#if 1
    return v == 1 ? 1 : 1 << (32 - __builtin_clz(v - 1));
#else
    v--;
    for (vuint32_t shift_by = 1; shift_by <= 16; shift_by *= 2) {
        v |= v >> shift_by;
    }
    return v;
#endif
}

uint32_t
pow2_round_down(vuint32_t v)
{
#if 0
    return (1U << (32U - __builtin_clz((vuint32_t)(v)) - 1U));
#else
    for (vuint32_t shift_by = 1; shift_by <= 16; shift_by *= 2) {
        v |= v >> shift_by;
    }
    return v - (v >> 1);
#endif
}

#define bbq_align_down_with_power2(a) (pow2_round_down(a))

#define bbq_log2(X) (log2_of(X))

int
main(void)
{
    // 2^f2(x) = f1(x)，f1 is the align down and f2 is log2
    // for example, f2(1023) = 9, f2(1024) = 10
    // Jiawei Wang(84201116)2022-12-15 08:11
    // f1(1023) = 512, f1(1024) = 1024

    ASSERT(bbq_align_down_with_power2(1023) == 512);
    ASSERT(pow2_round_up(1023) == 1024);

    ASSERT(bbq_align_down_with_power2(1024) == 1024);
    ASSERT(pow2_round_up(1024) == 1024);

    ASSERT(bbq_align_down_with_power2(1023) == 512);
    ASSERT(pow2_round_up(1023) == 1024);

    ASSERT(bbq_align_down_with_power2(1023) == 512);
    ASSERT(pow2_round_up(1023) == 1024);

    ASSERT(bbq_log2(1023) == 9);
    ASSERT(bbq_log2(1024) == 10);

    return 0;
}
