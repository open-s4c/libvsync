/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#define INIT_WITH_N_NODES 1
/**
 * for thread with tid = 0
 *
 */
void
t1(vsize_t tid)
{
	enq(tid, 2, 'A');
}

/**
 * for thread with tid = 1
 *
 */
data_t *deq_1 = NULL;
void
t2(vsize_t tid)
{
	deq_1 = deq(tid);
	ASSERT(deq_1);
	// must succeed because the queue was initialized with one element
	ASSERT(deq_1->key == 1);
	queue_clean(tid);
}
/**
 * for thread with tid = 2
 *
 */
void
t3(vsize_t tid)
{
	enq(tid, 3, 'B');
}
void
verify(void)
{
	ASSERT(g_len == 2);
	for (vsize_t i = 0; i < g_len; i++) {
		ASSERT(g_final_state[i] == 2 || g_final_state[i] == 3);
	}
}
