/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
vsize_t ds_idx = 0;

vuintptr_t v1 = 0;
vuintptr_t v2 = 0;
vuintptr_t v3 = 0;

vbool_t g_push_msg = false;

void
pre(void)
{
    push(0, ds_idx, 1);
    push(0, ds_idx, 2);
    push(0, ds_idx, 3);
}

void
t0(vsize_t tid)
{
    stack_enter(tid);
    v1 = pop(tid, ds_idx);
    v2 = pop(tid, ds_idx);
    stack_exit(tid);

    if (v2 == 4 || v1 == 4)
        ASSERT(g_push_msg);
    else
        ASSERT(v1 > v2);
}

void
t1(vsize_t tid)
{
    g_push_msg = true;
    push(tid, ds_idx, 4);
}

void
t2(vsize_t tid)
{
#if defined(VSYNC_VERIFICATION_QUICK)
    V_UNUSED(tid);
#else
    stack_enter(tid);
    v3 = pop(tid, ds_idx);
    stack_exit(tid);
    if (v3 == 4)
        ASSERT(g_push_msg);
    stack_clean(tid);
#endif
}

void
post(void)
{
    printf("%lu %lu %lu\n", v1, v2, v3);
    ASSERT(v1 != v3);
    ASSERT(v2 != v3);
}
#endif
