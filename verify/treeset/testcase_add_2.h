/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H

// Concurrent adds with the same key

int r0, r1, r2;

void
pre(void)
{
}

void
t0(vsize_t tid)
{
    V_UNUSED(tid);
    r0 = tr_add(0);
}

void
t1(vsize_t tid)
{
    V_UNUSED(tid);
    r1 = tr_add(0);
}

void
t2(vsize_t tid)
{
    V_UNUSED(tid);
    r2 = tr_add(0);
}

void
t3(vsize_t tid)
{
    V_UNUSED(tid);
    tr_add(-1);
    tr_add(1);
    tr_add(-2);
    tr_add(2);
}

void
post(void)
{
    ASSERT(r0 + r1 + r2 == 1);
}
#endif
