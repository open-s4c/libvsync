/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H

// Concurrent adds with different keys
void
pre(void)
{
}

void
t0(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(!tr_con(0));
    ASSERT(tr_add(0));
    ASSERT(tr_con(0));
}

void
t1(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(!tr_con(1));
    ASSERT(tr_add(1));
    ASSERT(tr_con(1));
}

void
t2(vsize_t tid)
{
    V_UNUSED(tid);
    ASSERT(!tr_con(2));
    ASSERT(tr_add(2));
    ASSERT(tr_con(2));
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
