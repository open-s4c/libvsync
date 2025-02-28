/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <test/listset/listset_test_interface.h>

#define MIN_KEY 3
#define MAX_KEY 5

void
insert(vlistset_key_t k1, vlistset_key_t k2, vlistset_key_t step,
       vbool_t success_expected)
{
    vlistset_key_t key;
    vbool_t success = false;
    for (key = k1; key <= k2; key += step) {
        success = lst_add(MAIN_TID, 0, key);
        ASSERT(success == success_expected);
    }
    V_UNUSED(success);
}

void
rem(vlistset_key_t k1, vlistset_key_t k2, vlistset_key_t step,
    vbool_t success_expected)
{
    vlistset_key_t key;
    vbool_t success = false;
    for (key = k1; key <= k2; key += step) {
        success = lst_rem(MAIN_TID, 0, key);
        ASSERT(success == success_expected);
    }
    V_UNUSED(success);
}

void
find(vlistset_key_t k1, vlistset_key_t k2, vlistset_key_t step,
     vbool_t success_expected)
{
    vlistset_key_t key;
    vbool_t success = false;
    for (key = k1; key <= k2; key += step) {
        success = lst_con(MAIN_TID, 0, key);
        ASSERT(success == success_expected);
    }
    V_UNUSED(success);
}

void
print(char *lbl)
{
    lst_print(0, lbl);
}

int
main(void)
{
    vlistset_key_t min_key = 0;
    vlistset_key_t max_key = 10;

    lsts_init();

    insert(min_key, max_key, 1, true);
    find(min_key, max_key, 1, true);
    rem(min_key, max_key, 1, true);
    find(min_key, max_key, 1, false);

    insert(min_key, max_key, 1, true);

    min_key = MIN_KEY;
    max_key = MAX_KEY;
    insert(min_key, max_key, 1, false);
    find(min_key, max_key, 1, true);

    rem(min_key, max_key, 1, true);
    find(min_key, max_key, 1, false);

    lst_verify_traces(0);
    lsts_destroy();
}
