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
    V_UNUSED(tid);
    vbitmap_set_range(g_bitmap, 1, 4);
}
void
t1(vsize_t tid)
{
    V_UNUSED(tid);
    vbitmap_clr_range(g_bitmap, 2, 3);
}
void
t2(vsize_t tid)
{
    V_UNUSED(tid);
    vbitmap_set_range(g_bitmap, 63, 66);
}
void
post(void)
{
    vsize_t set_indexes[6] = {1, 4, 63, 64, 65, 66};
    for (vsize_t i = 0; i < 6; i++) {
        vbool_t set = vbitmap_get(g_bitmap, set_indexes[i]);
        ASSERT(set);
    }
}
