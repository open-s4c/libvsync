/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
#define INIT_SIZE 3
vskiplist_key_t g_init_keys[INIT_SIZE] = {3, 5, 6};
void
t0(vsize_t tid)
{
    skip_enter(tid);
    add_key(tid, 4, SUCC_EXPECTED);
    skip_exit(tid);
}
void
t1(vsize_t tid)
{
    skip_enter(tid);
    rem_key(tid, 5, SUCC_EXPECTED);
    skip_exit(tid);
    skip_clean(tid);
}
void
t2(vsize_t tid)
{
    skip_enter(tid);
    lookup_key(tid, 6, SUCC_EXPECTED);
    skip_exit(tid);
}
void
pre(void)
{
}
void
post(void)
{
    lookup_key(MAIN_TID, 4, SUCC_EXPECTED);
    lookup_key(MAIN_TID, 5, FAIL_EXPECTED);
    // replacing 5 with 6, 5 is removed
    g_init_keys[1] = 6;
}
#endif
