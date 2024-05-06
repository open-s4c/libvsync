/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <pthread.h>
#include <vsync/common/assert.h>

#include "mock_mutex.h"
#include <vsync/thread/cond.h>
#include <test/thread_launcher.h>

#ifdef VSYNC_VERIFICATION_QUICK
    #define NTHREADS 2
#else
    #define NTHREADS 3
#endif

vcond_t g_cond;
vmutex_t g_mutex;
int g_cs_x, g_cs_y;

void *
run(void *arg)
{
    vmutex_acquire(&g_mutex);
    if (g_cs_x++ < NTHREADS - 1) {
        while (g_cs_x != NTHREADS) {
            vcond_wait(&g_cond, &g_mutex);
        }
        g_cs_y++;
    } else {
        g_cs_y -= NTHREADS;
    }
    vmutex_release(&g_mutex);
    vcond_signal(&g_cond);

    V_UNUSED(arg);
    return NULL;
}

int
main(void)
{
    launch_threads(NTHREADS, run);
    ASSERT(g_cs_x == NTHREADS);
    ASSERT(g_cs_y == -1);
    return 0;
}
