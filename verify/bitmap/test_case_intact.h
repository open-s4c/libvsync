/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
void
pre(void)
{
    vbitmap_set_range(g_bitmap, NTHREADS, BITMAP_LEN - 1);
}
void
t(vsize_t tid)
{
    vbool_t set = false;

    vbitmap_set_bit(g_bitmap, tid);
    set = vbitmap_get(g_bitmap, tid);
    ASSERT(set);

    vbitmap_clr_bit(g_bitmap, tid);
    set = vbitmap_get(g_bitmap, tid);
    ASSERT(!set);
}
void
t0(vsize_t tid)
{
    t(tid);
}
void
t1(vsize_t tid)
{
    t(tid);
}
void
t2(vsize_t tid)
{
    t(tid);
}
void
post(void)
{
    for (vsize_t i = 0; i < BITMAP_LEN; i++) {
        vbool_t set = vbitmap_get(g_bitmap, i);
        if (i < NTHREADS) {
            ASSERT(!set);
        } else {
            ASSERT(set);
        }
    }
}
#endif
