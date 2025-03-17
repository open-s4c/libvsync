/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
/* Preinsert keys \in [1] in all lists */
#define INIT_K1 0
#define INIT_K2 1

/* goal:
 * Linearizability
 * multiple listsets add
 */
const vsize_t g_fst_lst_idx = 0;
const vsize_t g_2nd_lst_idx = 1;

void
pre(void)
{
    for (vlistset_key_t k = INIT_K1; k <= INIT_K2; k++) {
        for (vsize_t i = 0; i < N_DS; i++) {
            lst_add(MAIN_TID, i, k);
        }
    }
}
void
t0(vsize_t tid)
{
    /* add 2 to list 0 */
    ASSERT(lst_add(tid, g_fst_lst_idx, 2));
    /* add 2 to list 1 */
    ASSERT(lst_add(tid, g_2nd_lst_idx, 2));
}
void
t1(vsize_t tid)
{
    /* if 2 is added to list 1 */
    if (lst_con(tid, g_2nd_lst_idx, 2)) {
        /* 1 must be added to list 0 */
        ASSERT(lst_con(tid, g_fst_lst_idx, 2));
    } else {
        verification_ignore();
    }
}
void
t2(vsize_t tid)
{
    V_UNUSED(tid);
}

void
post(void)
{
    lst_verify_traces(g_fst_lst_idx);
    lst_verify_traces(g_2nd_lst_idx);
}
#endif
