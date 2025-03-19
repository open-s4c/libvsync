/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
#define INIT_SIZE 3
#define DONT_CHECK_KEY_EXIST
vskiplist_key_t g_init_keys[INIT_SIZE] = {1, 2, 3};
vbool_t success[2];

vbool_t
_t(vsize_t tid, vskiplist_key_t key)
{
    vbool_t success = false;
    skip_enter(tid);
    vskiplist_node_t *node = skip_lookup(tid, DS_IDX, key);
    if (node) {
        success = skip_rem_node(tid, DS_IDX, key, node);
    }
    skip_exit(tid);
    return success;
}

void
t0(vsize_t tid)
{
    success[tid] = _t(tid, 2);
}
void
t1(vsize_t tid)
{
    success[tid] = _t(tid, 2);
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
    ASSERT(success[0] ^ success[1]);
}
#endif
