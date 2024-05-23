/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_UTILS_MATH_H
#define VSYNC_UTILS_MATH_H

#include <vsync/common/assert.h>
#include <vsync/vtypes.h>

#ifdef VSYNC_VERIFICATION
    #include <vsync/utils/internal/math.h>
#else

    /* We define a few constants to make the linter happy. */
    #define V_NUM_8  8U
    #define V_NUM_32 32U

static inline vuint32_t
v_log2(vuint32_t v)
{
    ASSERT(v != 0);
    return (
        (V_NUM_8 * sizeof(unsigned long long) - (vuint32_t)__builtin_clzll(v)) -
        1U);
}

static inline vuint32_t
v_pow2_round_up(vuint32_t v)
{
    ASSERT(v != 0);
    return v == 1U ? 1U : 1U << (V_NUM_32 - (vuint32_t)__builtin_clz(v - 1U));
}

static inline vuint32_t
v_pow2_round_down(vuint32_t v)
{
    ASSERT(v != 0);
    return 1U << ((V_NUM_32 - (vuint32_t)__builtin_clz(v)) - 1U);
}

    #undef V_NUM_8
    #undef V_NUM_32

#endif

#ifndef V_IS_POWER_OF_TWO
    /**
     * Checks if the given number is not zero and is a power of two
     *
     */
    #define V_IS_POWER_OF_TWO(_num_)                                           \
        (((_num_) != 0) && ((_num_) & ((_num_)-1)) == 0)

#endif

#ifndef V_MOD_POWER_OF_TWO
    /**
     *  Calculates _num_ % _pot_
     * bitwise & is used if _pot_ is a power of two
     */
    #define V_MOD_POWER_OF_TWO(_num_, _pot_)                                   \
        (V_IS_POWER_OF_TWO(_pot_) ? ((_num_) & ((_pot_)-1)) : (_num_) % (_pot_))
#endif

/**
 * Returns the parameter with the lowest value.
 *
 */
#ifndef VMIN
    #define VMIN(_a_, _b_)                                                     \
        ({                                                                     \
            __typeof__(_a_) _a = (_a_);                                        \
            __typeof__(_b_) _b = (_b_);                                        \
            _a < _b ? _a : _b;                                                 \
        })
#endif

/**
 * Returns the parameter with the greatest value.
 *
 */
#ifndef VMAX
    #define VMAX(_a_, _b_)                                                     \
        ({                                                                     \
            __typeof__(_a_) _a = (_a_);                                        \
            __typeof__(_b_) _b = (_b_);                                        \
            _a > _b ? _a : _b;                                                 \
        })
#endif

/**
 * Evaluates to true if `_v_` is in [`_lb_`: `_ub_`].
 *
 * @param _lb_ lower bound
 * @param _ub_ upper bound
 * @param _v_  value to check if in range
 */
#ifndef VIN_RANGE
    #define VIN_RANGE(_lb_, _v_, _ub_) (((_lb_) <= (_v_)) && ((_v_) <= (_ub_)))
#endif

/**
 * Returns ceiling of integer division without the use of double/float
 * conversion.
 * @example `VCEIL_DIV(3.5, 2)` returns 2
 *
 */
#ifndef VCEIL_DIV
    #define VCEIL_DIV(_x_, _y_)                                                \
        ((_x_) % (_y_) == 0 ? ((_x_) / (_y_)) : (((_x_) / (_y_)) + 1))
#endif

/**
 * Returns true if the given value is odd.
 *
 */
#ifndef VIS_ODD
    #define VIS_ODD(_v_) (((_v_)&1U) == 1U)
#endif

/**
 * Returns true if the given value is even.
 *
 */
#ifndef VIS_EVEN
    #define VIS_EVEN(_v_) (((_v_)&1U) == 0U)
#endif

/**
 * Returns least `N` where `N >= divisor` and `N % dividend` equals zero.
 *
 * @param divisor the divisor.
 * @param dividend the dividend.
 * @return vsize_t N.
 */
static inline vsize_t
v_least_containing_multiple(vsize_t divisor, vsize_t dividend)
{
    ASSERT(dividend);
    vsize_t result = ((divisor + (dividend - 1U)) / dividend) * dividend;
    ASSERT(result >= divisor);
    return result;
}

#endif
