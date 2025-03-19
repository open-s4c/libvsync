/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
#define INIT_SIZE 4
vskiplist_key_t g_init_keys[INIT_SIZE] = {0, 2, 5, 10};
vskiplist_key_t g_next_key             = 0;
vskiplist_key_t g_removed_key          = 0;
void
t0(vsize_t tid)
{
    skip_enter(tid);
    add_key(tid, 3, NO_EXPECTATION);
    skip_exit(tid);
}
void
t1(vsize_t tid)
{
    vbool_t success        = false;
    vskiplist_node_t *node = NULL;
    vskiplist_node_t *succ = NULL;

    skip_enter(tid);
    node = skip_lookup_next(tid, DS_IDX, 3);
    ASSERT(node);

    if (skiplist_mock_node_cmp(node, 3) == 0) {
        g_next_key = 3;
    } else if (skiplist_mock_node_cmp(node, 5) == 0) {
        g_next_key = 5;
    } else {
        // either the node is with key 3 or 5
        ASSERT(0);
    }
    // if 3 was found we expect the succ to have 5 as a key, and if 5 was found
    // we expect the succ to have 10 as a key
    g_removed_key = g_next_key == 3 ? 5 : 10;

    succ = skip_get_next(tid, DS_IDX, node);
    ASSERT(skiplist_mock_node_cmp(succ, g_removed_key) == 0);

    // passing a different address
    success = skip_rem_node(tid, DS_IDX, g_removed_key, node);
    ASSERT(!success);

    // passing correct address
    success = skip_rem_node(tid, DS_IDX, g_removed_key, succ);
    ASSERT(success);
    skip_exit(tid);
    skip_clean(tid);
}
void
t2(vsize_t tid)
{
    vskiplist_node_t *node = NULL;
    vskiplist_node_t *succ = NULL;
    skip_enter(tid);
    node = skip_lookup_next(tid, DS_IDX, 11);
    ASSERT(node == NULL);
    node = skip_lookup_next(tid, DS_IDX, 10);
    if (node) {
        succ = skip_get_next(tid, DS_IDX, node);
        ASSERT(succ == NULL);
    }
    skip_exit(tid);
}
void
pre(void)
{
}
void
post(void)
{
    // this key has been removed
    lookup_key(MAIN_TID, g_removed_key, FAIL_EXPECTED);
    // for the final verification, I am adding the added key in place of the
    // removed one, verify will make sure it has been added
    if (g_removed_key == 5) {
        g_init_keys[2] = 3;
    } else {
        g_init_keys[3] = 3;
    }
}
#endif
