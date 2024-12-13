/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VATOMIC_CONFIG_H
#define VATOMIC_CONFIG_H
/*******************************************************************************
 * @file config.h
 * @brief Global configuration of vatomics
 *
 * The configuration defined in this file is typically applied to the whole
 * project using vatomic.
 ******************************************************************************/

#ifndef VATOMIC_CORE_H
    #error "File included directly. Include <vsync/atomic/core.h> instead!"
#endif

#include <vsync/common/verify.h>

/*******************************************************************************
 * @def VATOMIC_ENABLE_FENCE_RLX_NOP
 * @brief maps `vatomic_fence_rlx` to a NOP instruction, otherwise to nothing.
 ******************************************************************************/
#ifdef DOC
    #define VATOMIC_ENABLE_FENCE_RLX_NOP
#elif defined(VATOMIC_ENABLE_FENCE_RLX_NOP) || defined(VSYNC_MAP_RLX_TO_NOP)
    #define V_FENCE_RLX_INSTRUCTION "nop"
#else
    #define V_FENCE_RLX_INSTRUCTION ""
#endif

/*******************************************************************************
 * @def VATOMIC_DISABLE_POLITE_AWAIT
 * @brief disables polite await in x86_64 and aarch64.
 *
 * By default, `vatomic_await_` functions use polite await strategies with
 * `PAUSE` or `WFE` instructions in x86_64 and arm64, respectively. Define
 * VATOMIC_DISABLE_POLITE_AWAIT to busy loop without such instructions.
 ******************************************************************************/
#ifdef DOC
    #define VATOMIC_DISABLE_POLITE_AWAIT
#elif defined(VSYNC_NO_WFE) && !defined(VATOMIC_DISABLE_POLITE_AWAIT)
    #define VATOMIC_DISABLE_POLITE_AWAIT
#endif

/*******************************************************************************
 * @def VATOMIC_BUILTINS
 * @brief uses `__atomic` compiler builtins to implement atomic operations.
 *
 * Unless VATOMIC_BUILTINS is defined, arm32 and aarch64
 * architectures employ custom assembly as atomics implementation. Other
 * architectures, including x86_64, always employ compiler builtins.
 ******************************************************************************/
#ifdef DOC
    #define VATOMIC_BUILTINS
#elif defined(VSYNC_BUILTINS)
    #define VATOMIC_BUILTINS
#endif

/*******************************************************************************
 * @def VATOMIC_ENABLE_ATOMIC_SC
 * @brief turns barrier mode of all atomic operations and fences to seq_cst.
 ******************************************************************************/
#ifdef DOC
    #define VATOMIC_ENABLE_ATOMIC_SC
#elif defined(VSYNC_SC)
    #define VATOMIC_ENABLE_ATOMIC_SC
#endif

/*******************************************************************************
 * @def VATOMIC_ENABLE_ATOMIC_RLX
 * @brief turns barrier mode of all atomic operations and fences to relaxed.
 ******************************************************************************/
#ifdef DOC
    #define VATOMIC_ENABLE_ATOMIC_RLX
#elif defined(VSYNC_RLX)
    #define VATOMIC_ENABLE_ATOMIC_RLX
#endif

/*******************************************************************************
 * @def VATOMIC_DISABLE_ARM64_LSE
 * @brief Do not use ARM64 LSE instructions for atomic operations.
 *
 * If the compiler is configured to emit LSE instructions with some flag
 * such as -march=armv8-a+lse, vatomic uses LSE instructions. Defining this flag
 * disables the use of these instructions.
 ******************************************************************************/
#ifdef DOC
    #define VATOMIC_DISABLE_ARM64_LSE
#endif

/*******************************************************************************
 * @def VATOMIC_ENABLE_ARM64_LXE
 * @brief use ARM64 LSE instructions in slow path of LXSX for atomic operations.
 *
 * To use this option, the compiler must be configured to emit LSE instructions
 * with some flag such as -march=armv8-a+lse.
 ******************************************************************************/
#ifdef DOC
    #define VATOMIC_ENABLE_ARM64_LXE
#endif

#endif /* VATOMIC_CONFIG_H */
