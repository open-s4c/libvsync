/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_ITREESET_H
#define VSYNC_ITREESET_H

#if defined TREESET_BST_FINE
    #include <vsync/map/treeset_bst_fine.h>
#elif defined TREESET_RB_FINE
    #include <vsync/map/treeset_rb_fine.h>
#elif defined TREESET_BST_COARSE
    #include <vsync/map/treeset_bst_coarse.h>
#elif defined TREESET_RB_COARSE
    #include <vsync/map/treeset_rb_coarse.h>
#else
    #error "Choose treeset implementation by setting TREESET_*"
#endif

#endif
