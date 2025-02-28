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
#define NITERS 1000U

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

my_node_t g_nodes[NKEYS];
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
            g_nodes[i].key = 2 * i;
            g_perm_add[i]  = i;
            g_perm_rem[i]  = i;
        }
        deterministic_random_shuffle(g_perm_add, NKEYS);
        deterministic_random_shuffle(g_perm_rem, NKEYS);

        // add

        for (vsize_t i = 0; i < NKEYS; ++i) {
            my_node_t *node = &g_nodes[g_perm_add[i]];
            ASSERT(!vrbtree_get(&tree, node->key));
            ASSERT(vrbtree_add(&tree, node->key, &node->embed));
            ASSERT(vrbtree_get(&tree, node->key) == &node->embed);
            ASSERT(vrbtree_size(&tree) == i + 1);
        }

        for (vsize_t i = 0; i < NKEYS; ++i) {
            ASSERT(vrbtree_get(&tree, g_nodes[i].key) == &g_nodes[i].embed);
        }

        vrbtree_node_t *res_first = vrbtree_first(&tree);
        vrbtree_node_t *res_last  = vrbtree_last(&tree);
        ASSERT(V_CONTAINER_OF(res_first, my_node_t, embed)->key == 0);
        ASSERT(V_CONTAINER_OF(res_last, my_node_t, embed)->key ==
               2 * NKEYS - 2);
        ASSERT(vrbtree_predecessor(&tree, 0) == NULL);
        ASSERT(vrbtree_successor(&tree, 2 * NKEYS - 2) == NULL);

        for (vsize_t i = 0; i < NKEYS - 1; ++i) {
            vrbtree_node_t *res_pred = vrbtree_predecessor(&tree, 2 * i + 2);
            vrbtree_node_t *res_succ = vrbtree_successor(&tree, 2 * i);
            ASSERT(V_CONTAINER_OF(res_pred, my_node_t, embed)->key == 2 * i);
            ASSERT(V_CONTAINER_OF(res_succ, my_node_t, embed)->key ==
                   2 * i + 2);

            vrbtree_node_t *res_pred_2 = vrbtree_predecessor(&tree, 2 * i + 1);
            vrbtree_node_t *res_succ_2 = vrbtree_successor(&tree, 2 * i + 1);
            ASSERT(V_CONTAINER_OF(res_pred_2, my_node_t, embed)->key == 2 * i);
            ASSERT(V_CONTAINER_OF(res_succ_2, my_node_t, embed)->key ==
                   2 * i + 2);
        }

        // rem

        for (vsize_t i = 0; i < NKEYS; ++i) {
            my_node_t *node = &g_nodes[g_perm_rem[i]];
            ASSERT(vrbtree_get(&tree, node->key) == &node->embed);
            ASSERT(vrbtree_remove(&tree, node->key));
            ASSERT(!vrbtree_get(&tree, node->key));
            ASSERT(vrbtree_size(&tree) == NKEYS - 1 - i);
        }

        for (vsize_t i = 0; i < NKEYS; ++i) {
            ASSERT(!vrbtree_get(&tree, g_nodes[i].key));
        }
    }

    vrbtree_destroy(&tree);

    return 0;
}
