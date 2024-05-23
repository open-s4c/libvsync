/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifdef VSYNC_VERIFICATION_QUICK
    #define NTHREADS 2
#else
    #define NTHREADS 3
#endif

#include <test/boilerplate/lock.h>
#include <vsync/thread/mutex/musl.h>

vmutex_t lock;

void
acquire(vuint32_t tid)
{
    V_UNUSED(tid);
    vmutex_acquire(&lock);
}

void
release(vuint32_t tid)
{
    V_UNUSED(tid);
    vmutex_release(&lock);
}
