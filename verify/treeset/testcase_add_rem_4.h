/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
// Concurrent adds/removes with same/different keys

void
pre(void)
{
    tr_add(2);
    tr_add(3);
}

void
t0(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(tr_add(0));
    tr_add(2);
}

void
t1(vsize_t tid)
{
    V_UNUSED(tid);
    tr_add(3);
    ASSERT(tr_add(1));
}

void
t2(vsize_t tid)
{
    V_UNUSED(tid);
    tr_rem(0);
    ASSERT(tr_rem(2));
}

void
t3(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(tr_rem(3));
    tr_rem(1);
}

void
post(void)
{
}
#endif
