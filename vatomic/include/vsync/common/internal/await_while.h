/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef AWAIT_WHILE_INTERNAL_H
#define AWAIT_WHILE_INTERNAL_H

/*******************************************************************************
 * @def VSYNC_DISABLE_SPIN_ANNOTATION
 * @brief disables await_while and await_do annotations.
 *
 * If we are verifying code, await_while and await_do macros add the necessary
 * annotations to verify the termination of await loops with GenMC > 0.6.
 *
 * GenMC 0.8 and below has issues automatically detecting await loops. With spin
 * annotation, loops are properly annotated. Use disable-spin-assume and
 * -check-liveness GenMC options.
 *
 ******************************************************************************/
#ifdef DOC
    #define VSYNC_DISABLE_SPIN_ANNOTATION
#endif

/*******************************************************************************
 * await_while and other await loop macros
 *
 * If we are verifying code, these macros will add the necessary annotation
 * to be able to verify the termination of await loops.
 *
 * Note: GenMC 0.6 has issues automatically detecting spinloops (await
 * loops). With VSYNC_SPIN_ANNOTATION, loops are properly annotated. Use
 * disable-spin-assume and -check-liveness GenMC options
 ******************************************************************************/
#if defined(VSYNC_VERIFICATION) && !defined(VSYNC_DISABLE_SPIN_ANNOTATION)

    #include <vsync/vtypes.h>
    #include <vsync/common/verify.h>

    #define await_while(cond)                                                  \
        for (verification_loop_begin();                                        \
             (verification_spin_start(),                                       \
              (cond) ? 1 : (verification_spin_end(1), 0));                     \
             verification_spin_end(0))

    #define await_do                                                           \
        do {                                                                   \
            vbool_t __tmp;                                                     \
            verification_loop_begin();                                         \
            do {                                                               \
                {                                                              \
                    verification_spin_start();                                 \
                }

    #define while_await(cond)                                                  \
        }                                                                      \
        while (__tmp = (cond), verification_spin_end(!__tmp), __tmp)           \
            ;                                                                  \
        }                                                                      \
        while (false)

    #define await_break                                                        \
        {                                                                      \
            verification_spin_end(1);                                          \
            break;                                                             \
        }

#endif
#endif /* AWAIT_WHILE_H */
