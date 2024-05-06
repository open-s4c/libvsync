/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <test/map/itreeset.h>
#include <test/vmem_stdlib.h>

int
main(void)
{
    treeset_t tree;
    int res;

    vmem_lib_t mem_lib = VMEM_LIB_DEFAULT();
    treeset_init(&tree, mem_lib);

    treeset_key_t key    = 3;
    int value            = 3;
    int *out_value       = NULL;
    void **ptr_out_value = (void **)&out_value;

    res = treeset_add(&tree, key, &value, ptr_out_value);
    ASSERT(res);
    ASSERT(out_value == NULL);

    res = treeset_add(&tree, key, &value, ptr_out_value);
    ASSERT(!res);
    ASSERT(out_value == &value);

    out_value = NULL;
    res       = treeset_contains(&tree, key, ptr_out_value);
    ASSERT(res);
    ASSERT(out_value == &value);

    out_value = NULL;
    res       = treeset_remove(&tree, key, ptr_out_value);
    ASSERT(res);
    ASSERT(out_value == &value);

    out_value = NULL;
    res       = treeset_remove(&tree, key, ptr_out_value);
    ASSERT(!res);
    ASSERT(out_value == NULL);

    res = treeset_contains(&tree, key, ptr_out_value);
    ASSERT(!res);
    ASSERT(out_value == NULL);

    treeset_destroy(&tree);
}
