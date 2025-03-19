/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
#define PRE_KEYS_LEN 2U

vuintptr_t g_pre_keys[PRE_KEYS_LEN] = {1, 3};
vuintptr_t g_new_key                = 2;
vbool_t g_added                     = false;
vbool_t g_got_nw_val                = false;

void
pre(void)
{
    for (vsize_t i = 0; i < PRE_KEYS_LEN; i++) {
        vbool_t success = imap_add(MAIN_TID, g_pre_keys[i], g_pre_keys[i]);
        ASSERT(success);
    }
}
void
t0(vsize_t tid)
{
    ASSERT(tid < PRE_KEYS_LEN);
    data_t *data = imap_get(tid, g_pre_keys[tid]);
    ASSERT(data);
}
void
t1(vsize_t tid)
{
    ASSERT(tid < PRE_KEYS_LEN);
    data_t *data = imap_get(tid, g_pre_keys[tid]);
    ASSERT(data);
}
void
t2(vsize_t tid)
{
    vbool_t success = imap_add(tid, g_new_key, g_new_key);
    ASSERT(success);
    data_t *data = imap_get(tid, g_new_key);
    ASSERT(data);
}
void
post(void)
{
    for (vsize_t i = 0; i < PRE_KEYS_LEN; i++) {
        data_t *data = imap_get(MAIN_TID, g_pre_keys[i]);
        ASSERT(data);
    }
    data_t *data = imap_get(MAIN_TID, g_new_key);
    ASSERT(data);
}
#endif
