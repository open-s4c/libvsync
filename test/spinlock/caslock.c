/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#define REACQUIRE 1

#include <vsync/spinlock/caslock.h>
#include <test/boilerplate/lock.h>

caslock_t lock = CASLOCK_INIT();

void
acquire(vuint32_t tid)
{
	if (tid == NTHREADS - 1)
		await_while (!caslock_tryacquire(&lock))
			;
	else
		caslock_acquire(&lock);
}

void
release(vuint32_t tid)
{
	V_UNUSED(tid);
	caslock_release(&lock);
}
