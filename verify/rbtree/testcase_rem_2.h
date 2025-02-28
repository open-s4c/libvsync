/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Concurrent removes

void
tinit(void)
{
    rbt_add(MAIN_TID, 0);
    rbt_add(MAIN_TID, 1);
}

void
t0(void)
{
    ASSERT(rbt_con(0, 0));
    ASSERT(rbt_rem(0, 0));
    ASSERT(!rbt_con(0, 0));
}

void
t1(void)
{
    ASSERT(rbt_con(1, 1));
    ASSERT(rbt_rem(1, 1));
    ASSERT(!rbt_con(1, 1));
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
