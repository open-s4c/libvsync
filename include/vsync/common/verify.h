/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_VERIFICATION_H
#define VSYNC_VERIFICATION_H
/*******************************************************************************
 * @file verify.h
 * @brief Verification support
 *
 * This file contains several functions to support verification and
 * corresponding options to enable or disable these functions depending on
 * the model checker used.
 ******************************************************************************/

#include <vsync/vtypes.h>
#include <vsync/common/macros.h>

/*******************************************************************************
 * @def VSYNC_VERIFICATION
 * @brief enables vsyncer verification.
 *
 * Define VSYNC_VERIFICATION to enable verification with vsyncer, GenMC and
 * Dartagnan. This uses compiler builtins for the implementation of atomic
 * operations.
 ******************************************************************************/
#ifdef DOC
    // Note: We need to define this, for the macro to be documented in our
    // documentation. However, we actually don't want to document anything
    // in our library which is hidden behind `VSYNC_VERIFICATION`, since
    // those would be internal implementation details. Thus we undefine
    // the macro again immediately.
    #define VSYNC_VERIFICATION
    #undef VSYNC_VERIFICATION
#endif

#if defined(VSYNC_VERIFICATION) && !defined(VATOMIC_BUILTINS)
    #define VATOMIC_BUILTINS
#endif

#if defined(VSYNC_VERIFICATION) && !defined(VATOMIC_DISABLE_POLITE_AWAIT)
    #define VATOMIC_DISABLE_POLITE_AWAIT
#endif

#include <vsync/common/await_while.h>

/*******************************************************************************
 * @def VSYNC_VERIFICATION_GENERIC
 * @brief enables use if basic __VERIFIER_ functions.
 *
 * Most model checkers provide __VERIFIER_nondet_int and __VERIFIER_assume
 * functions. These are considered GENERIC by libvsync.
 ******************************************************************************/
#ifdef DOC
    #define VSYNC_VERIFICATION_GENERIC
#elif defined(VSYNC_VERIFICATION) && defined(VSYNC_VERIFICATION_GENERIC)
    #define VSYNC_USE_VERIFER_ASSUME
    #define VSYNC_USE_VERIFER_NONDET
#endif

/*******************************************************************************
 * @def VSYNC_VERIFICATION_DAT3M
 * @brief enables Dartagnan's specific __VERIFIER_ functions.
 *
 * Besides GENERIC functions, also use __VERIFIER_spin_* and
 * __VERIFIER_loop_bound functions
 ******************************************************************************/
#ifdef DOC
    #define VSYNC_VERIFICATION_DAT3M
#elif defined(VSYNC_VERIFICATION) && defined(VSYNC_VERIFICATION_DAT3M)
    /* see include/dat3m.h in Dat3M source */
    #define VSYNC_USE_VERIFIER_ASSUME
    #define VSYNC_USE_VERIFIER_NONDET
    #define VSYNC_USE_VERIFIER_SPIN
    #define VSYNC_USE_VERIFIER_LOOP_BOUND
#endif

/*******************************************************************************
 * @def VSYNC_VERIFICATION_DAT3M
 * @brief enables Dartagnan's specific __VERIFIER_ functions.
 *
 * Besides GENERIC functions, also use __VERIFIER_spin_* and
 * __VERIFIER_loop_bound functions
 ******************************************************************************/
#ifdef DOC
    #define VSYNC_VERIFICATION_GENMC
#elif defined(VSYNC_VERIFICATION_GENERIC)
#elif defined(VSYNC_VERIFICATION_DAT3M)
#elif defined(VSYNC_VERIFICATION) /* && VSYNC_VERIFICATION_GENMC */
    #include <genmc.h>
    #define VSYNC_USE_VERIFIER_ASSUME
    #define VSYNC_USE_VERIFIER_NONDET
    #define VSYNC_USE_VERIFIER_SPIN
#endif

/*******************************************************************************
 * @def VSYNC_USE_VERIFIER_ASSUME
 * @brief enables functions based on __VERIFIER_assume.
 ******************************************************************************/
#ifdef DOC
    #define VSYNC_USE_VERIFIER_ASSUME
#elif defined(VSYNC_USE_VERIFIER_ASSUME)

static inline void
verification_ignore(void)
{
    __VERIFIER_assume(0);
}

static inline void
verification_assume(vbool_t cond)
{
    __VERIFIER_assume(cond);
}
#else
static inline void
verification_ignore(void)
{
}

static inline void
verification_assume(vbool_t condition)
{
    V_UNUSED(condition);
}
#endif

/*******************************************************************************
 * @def VSYNC_USE_VERIFIER_NONDET
 * @brief enables functions based on __VERIFIER_nondet_int.
 ******************************************************************************/
#ifdef DOC
    #define VSYNC_USE_VERIFIER_NONDET
#elif defined(VSYNC_USE_VERIFIER_NONDET)

static inline int
verification_rand(void)
{
    return __VERIFIER_nondet_int();
}
#else
static inline int
verification_rand(void)
{
    return 0;
}
#endif

/*******************************************************************************
 * @def VSYNC_USE_VERIFIER_SPIN
 * @brief enables functions based on __VERIFIER_spin annotations.
 ******************************************************************************/
#ifdef DOC
    #define VSYNC_USE_VERIFIER_SPIN
#elif defined(VSYNC_USE_VERIFIER_SPIN)

static inline void
verification_loop_begin(void)
{
    __VERIFIER_loop_begin();
}
static inline void
verification_spin_start(void)
{
    __VERIFIER_spin_start();
}
static inline void
verification_spin_end(int v)
{
    __VERIFIER_spin_end(v);
}
#else
static inline void
verification_loop_begin(void)
{
}
static inline void
verification_spin_start(void)
{
}
static inline void
verification_spin_end(int v)
{
    V_UNUSED(v);
}
#endif

#endif
