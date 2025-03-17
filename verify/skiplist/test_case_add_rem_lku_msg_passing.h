/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
#define INIT_SIZE 4

vskiplist_key_t g_init_keys[INIT_SIZE] = {0, 2, 5, 10};
int g_msg                              = 0;
const vskiplist_key_t g_key            = 3;
vbool_t g_t2_rem_3_ret                 = false;
void
t0(vsize_t tid)
{
    g_msg = 10;
    skip_enter(tid);
    add_key(tid, g_key, SUCC_EXPECTED);
    skip_exit(tid);
}
void
t1(vsize_t tid)
{
    skip_enter(tid);
    g_t2_rem_3_ret = rem_key(tid, g_key, NO_EXPECTATION);
    skip_exit(tid);
    if (g_t2_rem_3_ret) {
        ASSERT(g_msg == 10);
        skip_clean(tid);
    }
}
void
t2(vsize_t tid)
{
    skip_enter(tid);
    lookup_key(tid, 5, SUCC_EXPECTED);
    skip_exit(tid);
}
void
pre(void)
{
}
void
post(void)
{
    // if remove succeeded, we expect not to find the key
    // and if it fails we expect to find it
    lookup_key(MAIN_TID, g_key, g_t2_rem_3_ret ? FAIL_EXPECTED : SUCC_EXPECTED);
}
#endif
