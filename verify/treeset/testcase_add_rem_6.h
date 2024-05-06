/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Forcing execution of complicated cases when adding/removing

void
tinit(void)
{
    ASSERT(tr_add(0));
    ASSERT(tr_add(4));
    ASSERT(tr_add(8));
    ASSERT(tr_add(9));
    ASSERT(tr_add(-1));
}

void
t0(void)
{
    ASSERT(tr_add(3));
    ASSERT(tr_add(6));
    ASSERT(tr_rem(0));
}

void
t1(void)
{
    ASSERT(tr_add(1));
    ASSERT(tr_add(7));
    ASSERT(tr_rem(4));
}

void
t2(void)
{
    ASSERT(tr_add(2));
    ASSERT(tr_add(5));
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
