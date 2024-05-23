/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <test/boilerplate/lock.h>
#include <vsync/thread/mutex/tristate.h>

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
