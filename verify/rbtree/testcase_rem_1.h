/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Linearizability of removes

void
tinit(void)
{
    rbt_add(MAIN_TID, 0);
    rbt_add(MAIN_TID, 1);
}

void
t0(void)
{
    ASSERT(rbt_rem(0, 0));
    ASSERT(rbt_rem(0, 1));
}

void
t1(void)
{
    if (!rbt_con(1, 1)) {
        ASSERT(!rbt_con(1, 0));
        ASSERT(!rbt_con(1, 1));
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
