/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VATOMIC_TMPLR_H
#define VATOMIC_TMPLR_H

#include <tmplr/macros.h>
#include <vsync/vtypes.h>

/**
 * TT is an unsigned integer type or void *.
 */
typedef unsigned int TT;

/**
 * AA is one of the vatomic types.
 *
 * These are vatomic{8,16,32,64,ptr}.
 */
typedef struct {
    TT _v;
} AA;

/* some further macros to make clangd happy */
#define _MAP_OP

#endif
