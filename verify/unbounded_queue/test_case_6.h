/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/common/verify.h>
#define INIT_WITH_N_NODES 2
#define KICKOUT_SENTINEL
/**
 * This test case is created targeting bugs that  resulting from recycling
 * node (e.g. in the recycle queue)
 */

/**
 * for thread with tid = 0
 *
 */
vbool_t deq_succeeded = false;
void
t1(vsize_t tid)
{
	data_t *node = NULL;

	enq(tid, 3, 'A'); // reuses nothing

	node = deq(tid);
	ASSERT(node);
	ASSERT(node->key == 1);
	free(node);

	node = deq(tid);
	ASSERT(node);
	ASSERT(node->key == 2);
	free(node);
}

/**
 * for thread with tid = 1
 *
 */
void
t2(vsize_t tid)
{
	enq(tid, 4, 'B');
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
	ASSERT(g_len == 2);
}
