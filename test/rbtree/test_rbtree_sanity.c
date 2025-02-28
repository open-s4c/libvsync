/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/map/rbtree.h>

typedef struct my_node_s {
    vrbtree_key_t key;
    vrbtree_node_t embed;
} my_node_t;

vint32_t
_cmp_fun(vrbtree_node_t *node, vrbtree_key_t key)
{
    my_node_t *nn = V_CONTAINER_OF(node, my_node_t, embed);
    if (nn->key < key) {
        return -1;
    } else if (nn->key > key) {
        return 1;
    } else {
        return 0;
    }
}

void
_retire_fun(vrbtree_node_t *node, void *args)
{
    V_UNUSED(node, args);
}

void
test_0(void)
{
    vrbtree_t tree;
    vrbtree_key_t key = 3;
    vbool_t res;
    vrbtree_node_t *res_node = NULL;

    vrbtree_init(&tree, _retire_fun, NULL, _cmp_fun);

    my_node_t node;
    node.key = key;

    ASSERT(vrbtree_size(&tree) == 0L);
    ASSERT(vrbtree_first(&tree) == NULL);
    ASSERT(vrbtree_last(&tree) == NULL);

    res = vrbtree_add(&tree, key, &node.embed);
    ASSERT(res);
    res = vrbtree_add(&tree, key, &node.embed);
    ASSERT(!res);

    res_node = vrbtree_get(&tree, key);
    ASSERT(res_node == &node.embed);

    ASSERT(vrbtree_size(&tree) == 1L);
    ASSERT(vrbtree_first(&tree) == &node.embed);
    ASSERT(vrbtree_last(&tree) == &node.embed);

    res = vrbtree_remove(&tree, key);
    ASSERT(res);
    res = vrbtree_remove(&tree, key);
    ASSERT(!res);

    res_node = vrbtree_get(&tree, key);
    ASSERT(!res_node);

    ASSERT(vrbtree_size(&tree) == 0L);

    vrbtree_destroy(&tree);
}

void
test_1(void)
{
    vrbtree_t tree;
    my_node_t nodes[3];
    nodes[0].key = 0;
    nodes[1].key = 2;
    nodes[2].key = 4;

    vrbtree_init(&tree, _retire_fun, NULL, _cmp_fun);

    for (vsize_t i = 0; i < 3; ++i) {
        vrbtree_add(&tree, nodes[i].key, &nodes[i].embed);
    }

    ASSERT(vrbtree_size(&tree) == 3);
    ASSERT(vrbtree_first(&tree) == &nodes[0].embed);
    ASSERT(vrbtree_last(&tree) == &nodes[2].embed);

    ASSERT(vrbtree_predecessor(&tree, 0) == NULL);
    ASSERT(vrbtree_predecessor(&tree, 1) == &nodes[0].embed);
    ASSERT(vrbtree_predecessor(&tree, 2) == &nodes[0].embed);
    ASSERT(vrbtree_predecessor(&tree, 3) == &nodes[1].embed);

    ASSERT(vrbtree_successor(&tree, 1) == &nodes[1].embed);
    ASSERT(vrbtree_successor(&tree, 2) == &nodes[2].embed);
    ASSERT(vrbtree_successor(&tree, 3) == &nodes[2].embed);
    ASSERT(vrbtree_successor(&tree, 4) == NULL);

    vrbtree_destroy(&tree);
}

int
main(void)
{
    test_0();
    test_1();

    return 0;
}
