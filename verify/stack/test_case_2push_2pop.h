/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
vsize_t ds_idx = 0;

void
pre(void)
{
}

void
t0(vsize_t tid)
{
    push(tid, ds_idx, 1);
    stack_enter(tid);
    pop(tid, ds_idx);
    stack_exit(tid);
}

void
t1(vsize_t tid)
{
    stack_enter(tid);
    pop(tid, ds_idx);
    stack_exit(tid);
    push(tid, ds_idx, 2);
}

void
t2(vsize_t tid)
{
    stack_enter(tid);
    pop(tid, ds_idx);
    stack_exit(tid);
#if !defined(VSYNC_VERIFICATION_QUICK)
    stack_clean(tid);
#endif
}

void
post(void)
{
}
#endif
