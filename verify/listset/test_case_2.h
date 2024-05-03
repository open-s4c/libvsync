/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#define INIT_K1 0
#define INIT_K2 4

/* Preinsert keys \in [1, 4]
 * goal: SC of remove
 */
const vsize_t g_lst_idx = 0;
void
pre(void)
{
    vsize_t tid = 0;
    lst_reg(tid);
    for (vlistset_key_t k = INIT_K1; k <= INIT_K2; k++) {
        lst_add(tid, g_lst_idx, k);
    }
    lst_dereg(tid);
}
void
t0(vsize_t tid)
{
    vbool_t success = false;
    success         = lst_rem(tid, g_lst_idx, 3);
    ASSERT(success);

    success = lst_rem(tid, g_lst_idx, 2);
    ASSERT(success);
}
void
t1(vsize_t tid)
{
    if (!lst_con(tid, 0, 2)) {
        ASSERT(!lst_con(tid, g_lst_idx, 3));

    } else {
        verification_ignore();
    }
}
void
t2(vsize_t tid)
{
    lst_clean(tid);
}
void
post(void)
{
    lst_verify_traces(g_lst_idx);
}
