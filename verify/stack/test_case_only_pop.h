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
    for (vsize_t i = 1; i <= 4; i++) {
        push(MAIN_TID, ds_idx, i);
    }
}

void
t0(vsize_t tid)
{
    stack_enter(tid);
    vuintptr_t val_1 = pop(tid, ds_idx);
    vuintptr_t val_2 = pop(tid, ds_idx);
    stack_exit(tid);
    ASSERT(val_1 > val_2);
}

void
t1(vsize_t tid)
{
    stack_enter(tid);
    vuintptr_t val_1 = pop(tid, ds_idx);
    stack_exit(tid);
    ASSERT(val_1);
}

vbool_t t2_is_active = false;
void
t2(vsize_t tid)
{
#if !defined(VSYNC_VERIFICATION_QUICK)
    t2_is_active = true;
    stack_enter(tid);
    vuintptr_t val_1 = pop(tid, ds_idx);
    stack_exit(tid);
    stack_clean(tid);
    ASSERT(val_1);
#endif
    V_UNUSED(tid);
}

void
post(void)
{
    vuintptr_t val = pop(MAIN_TID, ds_idx);
    if (t2_is_active) {
        ASSERT(val == 0);
    } else {
        ASSERT(val == 1);
    }
}
#endif
