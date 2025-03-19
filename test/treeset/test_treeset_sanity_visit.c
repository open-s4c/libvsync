/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <test/map/itreeset.h>
#include <test/vmem_stdlib.h>
#include <vsync/common/assert.h>
#include <vsync/vtypes.h>

void
visitor(treeset_key_t key, void *value, void *arg)
{
    ASSERT(*(int *)arg == -1);
    vbool_t key_3 = (key == 3 && *(int *)value == 30);
    vbool_t key_5 = (key == 5 && *(int *)value == 50);
    ASSERT(key_3 || key_5);
}

int
main(void)
{
    treeset_t tree;

    vmem_lib_t mem_lib = VMEM_LIB_DEFAULT();
    treeset_init(&tree, mem_lib);

    int value3 = 30;
    treeset_add(&tree, 3, &value3, NULL);

    int value5 = 50;
    treeset_add(&tree, 5, &value5, NULL);

    int arg = -1;
    treeset_visit(&tree, visitor, &arg);

    treeset_destroy(&tree);
}
