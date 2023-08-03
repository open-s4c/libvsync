/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#define REACQUIRE 1

#include <vsync/spinlock/mcslock.h>
#include <test/boilerplate/lock.h>

mcslock_t lock = MCSLOCK_INIT();
struct mcs_node_s nodes[NTHREADS];

void
acquire(vuint32_t tid)
{
	if (tid == NTHREADS - 1)
		await_while (!mcslock_tryacquire(&lock, &nodes[tid]))
			;
	else
		mcslock_acquire(&lock, &nodes[tid]);
}

void
release(vuint32_t tid)
{
	mcslock_release(&lock, &nodes[tid]);
}
