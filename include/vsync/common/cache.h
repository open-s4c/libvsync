/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_CACHE_H
#define VSYNC_CACHE_H
/*******************************************************************************
 * This file provides cache macros to align and pad data structures to the cache
 * line size.  The cache alignment is disabled if the code is compiled for
 * verification (with VSYNC_VERIFICATION defined).
 ******************************************************************************/

#ifndef VSYNC_CACHELINE_SIZE
    /* L3 cache line size for ARM processors */
    #define VSYNC_CACHELINE_SIZE 128
#endif

#ifdef VSYNC_VERIFICATION
    #define VSYNC_CACHEALIGN
    #define VSYNC_CACHEPAD(typ, var) typ var
#else /* ! VSYNC_VERIFICATION */
    #define VSYNC_CACHEALIGN __attribute__((aligned(VSYNC_CACHELINE_SIZE)))
    #define VSYNC_CACHEDIFF(sz)                                                \
        ((VSYNC_CACHELINE_SIZE - ((sz) % VSYNC_CACHELINE_SIZE)) %              \
         VSYNC_CACHELINE_SIZE)
    #define VSYNC_CACHEPAD(typ, var) char var[VSYNC_CACHEDIFF(sizeof(typ))]
#endif /* ! VSYNC_VERIFICATION */

#endif
