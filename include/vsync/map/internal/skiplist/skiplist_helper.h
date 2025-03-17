/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSKIPLIST_HELPER_H
#define VSKIPLIST_HELPER_H

#include <vsync/common/compiler.h>
#include <vsync/utils/math.h>
#include <vsync/map/internal/skiplist/skiplist_types.h>

#define VSKIPLIST_SHIFT_LEFT_BIT_CNT       13U
#define VSKIPLIST_SHIFT_RIGHT_BIT_CNT      17U
#define VSKIPLIST_SHIFT_LEFT_AGAIN_BIT_CNT 5U

#ifdef VSYNC_VERIFICATION
__thread vsize_t local_seed;
#endif

static inline vsize_t
_vskiplist_seed_get(vskiplist_seed_t *skip_seed)
{
#if defined(VSYNC_VERIFICATION)
    (void)skip_seed;
    return local_seed;
#else
    return vatomicsz_read_rlx(&skip_seed->seed);
#endif
}

static inline void
_vskiplist_seed_set(vskiplist_seed_t *skip_seed, vsize_t seed)
{
#if defined(VSYNC_VERIFICATION)
    (void)skip_seed;
    local_seed = seed;
#else
    vatomicsz_write_rlx(&skip_seed->seed, seed);
#endif
}

static inline void
_vskiplist_seed_init(vskiplist_seed_t *skip_seed, vsize_t rand_seed,
                     vsize_t max_height)
{
    vsize_t seed = rand_seed | 256U;

    _vskiplist_seed_set(skip_seed, seed);

    skip_seed->max_height = max_height;
}
/**
 * Generates a random height for the skiplist node, base on the given seed
 * @note the implementation of this function follows the sample code of the art
 * of multiprocessor programing book
 * For a skiplist of 32 maximum height, the probability of the returned random
 * height is as follows:
 * height = 0 -> P = 3/4
 * height in [1, 30] -> P = 2^-(i+2)
 * height = 31 -> p =  2^-32
 * @param skip_seed
 * @return vuint32_t random height
 */
static inline vsize_t
_vskiplist_get_rand_height(vskiplist_seed_t *skip_seed)
{
    vsize_t level = 1;

    vsize_t seed = _vskiplist_seed_get(skip_seed);

    seed ^= seed << VSKIPLIST_SHIFT_LEFT_BIT_CNT;       /* 13 */
    seed ^= seed >> VSKIPLIST_SHIFT_RIGHT_BIT_CNT;      /* 17 */
    seed ^= seed << VSKIPLIST_SHIFT_LEFT_AGAIN_BIT_CNT; /* 5 */

    _vskiplist_seed_set(skip_seed, seed);

    // 0x8001 -> 0b1000_0000_0000_0001
    if ((seed & 0x8001U) != 0) { // test highest and lowest bits
        return 1;
    }

    while (((seed >>= 1U) & 1U) != 0U) {
        ++level;
    }

    return VMIN(level, skip_seed->max_height - 1U);
}

#undef VSKIPLIST_SHIFT_LEFT_BIT_CNT
#undef VSKIPLIST_SHIFT_RIGHT_BIT_CNT
#undef VSKIPLIST_SHIFT_LEFT_AGAIN_BIT_CNT
#endif
