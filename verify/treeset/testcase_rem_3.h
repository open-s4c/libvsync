/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
// Linearizability of removes from the same thread

void
pre(void)
{
    tr_add(0);
    tr_add(1);
    tr_add(2);
}

void
t0(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(tr_rem(0));
    ASSERT(tr_rem(1));
    ASSERT(tr_rem(2));
}

void
t1(vsize_t tid)
{
    V_UNUSED(tid);
    if (!tr_con(1)) {
        ASSERT(!tr_con(0));
    }
    if (!tr_con(2)) {
        ASSERT(!tr_con(0));
        ASSERT(!tr_con(1));
    }
}

void
t2(vsize_t tid)
{
    V_UNUSED(tid);
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
