/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_COMMON_MACROS_H
#define VSYNC_COMMON_MACROS_H

/*******************************************************************************
 * static assert
 ******************************************************************************/

#define V_ASSERT_(COND, MSG) char _v_##MSG[(COND) ? 1 : -1]
#define V_ASSERT(COND, MSG)  V_ASSERT_(COND, MSG)

/*******************************************************************************
 * paster and expanders
 ******************************************************************************/

#define V_PASTE(a, b)  a##b
#define V_CONCAT(a, b) V_PASTE(a, b)
#define V_JOIN(a, b)   V_CONCAT(V_CONCAT(a, _), b)
#define V_STRING(a)    #a

/*******************************************************************************
 * attributes
 ******************************************************************************/

#ifdef VSYNC_DISABLE_DEPRECATED_WARNING
    #define V_DEPRECATED
#else
    #define V_DEPRECATED __attribute__((deprecated))
#endif

/*******************************************************************************
 * argument count macro
 ******************************************************************************/

#define V_NR_VARS_(m, a, b, c, d, e, f, g, h, i, j, k, n, ...) m##n
#define V_NR_VARS(m, ...)                                                      \
    V_NR_VARS_(m, ##__VA_ARGS__, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

/*******************************************************************************
 * unused args
 ******************************************************************************/
/**
 * V_UNUSED_\d macros are used by V_UNUSED to add a void behind the
 * parameters so that compiler errors of unused variables are turned off
 * when the DBG print macros are turned off and the parameter is not used
 * anywhere else in the code
 *
 */
#define V_UNUSED_0()                                                           \
    do {                                                                       \
    } while (0)

#define V_UNUSED_1(_param_)                                                    \
    do {                                                                       \
        (void)(_param_);                                                       \
        V_UNUSED_0();                                                          \
    } while (0)

#define V_UNUSED_2(_param_, ...)                                               \
    do {                                                                       \
        (void)(_param_);                                                       \
        V_UNUSED_1(__VA_ARGS__);                                               \
    } while (0)

#define V_UNUSED_3(_param_, ...)                                               \
    do {                                                                       \
        (void)(_param_);                                                       \
        V_UNUSED_2(__VA_ARGS__);                                               \
    } while (0)

#define V_UNUSED_4(_param_, ...)                                               \
    do {                                                                       \
        (void)(_param_);                                                       \
        V_UNUSED_3(__VA_ARGS__);                                               \
    } while (0)

#define V_UNUSED_5(_param_, ...)                                               \
    do {                                                                       \
        (void)(_param_);                                                       \
        V_UNUSED_4(__VA_ARGS__);                                               \
    } while (0)

#define V_UNUSED_6(_param_, ...)                                               \
    do {                                                                       \
        (void)(_param_);                                                       \
        V_UNUSED_5(__VA_ARGS__);                                               \
    } while (0)

#define V_UNUSED_7(_param_, ...)                                               \
    do {                                                                       \
        (void)(_param_);                                                       \
        V_UNUSED_6(__VA_ARGS__);                                               \
    } while (0)

#define V_UNUSED_8(_param_, ...)                                               \
    do {                                                                       \
        (void)(_param_);                                                       \
        V_UNUSED_7(__VA_ARGS__);                                               \
    } while (0)

#define V_UNUSED_9(_param_, ...)                                               \
    do {                                                                       \
        (void)(_param_);                                                       \
        V_UNUSED_8(__VA_ARGS__);                                               \
    } while (0)

#define V_UNUSED_10(_param_, ...)                                              \
    do {                                                                       \
        (void)(_param_);                                                       \
        V_UNUSED_9(__VA_ARGS__);                                               \
    } while (0)

#define V_UNUSED_11(_param_, ...)                                              \
    do {                                                                       \
        (void)(_param_);                                                       \
        V_UNUSED_10(__VA_ARGS__);                                              \
    } while (0)

/* turns off unused variable warnings on all parameters
 * @note V_NR_VARS is defined in vatomic_utils.h
 */
#define V_UNUSED(...)                                                          \
    do {                                                                       \
        V_NR_VARS(V_UNUSED_, ##__VA_ARGS__)(__VA_ARGS__);                      \
    } while (0)

/**
 * Compiler barrier
 *
 */
#define V_COMPILER_BARRIER() __asm__ __volatile__("" ::: "memory")

#endif
