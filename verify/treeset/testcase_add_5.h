/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Linearizability of adds from different threads

int r2, r3;

void
tinit(void)
{
}

void
t0(void)
{
    ASSERT(tr_add(0));
}

void
t1(void)
{
    ASSERT(tr_add(1));
}

void
t2(void)
{
    r2 = tr_con(0) && !tr_con(1);
}

void
t3(void)
{
    r3 = tr_con(1) && !tr_con(0);
}

void
tfini(void)
{
    ASSERT(!(r2 && r3));
}
