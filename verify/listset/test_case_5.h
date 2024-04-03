/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
/* Preinsert keys \in [0, 2]  */
#define INIT_K1 0
#define INIT_K2 2

/* goal:
 * removing from head, middle, tail
 */
const vsize_t g_lst_idx = 0;
void
pre(void)
{
	vsize_t tid = 0;
	lst_reg(tid);
	for (vlistset_key_t k = INIT_K1; k <= INIT_K2; k++) {
		ASSERT(lst_add(tid, g_lst_idx, k));
	}
	lst_dereg(tid);
}
void
t0(vsize_t tid)
{
	ASSERT(lst_rem(tid, g_lst_idx, 0));
	ASSERT(!lst_con(tid, g_lst_idx, 0));
}
void
t1(vsize_t tid)
{
	ASSERT(lst_rem(tid, g_lst_idx, 1));
	ASSERT(!lst_con(tid, g_lst_idx, 1));
}
void
t2(vsize_t tid)
{
#if !defined(VSYNC_VERIFICATION_QUICK)
	ASSERT(lst_rem(tid, g_lst_idx, 2));
	ASSERT(!lst_con(tid, g_lst_idx, 2));
#else
	V_UNUSED(tid);
#endif
}
void
post(void)
{
	lst_verify_traces(g_lst_idx);
}
