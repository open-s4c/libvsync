/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#if defined(VSYNC_ADDRESS_SANITIZER)
    #undef NTHREADS
    #undef IT
    #define NTHREADS 4U
    #define IT       128U
#endif
#include <test/impsc.h>
#include <test/thread_launcher.h>
#include <vsync/common/assert.h>
#define CONSUMER_DELAY_MICRO_SEC 200U
void
consume(void)
{
    usleep(CONSUMER_DELAY_MICRO_SEC);
    for (vsize_t i = 0; i < IT; i++) {
        deq();
    }
}
void
produce(vsize_t tid)
{
    vuintptr_t v = 0;
    for (vsize_t i = 1; i <= IT; i++) {
        v = (NTHREADS * tid) + i;
        enq(tid, v);
    }
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;
    if (tid == 0) {
        consume();
    } else {
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
