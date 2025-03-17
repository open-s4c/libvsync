/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <vsync/vtypes.h>
#include <test/thread_launcher.h>
#include <vsync/common/verify.h>
#include <pthread.h>

#define SMR_MAX_NTHREADS 3

void *run(void *args);
void verify(void);
void prepare(void);
void t0(vsize_t);
void t1(vsize_t);
void t2(vsize_t);

#ifdef TEST_CASE
    #include TEST_CASE
#else
    #error "no test case was defined"
#endif

void *
run(void *args)
{
    vsize_t tid = ((vsize_t)args);
    ismr_reg(tid);
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
    ismr_dereg(tid);
    return NULL;
}

int
main(void)
{
    ismr_init();
    prepare();
    launch_threads(SMR_MAX_NTHREADS, run);
    verify();
    ismr_destroy();
    ASSERT(mock_node_no_leak());
    return 0;
}
