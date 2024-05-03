/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VHASH_CONFIG_H
#define VHASH_CONFIG_H

#ifdef VSYNC_VERIFICATION
    #define VHASHTABLE_BUCKET_COUNT 2U
    #define VHASHTABLE_BUCKET_LEN   2U
#else
    #ifndef VHASHTABLE_BUCKET_COUNT
        #define VHASHTABLE_BUCKET_COUNT 1024U
    #endif
    #define VHASHTABLE_BUCKET_LEN 4U
#endif

#endif
