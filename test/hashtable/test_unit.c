/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <test/hashtable/map.h>
#define TID 0

#define LEN (VHASHTABLE_BUCKET_COUNT * VHASHTABLE_BUCKET_LEN * 10)
user_key_t g_keys[LEN];


vbool_t
check_exists(user_key_t key, vsize_t len)
{
    for (vsize_t j = 0; j < len; j++) {
        if (user_key_eq(key, g_keys[j])) {
            return true;
        }
    }
    return false;
}

void
gen_unique_keys(void)
{
    user_key_t key = {0};
    for (vsize_t i = 0; i < LEN; i++) {
        do {
            key = gen_key();
        } while (check_exists(key, i));
        g_keys[i] = key;
    }
}

void
unit_test_add_get_remove(void)
{
    map_init();

    hash_entry_t *entry = NULL;

    vbool_t success = false;

    /* get -> insert -> get */
    for (vsize_t i = 0; i < LEN; i++) {
        entry = map_get(TID, g_keys[i]);
        ASSERT(entry == NULL);

        success = map_insert(TID, g_keys[i]);
        ASSERT(success);

        entry = map_get(TID, g_keys[i]);
        ASSERT(entry != NULL);

        success = map_insert(TID, g_keys[i]);
        ASSERT(success != true);
    }

    for (vsize_t i = 0; i < LEN; i++) {
        entry = map_get(TID, g_keys[i]);
        ASSERT(entry != NULL);

        success = map_remove(TID, g_keys[i]);
        ASSERT(success);

        entry = map_get(TID, g_keys[i]);
        ASSERT(entry == NULL);

        success = map_remove(TID, g_keys[i]);
        ASSERT(success == false);
    }

    V_UNUSED(success, entry);
    map_destroy();
}

int
main(void)
{
    gen_unique_keys();
    unit_test_add_get_remove();
    return 0;
}
