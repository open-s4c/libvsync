/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
// Concurrent adds/removes with same/different keys

int r00, r01, r10, r11, r20, r21, r30, r31;

void
pre(void)
{
}

void
t0(vsize_t tid)
{
    V_UNUSED(tid);
    r00 = tr_add(0);
    r01 = tr_add(1);
}

void
t1(vsize_t tid)
{
    V_UNUSED(tid);
    r10 = tr_add(0);
    r11 = tr_add(1);
}

void
t2(vsize_t tid)
{
    V_UNUSED(tid);
    r20 = tr_rem(0);
    r21 = tr_rem(1);
}

void
t3(vsize_t tid)
{
    V_UNUSED(tid);
    r30 = tr_rem(0);
    r31 = tr_rem(1);
}

void
post(void)
{
    int con0      = r00 + r10 - r20 - r30;
    int con1      = r01 + r11 - r21 - r31;
    int real_con0 = tr_con(0);
    int real_con1 = tr_con(1);
    ASSERT(con0 == real_con0);
    ASSERT(con1 == real_con1);
}
#endif
