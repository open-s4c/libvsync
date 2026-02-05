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
#define CONSUME_COUNT            (IT * NTHREADS)
void
consume(void)
{
    usleep(CONSUMER_DELAY_MICRO_SEC);
    vuintptr_t last = 0;
    vuintptr_t v    = 0;
    for (vsize_t i = 0; i < CONSUME_COUNT; i++) {
        v = deq();
        if (v != 0 && v != IT + 1) {
            if (last != 0) {
                ASSERT(v == last + 1);
            }
            last = v;
        }
    }
}
void
produce(vsize_t tid)
{
    vuintptr_t v = 0;
    for (vsize_t i = 1; i <= IT; i++) {
        if (tid == 1) {
            v = i;
        } else {
            v = IT + 1;
        }
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
