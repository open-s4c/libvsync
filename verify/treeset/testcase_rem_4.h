/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
// Forcing execution of complicated cases when removing

void
pre(void)
{
    for (int i = 0; i < 9; ++i) {
        tr_add(i);
    }
}

void
t0(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(tr_rem(0));
    ASSERT(tr_rem(3));
    ASSERT(tr_rem(6));
}

void
t1(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(tr_rem(1));
    ASSERT(tr_rem(4));
    ASSERT(tr_rem(7));
}

void
t2(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(tr_rem(2));
    ASSERT(tr_rem(5));
    ASSERT(tr_rem(8));
}

void
t3(vsize_t tid)
{
    V_UNUSED(tid);
}

void
post(void)
{
}
#endif
