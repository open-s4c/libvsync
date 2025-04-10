/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#define VFUTEX_LIVENESS_SIMPLE
#define NTHREADS 2

#include <test/boilerplate/lock.h>
#include <vsync/thread/mutex/slim.h>

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
