/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H

vsize_t ds_idx = 0;

int msg_pushed_1 = 0;
int msg_pushed_2 = 0;

void
pre(void)
{
}
void
t0(vsize_t tid)
{
    msg_pushed_1 = 1;
    push(tid, ds_idx, 1);

    msg_pushed_2 = 1;
    push(tid, ds_idx, 2);
}

void
t1(vsize_t tid)
{
    stack_enter(tid);
    vuintptr_t val = pop(tid, ds_idx);
    if (val == 2) {
        ASSERT(msg_pushed_1);
        ASSERT(msg_pushed_2);
        val = pop(tid, ds_idx);
        ASSERT(val == 1);
    } else if (val == 1) {
        ASSERT(msg_pushed_1);
    }
    stack_exit(tid);
}

void
t2(vsize_t tid)
{
    stack_clean(tid);
}

void
post(void)
{
}
#endif
