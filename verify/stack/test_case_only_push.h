/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
vsize_t ds_idx       = 0;
vuintptr_t popped[5] = {0};

void
pre(void)
{
}

void
t0(vsize_t tid)
{
    push(tid, ds_idx, 1);
    push(tid, ds_idx, 2);
}

void
t1(vsize_t tid)
{
    push(tid, ds_idx, 3);
}

vbool_t t2_is_active = false;
void
t2(vsize_t tid)
{
#if !defined(VSYNC_VERIFICATION_QUICK)
    t2_is_active = true;
    push(tid, ds_idx, 4);
#endif
}

void
post(void)
{
    vsize_t len = t2_is_active ? 4 : 3;

    for (vsize_t i = 0; i < len; i++) {
        popped[i] = pop(MAIN_TID, ds_idx);
        ASSERT(popped[i]);
        if (popped[i] == 1) {
            vbool_t found = false;
            /* if 1 is popped then then 2 have been popped ... */
            for (vsize_t j = 0; j < i; j++) {
                if (2 == popped[j]) {
                    found = true;
                }
            }
            ASSERT(found);
        }
    }

    ASSERT(pop(MAIN_TID, ds_idx) == 0);
}
#endif
