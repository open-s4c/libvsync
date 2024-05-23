/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Concurrent adds with different keys

void
tinit(void)
{
}

void
t0(void)
{
    ASSERT(!tr_con(0));
    ASSERT(tr_add(0));
    ASSERT(tr_con(0));
}

void
t1(void)
{
    ASSERT(!tr_con(1));
    ASSERT(tr_add(1));
    ASSERT(tr_con(1));
}

void
t2(void)
{
    ASSERT(!tr_con(2));
    ASSERT(tr_add(2));
    ASSERT(tr_con(2));
}

void
t3(void)
{
}

void
tfini(void)
{
}
