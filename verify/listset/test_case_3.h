/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
/* Preinsert keys \in [0, 2] */
#define INIT_K1 0
#define INIT_K2 2

/* goal:
 * message passing contains vs. rem
 * remove from the middle
 */
int msg                 = 0;
const vsize_t g_lst_idx = 0;
void
pre(void)
{
    for (vlistset_key_t k = INIT_K1; k <= INIT_K2; k++) {
        lst_add(MAIN_TID, g_lst_idx, k);
    }
}
void
t0(vsize_t tid)
{
    if (!lst_con(tid, g_lst_idx, 1)) {
        ASSERT(msg == 1);
    } else {
        verification_ignore();
    }
}
void
t1(vsize_t tid)
{
    msg = 1;
    ASSERT(lst_rem(tid, g_lst_idx, 1));
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
#endif
