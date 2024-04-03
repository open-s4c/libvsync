/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

/* start with empty list insert head, middle, tail */
/* goal:
 * removing from head, middle, tail
 */
const vsize_t g_lst_idx = 0;
void
t0(vsize_t tid)
{
	ASSERT(lst_add(tid, g_lst_idx, 0));
	ASSERT(lst_con(tid, g_lst_idx, 0));
}
void
t1(vsize_t tid)
{
	ASSERT(lst_add(tid, g_lst_idx, 1));
	ASSERT(lst_con(tid, g_lst_idx, 1));
}
void
t2(vsize_t tid)
{
	ASSERT(lst_add(tid, g_lst_idx, 2));
	ASSERT(lst_con(tid, g_lst_idx, 2));
}
void
pre(void)
{
}
void
post(void)
{
	lst_verify_traces(g_lst_idx);
}
