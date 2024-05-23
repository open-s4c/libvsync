/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Linearizability of adds/removes from the same thread

void
tinit(void)
{
    tr_add(1);
    tr_add(3);
}

void
t0(void)
{
    ASSERT(tr_add(0));
    ASSERT(tr_add(2));
}

void
t1(void)
{
    if (tr_rem(2)) {
        ASSERT(tr_rem(0));
    } else {
        verification_ignore();
    }
}

void
t2(void)
{
    ASSERT(tr_rem(1));
    ASSERT(tr_rem(3));
}

void
t3(void)
{
    if (tr_add(3)) {
        ASSERT(tr_add(1));
    } else {
        verification_ignore();
    }
}

void
tfini(void)
{
}
