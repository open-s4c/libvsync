/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VLISTSET_COMMON_H
#define VLISTSET_COMMON_H

#include <vsync/vtypes.h>

struct vlistset_node_s;
struct vlistset_s;

typedef vuintptr_t vlistset_key_t;
/**
 * The function is expected to return
 * 0 if container(node)->key == key
 * -1 if container(node)->key < key
 * 1 if container(node)->key > key
 * @param node address of `vlistset_node_t` node to compare its key to the given
 * `key`
 * @param key to compare to
 */
typedef int (*vlistset_cmp_key_t)(struct vlistset_node_s *node,
                                  vlistset_key_t key);
/**
 * This function is expected to free or recycle
 * @param node address of `vlistset_node_t` node to recycle/free
 * @param args the argument will be passed as given by the user, e.g. it can be
 * an allocator address
 */
typedef void (*vlistset_handle_node_t)(struct vlistset_node_s *node,
                                       void *args);

static inline void _vlistset_visit(struct vlistset_s *lst,
                                   vlistset_handle_node_t visitor, void *arg,
                                   vbool_t visitor_destructive);

#endif
