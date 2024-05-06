/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Concurrent adds/removes with same/different keys

int r00, r01, r10, r11, r20, r21, r30, r31;

void
tinit(void)
{
}

void
t0(void)
{
    r00 = tr_add(0);
    r01 = tr_add(1);
}

void
t1(void)
{
    r10 = tr_add(0);
    r11 = tr_add(1);
}

void
t2(void)
{
    r20 = tr_rem(0);
    r21 = tr_rem(1);
}

void
t3(void)
{
    r30 = tr_rem(0);
    r31 = tr_rem(1);
}

void
tfini(void)
{
    int con0      = r00 + r10 - r20 - r30;
    int con1      = r01 + r11 - r21 - r31;
    int real_con0 = tr_con(0);
    int real_con1 = tr_con(1);
    ASSERT(con0 == real_con0);
    ASSERT(con1 == real_con1);
}
