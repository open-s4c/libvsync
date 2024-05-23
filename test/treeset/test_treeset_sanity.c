/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <test/map/itreeset.h>
#include <test/map/treeset_test_interface.h>

#define NKEYS 4000

void
test_add_rem_con(vsize_t num)
{
    vsize_t n      = 0;
    vbool_t *exist = malloc(num * sizeof(vbool_t));
    for (vsize_t i = 0; i < num; ++i) {
        exist[i] = false;
    }

    vsize_t oper = 0;

    while (n < num) {
        int key         = random() % num;
        vbool_t suc_con = tr_con(key);
        vbool_t suc_add = tr_add(key);

        ASSERT(suc_con == exist[key]);
        ASSERT(suc_add == !exist[key]);

        tr_verify();

        if (!exist[key]) {
            exist[key] = true;
            n++;
        }
        oper++;
    }

    printf("Operations: %zu\n", oper);

    oper = 0;
    n    = 0;
    while (n < num) {
        int key         = random() % num;
        vbool_t suc_con = tr_con(key);
        vbool_t suc_rem = tr_rem(key);

        ASSERT(suc_con == exist[key]);
        ASSERT(suc_rem == exist[key]);

        tr_verify();

        if (exist[key]) {
            exist[key] = false;
            n++;
        }
        oper++;
    }

    printf("Operations: %zu\n", oper);

    free(exist);
}

int
main(void)
{
    tr_init();

    test_add_rem_con(NKEYS);

    tr_destroy();
}
