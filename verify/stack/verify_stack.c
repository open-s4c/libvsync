/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#define NTHREADS 3

#include <stdio.h>
#include <time.h>
#include <vsync/common/assert.h>
#include <vsync/vtypes.h>
#include <test/trace_manager.h>
#include <test/thread_launcher.h>
#include <vsync/common/verify.h>
#include <vsync/common/dbg.h>
#include <vsync/atomic.h>
#include <test/stack/stack_interface.h>

void *run(void *args);
void t0(vsize_t);
void t1(vsize_t);
void t2(vsize_t);
void pre(void);
void post(void);
void init(void);

#ifdef TEST_CASE
    #include TEST_CASE
#else
    #error "no test case was defined"
#endif

int
main(void)
{
    init();

    pre();
    launch_threads(NTHREADS, run);
    post();

// This has to be before destroy
#ifndef SKIP_VERIFY
    verify(0);
#endif
    destroy();
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;
    ASSERT(tid < NTHREADS);

    reg(tid);
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
    dereg(tid);
    return NULL;
}
