/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_VTYPES_H
#define VSYNC_VTYPES_H

/*******************************************************************************
 * @def VSYNC_ENABLE_FREESTANDING
 * @brief includes `vfreestanding.h` to define `vuint{8,16,32,64,ptr}_t` types.
 *
 * By default, use `vuint32_t` and `vuint64_t` types from `stdint.h`. Define
 * VSYNC_ENABLE_FREESTANDING to customize this definitions by providing a
 * file `vfreestanding.h` defining these types in .
 ******************************************************************************/
#ifdef DOC
    #define VSYNC_ENABLE_FREESTANDING
#elif defined(VSYNC_FREESTANDING)
    #define VSYNC_ENABLE_FREESTANDING
#endif

/*******************************************************************************
 * define stdint types
 ******************************************************************************/
#if defined(VSYNC_ENABLE_FREESTANDING)
    #include "vfreestanding.h"
#else
    #include <stdint.h>
    #include <stdbool.h>
    #include <inttypes.h>
    #include <stddef.h>
typedef uint8_t vuint8_t;
typedef uint16_t vuint16_t;
typedef uint32_t vuint32_t;
typedef uint64_t vuint64_t;
typedef uintptr_t vuintptr_t;
typedef int8_t vint8_t;
typedef int16_t vint16_t;
typedef int32_t vint32_t;
typedef int64_t vint64_t;
typedef intptr_t vintptr_t;
typedef size_t vsize_t;
typedef bool vbool_t;
#endif

/*******************************************************************************
 * MAX and WIDTH
 ******************************************************************************/

/* Max */
#define V_UNSIGNED_INT_MAX(T) ((T) ~(T)0)
#define V_SIGNED_INT_MAX(T)   (T)((1ULL << ((sizeof(T) * 8ULL) - 1ULL)) - 1ULL)

#if defined(UINT8_MAX)
    #define VUINT8_MAX UINT8_MAX
#else
    #define VUINT8_MAX V_UNSIGNED_INT_MAX(vuint8_t)
#endif

#if defined(UINT16_MAX)
    #define VUINT16_MAX UINT16_MAX
#else
    #define VUINT16_MAX V_UNSIGNED_INT_MAX(vuint16_t)
#endif

#if defined(UINT32_MAX)
    #define VUINT32_MAX UINT32_MAX
#else
    #define VUINT32_MAX V_UNSIGNED_INT_MAX(vuint32_t)
#endif

#if defined(UINT64_MAX)
    #define VUINT64_MAX UINT64_MAX
#else
    #define VUINT64_MAX V_UNSIGNED_INT_MAX(vuint64_t)
#endif

#if defined(UINTPTR_MAX)
    #define VUINTPTR_MAX UINTPTR_MAX
#else
    #define VUINTPTR_MAX V_UNSIGNED_INT_MAX(vuintptr_t)
#endif

#if defined(SIZE_MAX)
    #define VSIZE_MAX SIZE_MAX
#else
    #define VSIZE_MAX V_UNSIGNED_INT_MAX(vsize_t)
#endif

/* Width */
#if defined(UINT64_WIDTH)
    #define VUINT64_WIDTH UINT64_WIDTH
#elif defined(__UINT64_WIDTH__)
    #define VUINT64_WIDTH __UINT64_WIDTH__
#else
    #define VUINT64_WIDTH (sizeof(vuint64_t) * 8)
#endif

#if defined(UINTPTR_WIDTH)
    #define VUINTPTR_WIDTH UINTPTR_WIDTH
#elif defined(__UINTPTR_WIDTH__)
    #define VUINTPTR_WIDTH __UINTPTR_WIDTH__
#else
    #define VUINTPTR_WIDTH (sizeof(vuintptr_t) * 8)
#endif

#if defined(INT32_MAX)
    #define VINT32_MAX INT32_MAX
#else
    #define VINT32_MAX V_SIGNED_INT_MAX(vint32_t)
#endif

/* Format */
#if !defined(VSYNC_ENABLE_FREESTANDING)
    #define VUINTPTR_FORMAT PRIuPTR
    #define VUINT64_FORMAT  PRIu64
    #define VUINT32_FORMAT  PRIu32
#else
    /* If using FREESTANDING, we have no way how to define these format
     * properly. Here we do our best shot at the moment. */
    #define VUINTPTR_FORMAT "lu"
    #define VUINT64_FORMAT  "lu"
    #define VUINT32_FORMAT  "u"
#endif
#endif
