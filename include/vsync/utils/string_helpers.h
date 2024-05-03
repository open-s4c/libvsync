/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_UTILS_STRING_HELPERS_H
#define VSYNC_UTILS_STRING_HELPERS_H

#include <vsync/vtypes.h>

#define V_UTILS_MEMORY_NO_WRAP(src, sz)                                        \
    ((vuintptr_t)(src) < (vuintptr_t)(src) + (sz))

#define V_UTILS_MEMORY_NO_OVERLAP(src, src_sz, dst, dst_sz)                    \
    (((src) < (dst) && ((vuintptr_t)(src) + (src_sz)) <= (vuintptr_t)(dst)) || \
     ((dst) < (src) && ((vuintptr_t)(dst) + (dst_sz)) <= (vuintptr_t)(src)))

static inline vbool_t
v_memcpy_s_params_ok(void *dst, vsize_t dst_sz, const void *src, vsize_t src_sz)
{
    return src != NULL && dst != NULL && V_UTILS_MEMORY_NO_WRAP(src, src_sz) &&
           V_UTILS_MEMORY_NO_WRAP(dst, dst_sz) &&
           V_UTILS_MEMORY_NO_OVERLAP(src, src_sz, dst, dst_sz);
}

static inline int
v_memset_s_params_ok(void *dst, vsize_t dst_size, int ch, vsize_t count)
{
    (void)ch;
    return dst != NULL && dst_size >= count &&
           V_UTILS_MEMORY_NO_WRAP(dst, dst_size);
}

#undef V_UTILS_MEMORY_NO_WRAP
#undef V_UTILS_MEMORY_NO_OVERLAP

#endif
