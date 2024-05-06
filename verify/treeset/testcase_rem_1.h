/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Concurrent removes with different keys

void
tinit(void)
{
    tr_add(0);
    tr_add(1);
    tr_add(2);
}

void
t0(void)
{
    ASSERT(tr_con(0));
    ASSERT(tr_rem(0));
    ASSERT(!tr_con(0));
}

void
t1(void)
{
    ASSERT(tr_con(1));
    ASSERT(tr_rem(1));
    ASSERT(!tr_con(1));
}

void
t2(void)
{
    ASSERT(tr_con(2));
    ASSERT(tr_rem(2));
    ASSERT(!tr_con(2));
}

void
t3(void)
{
}

void
tfini(void)
{
}
