/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
// Forcing execution of complicated cases when adding/removing

void
pre(void)
{
    ASSERT(tr_add(0));
    ASSERT(tr_add(4));
    ASSERT(tr_add(8));
    ASSERT(tr_add(9));
    ASSERT(tr_add(-1));
}

void
t0(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(tr_add(3));
    ASSERT(tr_add(6));
    ASSERT(tr_rem(0));
}

void
t1(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(tr_add(1));
    ASSERT(tr_add(7));
    ASSERT(tr_rem(4));
}

void
t2(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(tr_add(2));
    ASSERT(tr_add(5));
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
