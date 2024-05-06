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

#define NUM_PUSH_PER_THREAD 3

vatomic64_t g_key = VATOMIC_INIT(1);
vsize_t ds_idx    = 0;

void
single_threaded(void)
{
    vuint64_t id       = 0;
    vuint64_t first_id = 1;
    vuint64_t last_id  = 10;
    vuint64_t out_id   = 0;

    init();

    for (id = first_id; id <= last_id; id++) {
        push(MAIN_TID, ds_idx, id);
    }

    for (id = last_id; id >= first_id; id--) {
        out_id = pop(MAIN_TID, ds_idx);
        ASSERT(out_id == id);
    }

    out_id = pop(MAIN_TID, ds_idx);
    ASSERT(out_id == 0);

    verify(ds_idx);

    destroy();
}

void *
run(void *arg)
{
    vsize_t tid    = ((vsize_t)arg);
    vsize_t i      = 0;
    vuintptr_t key = 0;

    reg(tid);

    for (i = 0; i < NUM_PUSH_PER_THREAD; i++) {
        key = vatomic64_get_inc(&g_key);
        push(tid, ds_idx, key);
    }

    vbool_t stop = false;
    while (!stop) {
        stack_enter(tid);
        stop = pop(tid, ds_idx) == 0;
        stack_exit(tid);
    }

    dereg(tid);

    return NULL;
}

void
multi_threaded(void)
{
    init();
    launch_threads(NTHREADS, run);
    verify(ds_idx);
    destroy();
}

int
main(void)
{
    single_threaded();
    multi_threaded();
    return 0;
}
