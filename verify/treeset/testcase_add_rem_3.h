/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
// Linearizability of adds/removes from the same thread

void
pre(void)
{
    tr_add(0);
    tr_add(3);
}

void
t0(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(tr_rem(0));
    ASSERT(tr_add(2));
}

void
t1(vsize_t tid)
{
    V_UNUSED(tid);
    if (tr_rem(2)) {
        ASSERT(tr_add(0));
    } else {
        verification_ignore();
    }
}

void
t2(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(tr_add(1));
    ASSERT(tr_rem(3));
}

void
t3(vsize_t tid)
{
    V_UNUSED(tid);
    if (tr_add(3)) {
        ASSERT(tr_rem(1));
    } else {
        verification_ignore();
    }
}

void
post(void)
{
}
#endif
