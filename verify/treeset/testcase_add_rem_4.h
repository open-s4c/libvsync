/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Concurrent adds/removes with same/different keys

void
tinit(void)
{
    tr_add(2);
    tr_add(3);
}

void
t0(void)
{
    ASSERT(tr_add(0));
    tr_add(2);
}

void
t1(void)
{
    tr_add(3);
    ASSERT(tr_add(1));
}

void
t2(void)
{
    tr_rem(0);
    ASSERT(tr_rem(2));
}

void
t3(void)
{
    ASSERT(tr_rem(3));
    tr_rem(1);
}

void
tfini(void)
{
}
