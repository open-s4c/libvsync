/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#define NTHREADS 1U
#include <test/map/isimple.h>
#include <stdlib.h>

#define MIN_KEY 1U
#define MAX_KEY VSIMPLE_HT_CAPACITY
#define TID     MAIN_TID

void
ut_insert_full(void)
{
    vuintptr_t key  = 0;
    vbool_t success = false;
    vsize_t cnt     = 0;

    for (key = MIN_KEY; key <= MAX_KEY; key++) {
        success = imap_add(TID, key, key);
        ASSERT(success);
        cnt++;
    }
    ASSERT(cnt == VSIMPLE_HT_CAPACITY);
    DBG_YELLOW("Inserted %zu entries", cnt);
}

void
ut_get_full(void)
{
    vuintptr_t key = 0;
    data_t *data   = NULL;

    for (key = MIN_KEY; key <= MAX_KEY; key++) {
        data = imap_get(TID, key);
        ASSERT(data);
        ASSERT(data->key == key);
        ASSERT(data->val == key);
    }
    DBG_YELLOW("Found all inserted entries");
}

void
ut_rem_insert(void)
{
    data_t *data    = NULL;
    vbool_t success = false;

    success = imap_rem(TID, MAX_KEY);
    ASSERT(success);
    data = imap_get(TID, MAX_KEY);
    ASSERT(data == NULL);
    success = imap_add(TID, MAX_KEY, MAX_KEY);
    ASSERT(success);
    data = imap_get(TID, MAX_KEY);
    ASSERT(data);
    ASSERT(data->key == MAX_KEY);
    ASSERT(data->val == MAX_KEY);
}

void
ut_rem_full(void)
{
    vuintptr_t key  = 0;
    vbool_t success = false;
    vsize_t cnt     = 0;

    for (key = MIN_KEY; key <= MAX_KEY; key++) {
        success = imap_rem(TID, key);
        ASSERT(success);
        cnt++;
    }
    ASSERT(cnt == VSIMPLE_HT_CAPACITY);
    DBG_YELLOW("Removed %zu entries", cnt);
}

int
main(void)
{
    imap_init();
    imap_reg(TID);

    ut_insert_full();
    imap_print();
    ut_get_full();
    ut_rem_insert();
    ut_rem_full();
    imap_print();

    imap_dereg(TID);
    imap_destroy();
    return 0;
}
