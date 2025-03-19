/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
#define LEN (VSIMPLE_HT_CAPACITY - 1U)
vuintptr_t g_keys[LEN];
void
pre(void)
{
    vsize_t cnt  = 0;
    vuintptr_t k = 0;
    vuintptr_t b = LEN;

    while (cnt < LEN) {
        k++;
        if (k % VSIMPLE_HT_CAPACITY == b) {
            vbool_t success = imap_add(MAIN_TID, k, k);
            ASSERT(success);
            g_keys[cnt] = k;
            cnt++;
        }
    }
}
void
t0(vsize_t tid)
{
    ASSERT(tid < LEN);
    vbool_t success = imap_add(tid, g_keys[tid], g_keys[tid]);
    ASSERT(!success);
}
void
t1(vsize_t tid)
{
    ASSERT(tid < LEN);
    data_t *data = imap_get(tid, g_keys[tid]);
    ASSERT(data);
}
void
t2(vsize_t tid)
{
    ASSERT(tid < LEN);
    vbool_t success = imap_rem(tid, g_keys[tid]);
    ASSERT(success);
}
void
post(void)
{
}
#endif
