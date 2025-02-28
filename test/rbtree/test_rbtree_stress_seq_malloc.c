/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Stress test of sequential logic of rbtree
//
// Performs NITERS iterations of:
// - add NKEYS in random order
// - remove NKEYS in different random order

#include <vsync/map/rbtree.h>
#include <test/rand.h>
#include <stdio.h>
#include <stdlib.h>

#define NKEYS  1000U
#define NITERS 1000U / 10

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
    V_UNUSED(args);
    my_node_t *nn = V_CONTAINER_OF(node, my_node_t, embed);
    free(nn);
}

my_node_t *g_nodes[NKEYS];
vrbtree_key_t g_keys[NKEYS];
vsize_t g_perm_add[NKEYS], g_perm_rem[NKEYS];


int
main(void)
{
    vrbtree_t tree;

    vrbtree_init(&tree, _retire_fun, NULL, _cmp_fun);

    for (vsize_t iter = 0; iter < NITERS; ++iter) {
        printf("Iter #%lu\n", iter);
        deterministic_random_init(iter);

        // init

        for (vsize_t i = 0; i < NKEYS; ++i) {
            g_nodes[i]      = malloc(sizeof(my_node_t));
            g_nodes[i]->key = i;
            g_keys[i]       = i;
            g_perm_add[i]   = i;
            g_perm_rem[i]   = i;
        }
        deterministic_random_shuffle(g_perm_add, NKEYS);
        deterministic_random_shuffle(g_perm_rem, NKEYS);

        // add

        for (vsize_t i = 0; i < NKEYS; ++i) {
            my_node_t *node = g_nodes[g_perm_add[i]];
            ASSERT(!vrbtree_get(&tree, node->key));
            ASSERT(vrbtree_add(&tree, node->key, &node->embed));
            ASSERT(vrbtree_get(&tree, node->key) == &node->embed);
            ASSERT(vrbtree_size(&tree) == i + 1);
        }

        for (vsize_t i = 0; i < NKEYS; ++i) {
            ASSERT(vrbtree_get(&tree, g_nodes[i]->key) == &g_nodes[i]->embed);
        }

        // rem

        for (vsize_t i = 0; i < NKEYS; ++i) {
            my_node_t *node   = g_nodes[g_perm_rem[i]];
            vrbtree_key_t key = g_keys[g_perm_rem[i]];
            ASSERT(vrbtree_get(&tree, key) == &node->embed);
            ASSERT(vrbtree_remove(&tree, key));
            ASSERT(!vrbtree_get(&tree, key));
            ASSERT(vrbtree_size(&tree) == NKEYS - 1 - i);
        }

        for (vsize_t i = 0; i < NKEYS; ++i) {
            vrbtree_key_t key = g_keys[i];
            ASSERT(!vrbtree_get(&tree, key));
        }
    }

    vrbtree_destroy(&tree);

    return 0;
}
