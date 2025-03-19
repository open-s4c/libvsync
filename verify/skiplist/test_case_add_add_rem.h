/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
#define INIT_SIZE 3
vskiplist_key_t g_init_keys[INIT_SIZE] = {0, 5, 10};
vbool_t g_t1_add_3_ret                 = false;
vbool_t g_t2_rem_2_ret                 = false;
vbool_t g_t2_add_3_ret                 = false;
void
t0(vsize_t tid)
{
    skip_enter(tid);
    add_key(tid, 2, SUCC_EXPECTED);
    g_t1_add_3_ret = add_key(tid, 3, NO_EXPECTATION);
#if !defined(VSYNC_VERIFICATION_QUICK)
    // 3 must exist, either because t1 or t2 added it
    lookup_key(tid, 3, SUCC_EXPECTED);
#endif
    skip_exit(tid);
}
void
t1(vsize_t tid)
{
    skip_enter(tid);
    g_t2_rem_2_ret = rem_key(tid, 2, NO_EXPECTATION);
#if !defined(VSYNC_VERIFICATION_QUICK)
    g_t2_add_3_ret = add_key(tid, 3, NO_EXPECTATION);
#endif
    skip_exit(tid);
}
void
t2(vsize_t tid)
{
#if defined(VSYNC_VERIFICATION_QUICK)
    V_UNUSED(tid);
#else
    skip_clean(tid);
#endif
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
    lookup_key(MAIN_TID, 2, !g_t2_rem_2_ret);

    /* they cannot both succeed */
    ASSERT(g_t1_add_3_ret != g_t2_add_3_ret);
}
#endif
