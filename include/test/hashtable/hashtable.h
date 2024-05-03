/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VHASHTABLE_H
#define VHASHTABLE_H

#if defined(HASHTABLE_STANDARD)
    #include <vsync/map/hashtable_standard.h>
#elif defined(HASHTABLE_EVICTABLE)
    #include <vsync/map/hashtable_evictable.h>
    #include <test/hashtable/hashtable_stats.h>
#else
    #error "undefined impl."
#endif


#endif
