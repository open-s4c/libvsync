/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef VSYNC_UTILS_STRING_H
#define VSYNC_UTILS_STRING_H

#include <string.h>
#include <vsync/common/macros.h>
#include <vsync/vtypes.h>

/*
 * @note the following functions can trigger model checker bugs related to
 * memset. Developers should work around the issue by copying using a for-loop,
 * when VSYNC_VERIFICATION is defined.
 */
static inline int
memcpy_s(void *dst, vsize_t dst_sz, const void *src, vsize_t src_sz)
{
	if (src_sz > dst_sz) {
		return 1;
	} else {
		memcpy(dst, src, src_sz);
		return 0;
	}
}

static inline int
memset_s(void *dst, vsize_t dst_sz, int ch, vsize_t count)
{
	if (count > dst_sz) {
		return 1;
	} else {
		memset(dst, ch, count);
		return 0;
	}
}
#endif
