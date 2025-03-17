/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <pthread.h>
#include <vsync/common/assert.h>
#include <vsync/thread/once.h>
#include <test/thread_launcher.h>

#define NTHREADS 4U

vonce_t g_once   = VONCE_INIT();
vsize_t g_winner = 0;

void *
verification_once_cb(void *arg)
{
    g_winner = (vsize_t)(vuintptr_t)arg;
    ASSERT(g_winner != 0);
    return arg;
}

void *
run(void *arg)
{
    vsize_t tid = ((vsize_t)(vuintptr_t)arg) + 1U;
    ASSERT(tid != 0);

    void *r =
        vonce_call(&g_once, verification_once_cb, (void *)(vuintptr_t)tid);

    if (r != NULL) {
        /* if once returns something, then I was the winner */
        ASSERT(tid == (vsize_t)(vuintptr_t)r);
        ASSERT(g_winner == tid);
    } else {
        /* if once returned nothing, then somebody else is the winner */
        ASSERT(g_winner != tid);
    }
    return NULL;
}

int
main(void)
{
    launch_threads(NTHREADS, run);
    return 0;
}
