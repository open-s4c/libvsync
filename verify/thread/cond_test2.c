/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <pthread.h>
#include <vsync/common/assert.h>

#include "mock_mutex.h"
#include <vsync/thread/cond.h>
#include <test/thread_launcher.h>

#ifdef VSYNC_VERIFICATION_QUICK
    #define NTHREADS 2U
#else
    #define NTHREADS 4U
#endif

vcond_t g_cond;
vmutex_t g_mutex;
vuint32_t g_shared;

void *
run(void *arg)
{
    vsize_t i = (vsize_t)(vuintptr_t)arg;
    vmutex_acquire(&g_mutex);
    g_shared++;
    while (i == 0 && g_shared != NTHREADS) {
        vcond_wait(&g_cond, &g_mutex);
    }
    vmutex_release(&g_mutex);
    vcond_signal(&g_cond);
    return NULL;
}

int
main(void)
{
    launch_threads(NTHREADS, run);
    ASSERT(g_shared == NTHREADS);
    return 0;
}
