/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_BBQ_TEST_HELPER
#define VSYNC_BBQ_TEST_HELPER

#if defined(VSYNC_VERIFICATION)

    #include <vsync/vtypes.h>

/* TODO: linters will complain about this even if VSYNC_VERIFICATION is not
 * defined! */

static inline int
memcpy_s(void *dst, vsize_t dstsz, void *src, vsize_t srcsz)
{
    ASSERT(srcsz <= dstsz);
    vuintptr_t *d = (vuintptr_t *)dst;
    vuintptr_t *s = (vuintptr_t *)src;
    for (vuint64_t i = 0; i < dstsz / sizeof(vuintptr_t); i++) {
        d[i] = s[i];
    }
    return 0;
}
#endif

#endif
