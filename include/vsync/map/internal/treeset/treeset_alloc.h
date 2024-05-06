/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TREESET_ALLOC
#define VSYNC_TREESET_ALLOC

static inline treeset_node_t *
_treeset_get_node(treeset_t *tree)
{
    return tree->mem_lib.malloc_fun(sizeof(treeset_node_t), tree->mem_lib.arg);
}

static inline void
_treeset_put_node(treeset_t *tree, treeset_node_t *node)
{
    tree->mem_lib.free_fun(node, tree->mem_lib.arg);
}

#endif
