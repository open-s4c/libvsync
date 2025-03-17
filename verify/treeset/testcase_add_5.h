/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
// Linearizability of adds from different threads

int r2, r3;

void
pre(void)
{
}

void
t0(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(tr_add(0));
}

void
t1(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(tr_add(1));
}

void
t2(vsize_t tid)
{
    V_UNUSED(tid);
    r2 = tr_con(0) && !tr_con(1);
}

void
t3(vsize_t tid)
{
    V_UNUSED(tid);
    r3 = tr_con(1) && !tr_con(0);
}

void
post(void)
{
    ASSERT(!(r2 && r3));
}
#endif
