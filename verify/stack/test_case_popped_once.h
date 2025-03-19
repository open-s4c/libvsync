/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
vsize_t ds_idx = 0;

#define MAX_PUSHING 4
int pop_counters[MAX_PUSHING] = {0};

void
pre(void)
{
    for (vsize_t i = 1; i <= 3; i++) {
        push(NTHREADS, ds_idx, i);
    }
}

void
update_counters(vuintptr_t val)
{
    if (val == 0)
        return;
    for (vsize_t i = 0; i < MAX_PUSHING; i++) {
        if (val - 1 == i) {
            pop_counters[i]++;
        }
    }
}

void
t0(vsize_t tid)
{
    stack_enter(tid);
    vuintptr_t val1 = pop(tid, ds_idx);

    update_counters(val1);

    vuintptr_t val2 = pop(tid, ds_idx);
    stack_exit(tid);

    update_counters(val2);

    if (val2 != 4) {
        ASSERT(val1 > val2);
    }
}

void
t1(vsize_t tid)
{
    stack_enter(tid);
    vuintptr_t val = pop(tid, ds_idx);
    update_counters(val);
    stack_exit(tid);

    push(tid, ds_idx, 4);
}

void
t2(vsize_t tid)
{
#if defined(VSYNC_VERIFICATION_QUICK)
    V_UNUSED(tid);
#else
    stack_clean(tid);
#endif
}

void
post(void)
{
    vsize_t count_not_popped = 0;
    for (vsize_t i = 0; i < MAX_PUSHING; i++) {
        ASSERT(pop_counters[i] <= 1);
        if (pop_counters[i] == 0) {
            count_not_popped++;
        }
    }
    ASSERT(count_not_popped == 1);
}
#endif
