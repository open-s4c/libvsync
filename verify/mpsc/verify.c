/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <test/impsc.h>
#include <pthread.h>
#include <stdio.h>
#include <vsync/common/macros.h>
#include <vsync/common/assert.h>
#include <test/thread_launcher.h>

#ifdef TEST_CASE
    #include TEST_CASE
#else
    #error "no test case was defined"
#endif

void *
run(void *args)
{
    vsize_t tid = (vsize_t)(uintptr_t)args;
    switch (tid) {
        case 0:
            consumer();
            break;
        case 1:
            p1();
            break;
        case 2:
            p2();
            break;
        default:
            break;
    }
    return NULL;
}

int
main(void)
{
    init();
    pre();
    launch_threads(NTHREADS, run);
    post();
    fini();
    return 0;
}
