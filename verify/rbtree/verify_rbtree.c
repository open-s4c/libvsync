/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <test/rbtree/rbtree_test_interface.h>

#include <pthread.h>

#include <vsync/map/rbtree.h>
#include <test/thread_launcher.h>
#include <vsync/common/verify.h>

#define NTHREADS 4U

#ifdef TEST_CASE
    #include TEST_CASE
#else
    #error "Choose testcase by setting TEST_CASE"
#endif


void *
run(void *arg)
{
    vsize_t tid = (vsize_t)(vuintptr_t)arg;
    ASSERT(tid < NTHREADS);

    rbt_reg(tid);
    switch (tid) {
        case 0:
            t0();
            break;
        case 1:
            t1();
            break;
        case 2:
            t2();
            break;
        case 3:
            t3();
            break;
    }
    rbt_dereg(tid);

    return NULL;
}

int
main(void)
{
    rbt_init();
    tinit();

    launch_threads(NTHREADS, run);

    tfini();
    rbt_destroy();
}
