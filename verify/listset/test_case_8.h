/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


/* goal:
 * removing from head, middle, tail
 */
const vsize_t g_lst_idx = 0;
void
pre(void)
{
    vsize_t tid = 0;
    lst_reg(tid);
    ASSERT(lst_add(tid, g_lst_idx, 1));
    lst_dereg(tid);
}
void
t0(vsize_t tid)
{
    /* add existing value */
    ASSERT(!lst_add(tid, g_lst_idx, 1));
}
void
t1(vsize_t tid)
{
    /* add non exisitng value at the head */
    ASSERT(lst_add(tid, g_lst_idx, 0));
    ASSERT(lst_con(tid, g_lst_idx, 0));
}
void
t2(vsize_t tid)
{
    /* remove non existing value */
    ASSERT(!lst_rem(tid, g_lst_idx, 2));
    lst_clean(tid);
}
void
post(void)
{
    lst_verify_traces(g_lst_idx);
}
