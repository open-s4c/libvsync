/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Concurrent adds/removes

vint32_t r0, r1, r2, r3;

void
tinit(void)
{
}

void
t0(void)
{
    r0 = rbt_add(0, 0);
}

void
t1(void)
{
    r1 = rbt_add(1, 0);
}

void
t2(void)
{
    r2 = rbt_rem(2, 0);
}

void
t3(void)
{
    r3 = rbt_rem(3, 0);
}

void
tfini(void)
{
    vint32_t con      = r0 + r1 - r2 - r3;
    vint32_t real_con = !!rbt_con(MAIN_TID, 0);
    ASSERT(con == real_con);
}
