/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTEST_CLEANER_THRD_H
#define VTEST_CLEANER_THRD_H

#include <test/smr/ismr.h>
#include <pthread.h>
#include <vsync/common/assert.h>
#include <unistd.h>
#include <test/thread_launcher.h>
#include <vsync/common/dbg.h>

#define VCLEANER_SLEEP_UB 1000000

typedef struct cleaner_s {
    pthread_t pthread;
    vsize_t tid;
    vsize_t cycles;
    vsize_t pin_core;
    vuint32_t sleep_micro_sec; /* [0,1000000] */
    vatomic8_t stop;
} cleaner_t;

static inline void *
_vcleaner_run(void *args)
{
    cleaner_t *cleaner = args;

    set_cpu_affinity(cleaner->pin_core);

    while (vatomic8_read_rlx(&cleaner->stop) != 1) {
        ismr_recycle(cleaner->tid);
        cleaner->cycles++;
#ifndef VSYNC_VERIFICATION
        usleep(cleaner->sleep_micro_sec);
#endif
    }
    ismr_recycle(cleaner->tid);
    return NULL;
}

static inline void
vcleaner_start(cleaner_t *cleaner, vsize_t tid, vuint32_t sleep_micro_sec,
               vsize_t pin_core)
{
    cleaner->cycles   = 0;
    cleaner->tid      = tid;
    cleaner->pin_core = pin_core;

#ifndef VSYNC_VERIFICATION
    cleaner->sleep_micro_sec = sleep_micro_sec;
    ASSERT(sleep_micro_sec <= VCLEANER_SLEEP_UB);
#endif

#if !defined(SMR_NONE)
    vatomic8_write_rlx(&cleaner->stop, 0);
    pthread_create(&cleaner->pthread, NULL, _vcleaner_run, cleaner);
    DBG_YELLOW("Cleaner thread has been created and it is pinned to core %zu",
               pin_core);
#endif
}

static inline void
vcleaner_stop(cleaner_t *cleaner)
{
#if defined(SMR_NONE)
    V_UNUSED(cleaner);
#else
    vatomic8_write_rlx(&cleaner->stop, 1);
    pthread_join(cleaner->pthread, NULL);
#endif
}

#endif
