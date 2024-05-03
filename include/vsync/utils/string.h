/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_UTILS_STRING_H
#define VSYNC_UTILS_STRING_H

#include <string.h>
#include <vsync/common/macros.h>
#include <vsync/vtypes.h>
#include <vsync/utils/string_helpers.h>

/*
 * @note the following functions can trigger model checker bugs related to
 * memset. Developers should work around the issue by copying using a for-loop,
 * when VSYNC_VERIFICATION is defined.
 */

#if !defined(VSYNC_MEMCPY_S_EXISTS) && !defined(VSYNC_VERIFICATION)
static inline int
memcpy_s(void *dst, vsize_t dst_sz, const void *src, vsize_t src_sz)
{
    if (v_memcpy_s_params_ok(dst, dst_sz, src, src_sz)) {
        memcpy(dst, src, src_sz);
        return 0;
    } else {
        return 1;
    }
}
#endif

#if !defined(VSYNC_MEMSET_S_EXISTS) && !defined(VSYNC_VERIFICATION)
static inline int
memset_s(void *dst, vsize_t dst_sz, int ch, vsize_t count)
{
    if (v_memset_s_params_ok(dst, dst_sz, ch, count)) {
        memset(dst, ch, count);
        return 0;
    } else {
        return 1;
    }
}
#endif

#endif
