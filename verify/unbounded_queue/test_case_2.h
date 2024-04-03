/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

/**
 * for thread with tid = 0
 *
 */
void
t1(vsize_t tid)
{
	enq(tid, 1, 'A');
	enq(tid, 2, 'B');
}

/**
 * for thread with tid = 1
 *
 */
data_t *deq_1 = NULL;
data_t *deq_2 = NULL;
void
t2(vsize_t tid)
{
	deq_1 = deq(tid);
	deq_2 = deq(tid);
}
/**
 * for thread with tid = 2
 *
 */
void
t3(vsize_t tid)
{
	queue_clean(tid);
}

void
verify(void)
{
	switch (g_len) {
		case 2:
			ASSERT(!deq_1);
			ASSERT(!deq_2);
			break;
		case 1:
			if (deq_1) {
				ASSERT(deq_1->key == 1);
				ASSERT(!deq_2);
			} else if (deq_2) {
				ASSERT(deq_2->key == 1);
				ASSERT(!deq_1);
			} else {
				ASSERT(0 &&
					   "we expect at least one dequeue to succeed if the final "
					   "state length is 1");
			}
			ASSERT(g_final_state[0] == 2);
			break;
		case 0:
			ASSERT(deq_1);
			ASSERT(deq_2);
			// verify FIFO
			ASSERT(deq_1->key == 1);
			ASSERT(deq_2->key == 2);
			break;
		default:
			ASSERT(0 && "the length makes no sense");
			break;
	}
}
