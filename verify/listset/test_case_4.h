/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

/* Preinsert keys \in [0, 1] in all lists */
#define INIT_K1 0
#define INIT_K2 1

/* goal:
 * Linearizability
 * multiple listsets
 */
const vsize_t g_fst_lst_idx = 0;
const vsize_t g_2nd_lst_idx = 1;
void
pre(void)
{
	vsize_t tid = 0;
	lst_reg(tid);
	for (vlistset_key_t k = INIT_K1; k <= INIT_K2; k++) {
		for (vsize_t i = 0; i < N_DS; i++) {
			lst_add(tid, i, k);
		}
	}
	lst_dereg(tid);
}
void
t0(vsize_t tid)
{
	/* remove 0 from list 0 */
	ASSERT(lst_rem(tid, g_fst_lst_idx, 0));
	/* remove 1 from list 1 */
	ASSERT(lst_rem(tid, g_2nd_lst_idx, 1));
}
void
t1(vsize_t tid)
{
	/* if 1 is removed from list 1 */
	if (!lst_con(tid, g_2nd_lst_idx, 1)) {
		/* 0 must be removed from list 0 */
		ASSERT(!lst_con(tid, g_fst_lst_idx, 0));
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
	lst_verify_traces(g_fst_lst_idx);
	lst_verify_traces(g_2nd_lst_idx);
}
