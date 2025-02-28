/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Linearizability of adds

void
tinit(void)
{
}

void
t0(void)
{
    ASSERT(rbt_add(0, 0));
    ASSERT(rbt_add(0, 1));
}

void
t1(void)
{
    if (rbt_con(1, 1)) {
        ASSERT(rbt_con(1, 0));
        ASSERT(rbt_con(1, 1));
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
