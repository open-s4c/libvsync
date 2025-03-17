/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H

#define RANGE_MIN 31U
#define RANGE_MAX (BITMAP_LEN - 1U)

void
pre(void)
{
}

void
t0(vsize_t tid)
{
    vbitmap_set_range(g_bitmap, RANGE_MIN, VUINT64_WIDTH);
    V_UNUSED(tid);
}
void
t1(vsize_t tid)
{
    vbitmap_set_range(g_bitmap, VUINT64_WIDTH, RANGE_MAX);
    V_UNUSED(tid);
}
void
t2(vsize_t tid)
{
    vbitmap_iter_t iter;
    vsize_t idx = 0;
    vbitmap_iter_init(g_bitmap, &iter);

    while (vbitmap_iter_next(&iter, &idx)) {
        ASSERT(idx >= RANGE_MIN);
        ASSERT(idx <= RANGE_MAX);
    }

    V_UNUSED(tid);
}
void
post(void)
{
    vbitmap_iter_t iter;
    vsize_t idx      = 0;
    vsize_t expected = RANGE_MIN;
    vbitmap_iter_init(g_bitmap, &iter);

    while (vbitmap_iter_next(&iter, &idx)) {
        ASSERT(idx >= RANGE_MIN);
        ASSERT(idx <= RANGE_MAX);
        ASSERT(idx == expected);
        expected++;
    }
    ASSERT(idx == RANGE_MAX);
}
#endif
