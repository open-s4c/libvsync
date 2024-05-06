/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Concurrent removes with the same key

int r0, r1, r2;

void
tinit(void)
{
    tr_add(0);
}

void
t0(void)
{
    r0 = tr_rem(0);
}

void
t1(void)
{
    r1 = tr_rem(0);
}

void
t2(void)
{
    r2 = tr_rem(0);
}

void
t3(void)
{
    tr_add(-1);
    tr_add(1);
    tr_add(-2);
    tr_add(2);
}

void
tfini(void)
{
    ASSERT(r0 + r1 + r2 == 1);
}
