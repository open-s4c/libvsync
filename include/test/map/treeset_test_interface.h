/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TREESET_TEST_INTERFACE
#define VSYNC_TREESET_TEST_INTERFACE

#include <test/vmem_stdlib.h>

treeset_t g_tree;

static inline vbool_t
tr_add(treeset_key_t key)
{
    vbool_t success = treeset_add(&g_tree, key, NULL, NULL);
    return success;
}

static inline vbool_t
tr_rem(treeset_key_t key)
{
    vbool_t success = treeset_remove(&g_tree, key, NULL);
    return success;
}

static inline vbool_t
tr_con(treeset_key_t key)
{
    vbool_t success = treeset_contains(&g_tree, key, NULL);
    return success;
}

static inline void
tr_init(void)
{
    vmem_lib_t mem_lib = VMEM_LIB_DEFAULT();
    treeset_init(&g_tree, mem_lib);
}

static inline void
tr_destroy(void)
{
    treeset_destroy(&g_tree);
}

static inline void
tr_verify(void)
{
    _treeset_verify(&g_tree);
}

#endif
