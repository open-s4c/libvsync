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
    push(NTHREADS, ds_idx, 1);
}

void
t0(vsize_t tid)
{
    push(tid, ds_idx, 2);
    push(tid, ds_idx, 3);
}

#define NUM_OF_POPS           3
#define NUM_POSSIBLE_OUTCOMES 10
vuintptr_t allowed_outcomes[NUM_POSSIBLE_OUTCOMES][NUM_OF_POPS] = {
    {1, 0, 0}, {1, 0, 3}, {1, 0, 2}, {1, 2, 0}, {1, 3, 2},
    {1, 2, 3}, {2, 1, 0}, {2, 1, 3}, {2, 3, 1}, {3, 2, 1}};

vuintptr_t outcome[NUM_OF_POPS];

void
t1(vsize_t tid)
{
    vbool_t full_match = false;

    for (vsize_t j = 0; j < NUM_OF_POPS; j++) {
        stack_enter(tid);
        outcome[j] = pop(tid, ds_idx);
        stack_exit(tid);
        DBG("outcome[%zu] popped %" VUINTPTR_FORMAT " \n", j, outcome[j]);
    }

    for (vsize_t i = 0; i < NUM_POSSIBLE_OUTCOMES; i++) {
        full_match = true;

        for (vsize_t j = 0; j < NUM_OF_POPS; j++) {
            if (outcome[j] != allowed_outcomes[i][j]) {
                full_match = false;
                break;
            }
        }

        if (full_match) {
            break;
        }
    }
    ASSERT(full_match);
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
