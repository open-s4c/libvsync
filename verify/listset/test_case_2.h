/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
#define INIT_K1 0
#define INIT_K2 4

/* Preinsert keys \in [1, 4]
 * goal: SC of remove
 */
const vsize_t g_lst_idx = 0;
#define KEY_A 3U
#define KEY_B 2U
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
    vbool_t success = false;
    success         = lst_rem(tid, g_lst_idx, KEY_A);
    ASSERT(success);

    success = lst_rem(tid, g_lst_idx, KEY_B);
    ASSERT(success);
}
void
t1(vsize_t tid)
{
    if (!lst_con(tid, g_lst_idx, KEY_B)) {
        ASSERT(!lst_con(tid, g_lst_idx, KEY_A));
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
#endif
