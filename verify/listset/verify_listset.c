/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <time.h>
#include <vsync/common/assert.h>
#include <vsync/vtypes.h>
#include <pthread.h>
#include <test/trace_manager.h>
#include <test/thread_launcher.h>
#include <vsync/common/verify.h>
#include <vsync/atomic.h>
#ifndef NTHREADS
	#define NTHREADS 3
#endif
/**
 * prototypes
 *
 */
void init(void);
void destroy(void);
void t0(vsize_t);
void t1(vsize_t);
void t2(vsize_t);
void pre(void);
void post(void);
void *run(void *args);

#include <test/listset/listset_test_interface.h>

// include the test case
#ifdef TEST_CASE
	#include TEST_CASE
#else
	#error "no test case was defined"
#endif

/******************************************************************************
 * main
 *****************************************************************************/
int
main(void)
{
	init();
	launch_threads(NTHREADS, run);
	destroy();
}
/******************************************************************************
 * init
 *****************************************************************************/
void
init(void)
{
	lsts_init();
	pre();
}
/******************************************************************************
 * destroy
 *****************************************************************************/
void
destroy(void)
{
	post();
	lsts_destroy();
}
/******************************************************************************
 * run
 *****************************************************************************/
void *
run(void *args)
{
	vsize_t tid = (vsize_t)(vuintptr_t)args;
	ASSERT(tid < NTHREADS);

	lst_reg(tid);
	switch (tid) {
		case 0:
			t0(tid);
			break;
		case 1:
			t1(tid);
			break;
		case 2:
			t2(tid);
			break;
		default:
			break;
	}
	lst_dereg(tid);
	return NULL;
}
