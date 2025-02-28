/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_RBTREE_TEST_INTERFACE_H
#define VSYNC_RBTREE_TEST_INTERFACE_H

#include <pthread.h>
#include <vsync/map/rbtree.h>

#include <test/vmem_stdlib.h>
#include <test/smr/ismr.h>

typedef struct rbt_mock_node_s {
    vrbtree_key_t key;
    vrbtree_node_t embed;
    smr_node_t smr_node;
} rbt_mock_node_t;

static inline void
_free_callback(smr_node_t *node, void *args)
{
    V_UNUSED(args);
#ifndef RBTREE_STATIC_MALLOC
    rbt_mock_node_t *mock = V_CONTAINER_OF(node, rbt_mock_node_t, smr_node);
    vmem_free(mock);
#else
    V_UNUSED(node);
#endif
}

static inline void
_retire_callback(vrbtree_node_t *node, void *args)
{
    V_UNUSED(args);
    rbt_mock_node_t *mock = V_CONTAINER_OF(node, rbt_mock_node_t, embed);
    ismr_retire(&mock->smr_node, _free_callback, false);
}

static inline vint32_t
_cmp_callback(vrbtree_node_t *node, vrbtree_key_t key)
{
    rbt_mock_node_t *mock = V_CONTAINER_OF(node, rbt_mock_node_t, embed);
    if (mock->key < key) {
        return -1;
    } else if (mock->key > key) {
        return 1;
    } else {
        return 0;
    }
}


vrbtree_t g_tree;

#ifdef RBTREE_STATIC_MALLOC
rbt_mock_node_t NODES[100];
vatomic32_t INDEX;
#endif

static inline vbool_t
rbt_add(vsize_t tid, vrbtree_key_t key)
{
    ismr_enter(tid);
#ifndef RBTREE_STATIC_MALLOC
    rbt_mock_node_t *mock = vmem_malloc(sizeof(rbt_mock_node_t));
#else
    rbt_mock_node_t *mock = &NODES[vatomic32_get_inc(&INDEX)];
#endif
    mock->key = key;

    vbool_t res = vrbtree_add(&g_tree, key, &mock->embed);
    if (res) {
    } else {
#ifndef RBTREE_STATIC_MALLOC
        vmem_free(mock);
#endif
    }
    ismr_exit(tid);
    return res;
}

static inline vbool_t
rbt_rem(vsize_t tid, vrbtree_key_t key)
{
    ismr_enter(tid);
    vbool_t res = vrbtree_remove(&g_tree, key);
    ismr_exit(tid);
    return res;
}

static inline vbool_t
rbt_con(vsize_t tid, vrbtree_key_t key)
{
    ismr_enter(tid);
    vrbtree_node_t *node = vrbtree_get(&g_tree, key);
    if (node) {
        rbt_mock_node_t *mock = V_CONTAINER_OF(node, rbt_mock_node_t, embed);
        ASSERT(mock->key == key);
    }
    ismr_exit(tid);
    return node;
}

static inline void
rbt_init(void)
{
    ismr_init();
    vrbtree_init(&g_tree, _retire_callback, NULL, _cmp_callback);
}

static inline void
rbt_destroy(void)
{
    vrbtree_destroy(&g_tree);
    ismr_destroy();
}

static inline void
rbt_reg(vsize_t tid)
{
    ismr_reg(tid);
}

static inline void
rbt_dereg(vsize_t tid)
{
    ismr_dereg(tid);
}

#endif
