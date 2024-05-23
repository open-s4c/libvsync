/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#define NTHREADS 3U
#include <test/map/isimple.h>
#include <test/thread_launcher.h>

void t0(vsize_t);
void t1(vsize_t);
void t2(vsize_t);
void pre(void);
void post(void);

#ifdef TEST_CASE
    #include TEST_CASE
#else
    #error "no test case was defined"
#endif


void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;
    imap_reg(tid);
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
    imap_dereg(tid);
    return NULL;
}

int
main(void)
{
    imap_init();
    pre();
    launch_threads(NTHREADS, run);
    post();
    imap_destroy();
    return 0;
}
