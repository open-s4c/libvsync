/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VATOMIC_TYPED_H
#define VATOMIC_TYPED_H

#include <vsync/vtypes.h>

/* *****************************************************************************
 * Basic atomic types
 * ****************************************************************************/
#ifndef DOC
    #define V_ALIGNED2  __attribute__((aligned(2)))
    #define V_ALIGNED4  __attribute__((aligned(4)))
    #define V_ALIGNED8  __attribute__((aligned(8)))
    #define V_ALIGNEDP  __attribute__((aligned(sizeof(void *))))
    #define V_ALIGNEDSZ __attribute__((aligned(sizeof(vsize_t))))
#endif

/* 8-bit unsigned atomic type. */
typedef struct vatomic8_s {
    vuint8_t _v;
} vatomic8_t;

/* 16-bit unsigned atomic type, aligned at 2 bytes. */
typedef struct vatomic16_s {
    vuint16_t _v;
} V_ALIGNED2 vatomic16_t;

/* 32-bit unsigned atomic type, aligned at 4 bytes. */
typedef struct vatomic32_s {
    vuint32_t _v;
} V_ALIGNED4 vatomic32_t;

/* 64-bit unsigned atomic type, aligned at 8 bytes. */
typedef struct vatomic64_s {
    vuint64_t _v;
} V_ALIGNED8 vatomic64_t;

/* architecture-dependent pointer atomic type, aligned at 4 or 8 bytes. */
typedef struct vatomicptr_s {
    void *_v;
} V_ALIGNEDP vatomicptr_t;

/* architecture-dependent vsize_t atomic type, aligned at 4 or 8 bytes. */
typedef struct vatomicsz_s {
    vsize_t _v;
} V_ALIGNEDSZ vatomicsz_t;

#endif
