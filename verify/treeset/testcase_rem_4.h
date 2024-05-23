/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Forcing execution of complicated cases when removing

void
tinit(void)
{
    for (int i = 0; i < 9; ++i) {
        tr_add(i);
    }
}

void
t0(void)
{
    ASSERT(tr_rem(0));
    ASSERT(tr_rem(3));
    ASSERT(tr_rem(6));
}

void
t1(void)
{
    ASSERT(tr_rem(1));
    ASSERT(tr_rem(4));
    ASSERT(tr_rem(7));
}

void
t2(void)
{
    ASSERT(tr_rem(2));
    ASSERT(tr_rem(5));
    ASSERT(tr_rem(8));
}

void
t3(void)
{
}

void
tfini(void)
{
}
