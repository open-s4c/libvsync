/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <vsync/vtypes.h>

#include <vsync/map/skiplist_lf.h>
#include <test/skiplist/skiplist_interface.h>

#define DS_IDX  0
#define MIN_KEY 0U
#define MAX_KEY 1000U

vskiplist_node_t *
lookup(vskiplist_key_t key, vbool_t expect_success)
{
    vskiplist_node_t *node = NULL;

    node = skip_lookup(MAIN_TID, DS_IDX, key);

    if (expect_success) {
        ASSERT(node);
    } else {
        ASSERT(node == NULL);
    }

    return node;
}

void
lookup_nodes(vskiplist_key_t start_key, vskiplist_key_t end_key,
             vbool_t expect_success, vskiplist_key_t step)
{
    vskiplist_node_t *node      = NULL;
    vskiplist_node_t *next_node = NULL;
    vskiplist_node_t *succ_node = NULL;
    vskiplist_key_t key         = 0;

    for (key = start_key; key <= end_key; key += step) {
        node = lookup(key, expect_success);

        if (expect_success) {
            ASSERT(node);

            next_node = skip_lookup_next(MAIN_TID, DS_IDX, key);

            ASSERT(next_node == node);

            if (key + step <= end_key) {
                succ_node = skip_get_next(MAIN_TID, DS_IDX, node);
                ASSERT(succ_node != next_node);
                ASSERT(skiplist_mock_node_cmp(succ_node, key + step) == 0);
            }
        }
    }
}

void
add_nodes(vskiplist_key_t start_key, vskiplist_key_t end_key,
          vbool_t expect_success, vskiplist_key_t step)
{
    vskiplist_node_t *existing_node = NULL;
    vskiplist_key_t key             = 0;
    vbool_t success                 = true;

    for (key = start_key; key <= end_key; key += step) {
        existing_node = NULL;
        success       = skip_add(MAIN_TID, DS_IDX, key, &existing_node);

        ASSERT(success == expect_success);

        if (success) {
            ASSERT(existing_node == NULL);
            existing_node = lookup(key, true);
        } else {
            ASSERT(existing_node);
            ASSERT(lookup(key, true) == existing_node);
        }
    }
}

void
rem_nodes(vskiplist_key_t start_key, vskiplist_key_t end_key,
          vbool_t expect_success, vskiplist_key_t step)
{
    vskiplist_node_t *next_node = NULL;
    vbool_t success             = true;

    vskiplist_key_t key;

    for (key = start_key; key <= end_key; key += step) {
        lookup(key, expect_success);

        success = skip_rem(MAIN_TID, DS_IDX, key);

        ASSERT(success == expect_success);

        if (success) {
            if (key + 1 <= end_key) {
                next_node = skip_lookup_next(MAIN_TID, DS_IDX, key);
                ASSERT(skiplist_mock_node_cmp(next_node, key + 1) == 0);
            }
        }
        // node is either removed or does not exist
        ASSERT(lookup(key, false) == NULL);
    }
}

void
test_basics(vskiplist_key_t start_key, vskiplist_key_t end_key)
{
    vskiplist_key_t step = 1;

    skip_init();

    add_nodes(start_key, end_key, true, step);

    add_nodes(start_key, end_key, false, step);

    lookup_nodes(start_key, end_key, true, step);

    rem_nodes(start_key, end_key, true, step);

    lookup_nodes(start_key, end_key, false, step);

    skip_destroy();
}

void
test_tip_toe(vskiplist_key_t start_key, vskiplist_key_t end_key)
{
    vskiplist_key_t step = 2;

    skip_init();

    add_nodes(start_key, end_key, true, step);

    add_nodes(start_key + 1, end_key, true, step);

    lookup_nodes(start_key, end_key, true, 1);

    rem_nodes(start_key + 1, end_key, true, step);

    lookup_nodes(start_key, end_key, true, step);

    lookup_nodes(start_key + 1, end_key, false, step);

    skip_destroy();
}

void
test_remove_node(vskiplist_key_t start_key, vskiplist_key_t end_key)
{
    vskiplist_key_t key;
    vbool_t success;
    vskiplist_node_t *node = NULL;

    skip_init();

    add_nodes(start_key, end_key, true, 1);

    for (key = start_key; key <= end_key; key += 2) {
        node = skip_lookup(MAIN_TID, DS_IDX, key);
        ASSERT(node);

        success = skip_rem_node(MAIN_TID, DS_IDX, key, node);
        ASSERT(success);
    }

    lookup_nodes(start_key, end_key, false, 2);

    lookup_nodes(start_key + 1, end_key, true, 2);

    skip_destroy();
}

int
main(void)
{
    test_basics(MIN_KEY, MAX_KEY);

    test_tip_toe(MIN_KEY, MAX_KEY);

    test_tip_toe(MIN_KEY, 1);

    test_remove_node(MIN_KEY, MAX_KEY);

    return 0;
}
