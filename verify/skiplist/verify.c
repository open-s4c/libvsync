/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef NTHREADS
    #define NTHREADS 3
#endif

#define DS_IDX 0
#include <test/skiplist/skiplist_interface.h>
#include <vsync/map/internal/skiplist/skiplist_helper.h>
typedef enum { NO_EXPECTATION, SUCC_EXPECTED, FAIL_EXPECTED } expectation_t;
vbool_t add_key(vsize_t tid, vskiplist_key_t key, expectation_t);
vbool_t rem_key(vsize_t tid, vskiplist_key_t key, expectation_t);
void lookup_key(vsize_t tid, vskiplist_key_t key, expectation_t);

#include <test/boilerplate/test_case.h>

int
main(void)
{
    tc();
    return 0;
}

void
reg(vsize_t tid)
{
    skip_reg(tid);
}
void
dereg(vsize_t tid)
{
    skip_dereg(tid);
}

vbool_t
_matching_expectation(vbool_t ret, expectation_t expectation)
{
    if (expectation == NO_EXPECTATION) {
        return true;
    }
    if (expectation == SUCC_EXPECTED) {
        return ret == true;
    } else if (expectation == FAIL_EXPECTED) {
        return ret == false;
    } else {
        ASSERT(0 && "expectation supported");
    }
    return false;
}

vbool_t
add_key(vsize_t tid, vskiplist_key_t key, expectation_t expectation)
{
    vbool_t success                 = true;
    vskiplist_node_t *existing_node = NULL;
    success = skip_add(tid, DS_IDX, key, &existing_node);
    ASSERT(_matching_expectation(success, expectation));
    return success;
}

vbool_t
rem_key(vsize_t tid, vskiplist_key_t key, expectation_t expectation)
{
    vbool_t success = true;

    success = skip_rem(tid, DS_IDX, key);

    ASSERT(_matching_expectation(success, expectation));

    return success;
}

void
lookup_key(vsize_t tid, vskiplist_key_t key, expectation_t expectation)
{
    vskiplist_node_t *node = NULL;

    node = skip_lookup(tid, DS_IDX, key);

    if (expectation == SUCC_EXPECTED) {
        if (!node) {
            skip_print(DS_IDX);
        }
        ASSERT(node && "node is not found");
    } else if (expectation == FAIL_EXPECTED) {
        ASSERT(node == NULL);
    }
}

void
init(void)
{
    vsize_t i = 0;
    skip_init();
    for (i = 0; i < INIT_SIZE; i++) {
        add_key(MAIN_TID, g_init_keys[i], SUCC_EXPECTED);
    }
}

void
fini(void)
{
#ifndef DONT_CHECK_KEY_EXIST
    vsize_t len = INIT_SIZE;
    for (vsize_t i = 0; i < len; i++) {
        lookup_key(MAIN_TID, g_init_keys[i], SUCC_EXPECTED);
    }
#endif
    skip_print(DS_IDX);
    skip_destroy();
}
