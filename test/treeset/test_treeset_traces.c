/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <test/map/itreeset.h>
#include <test/map/treeset_test_interface_traces.h>

#include <test/thread_launcher.h>
#include <test/trace_manager.h>
#include <vsync/common/verify.h>

#ifndef NTHREADS
    #error "Choose number of threads by setting NTHREADS"
#endif

#define NITERS 20000

void *
run(void *arg)
{
    vsize_t tid = (vsize_t)(vuintptr_t)arg;

    for (vsize_t i = 0; i < NITERS; ++i) {
        int key = i;
        if (tr_con_trace(tid, key)) {
            tr_rem_trace(tid, key);
        } else {
            tr_add_trace(tid, key);
        }
    }

    return NULL;
}

int
main(void)
{
    tr_init_trace();

    launch_threads(NTHREADS, run);

    tr_verify_traces();

    tr_destroy_trace();

    tr_verify_allocs();
}
