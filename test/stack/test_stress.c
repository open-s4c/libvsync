/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <test/stack/stack_interface.h>
#include <test/thread_launcher.h>

#ifndef NTHREADS
    #error "undefined number of threads"
#endif

#ifndef IT
    #error "undefined number of iterations"
#endif

vatomic64_t g_key = VATOMIC_INIT(1);

vsize_t ds_idx = 0;

void
pusher(vsize_t tid)
{
    vsize_t i      = 0;
    vuintptr_t key = 0;

    for (i = 0; i < IT; i++) {
        key = vatomic64_get_inc_rlx(&g_key);
        push(tid, ds_idx, key);
    }
}

void
popper(vsize_t tid)
{
    vsize_t i = 0;

    for (i = 0; i < IT; i++) {
        stack_enter(tid);
        pop(tid, ds_idx);
        stack_exit(tid);
    }
}

void *
run(void *arg)
{
    vsize_t tid = ((vsize_t)arg);

    reg(tid);

    if (tid % 2 == 0) {
        pusher(tid);
    } else {
        popper(tid);
    }

    dereg(tid);

    return NULL;
}

int
main(void)
{
    init();
    launch_threads(NTHREADS, run);
    verify(ds_idx);
    destroy();
    return 0;
}
