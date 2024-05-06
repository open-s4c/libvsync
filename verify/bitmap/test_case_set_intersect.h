/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

void
pre(void)
{
}
void
t0(vsize_t tid)
{
    vbitmap_set_range(g_bitmap, 60, 62);
    vbitmap_clr_bit(g_bitmap, 61);
    V_UNUSED(tid);
}
void
t1(vsize_t tid)
{
    vbitmap_set_range(g_bitmap, 62, 64);
    vbitmap_clr_bit(g_bitmap, 63);
    V_UNUSED(tid);
}
void
t2(vsize_t tid)
{
    vbitmap_set_range(g_bitmap, 64, 66);
    vbitmap_clr_bit(g_bitmap, 65);
    V_UNUSED(tid);
}
void
post(void)
{
    for (vsize_t i = 0; i < 59; i++) {
        vbool_t set = vbitmap_get(g_bitmap, i);
        ASSERT(!set);
    }
    for (vsize_t i = 60; i < 66; i++) {
        vbool_t set = vbitmap_get(g_bitmap, i);

        if (i == 61 || i == 63 || i == 65) {
            ASSERT(!set);
        } else {
            ASSERT(set);
        }
    }
}
