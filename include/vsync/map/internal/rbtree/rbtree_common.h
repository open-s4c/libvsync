/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_RBTREE_COMMON_H
#define VSYNC_RBTREE_COMMON_H

#include <vsync/vtypes.h>

struct vrbtree_node_s;
struct vrbtree_s;

typedef vuintptr_t vrbtree_key_t;

/**
 * Compare container(node)->key with the given key.
 *
 * @param node address of `vrbtree_node_t` node to compare its key to the given
 * `key`.
 * @param key to compare to.
 * @return 0 if container(node)->key == key.
 * @return -1 if container(node)->key < key.
 * @return 1 if container(node)->key > key.
 */
typedef vint32_t (*vrbtree_cmp_key_t)(struct vrbtree_node_s *node,
                                      vrbtree_key_t key);

/**
 * This function is expected to free or recycle.
 *
 * @param node address of `vrbtree_node_t` node to recycle/free.
 * @param args the argument will be passed as given by the user, e.g. it can be
 * an allocator address.
 */
typedef void (*vrbtree_handle_node_t)(struct vrbtree_node_s *node, void *args);

static inline void _vrbtree_visit(struct vrbtree_s *tree,
                                  vrbtree_handle_node_t visitor, void *arg);

#endif
