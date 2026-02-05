/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#undef NTHREADS
#define NTHREADS 2U
/**
 * @file spsc_sanity.c
 * @brief assert FIFO order of single producer, single consumer
 *
 */
#include <test/impsc.h>
#include <test/thread_launcher.h>
#include <vsync/common/assert.h>

#if defined(VSYNC_ADDRESS_SANITIZER)
    #define NUM_ENTRIES 10000U
#else
    #define NUM_ENTRIES 100000U
#endif

void
consume(void)
{
    vuintptr_t v = 0;
    for (vuintptr_t i = 1; i <= NUM_ENTRIES; i++) {
        while (v = deq(), !v) {
            /* ignore empty dequeues */
        }
        ASSERT(v == i);
    }
}
void
produce(vsize_t tid)
{
    for (vuintptr_t i = 1; i <= NUM_ENTRIES; i++) {
        enq(tid, i);
    }
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;
    if (tid == 0) {
        consume();
    } else if (tid == 1) {
        produce(tid);
    }
    return NULL;
}

int
main(void)
{
    init();
    launch_threads(NTHREADS, run);
    fini();
    return 0;
}
