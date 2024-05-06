/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Linearizability of removes from the same thread

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
    ASSERT(tr_rem(0));
    ASSERT(tr_rem(1));
    ASSERT(tr_rem(2));
}

void
t1(void)
{
    if (!tr_con(1)) {
        ASSERT(!tr_con(0));
    }
    if (!tr_con(2)) {
        ASSERT(!tr_con(0));
        ASSERT(!tr_con(1));
    }
}

void
t2(void)
{
}

void
t3(void)
{
}

void
tfini(void)
{
}
