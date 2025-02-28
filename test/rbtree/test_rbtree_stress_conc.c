/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

// Stress test of readers/writer concurrency in rbtree
//
// Performs NITERS iterations of:
// - writer adds NKEYS in random order
// - in parallel each of NREADERS readers do NCHECKS random checks:
//   - keep searching until the i-th added element appears in the tree
//   - assert that the (i-1)-th added element is in the tree
//
// - writer removes NKEYS in different random order
// - in parallel each of NREADERS readers do NCHECKS random checks:
//   - keep searching until the i-th removed element leaves the tree
//   - assert that the (i-1)-th removed element is not in the tree

#include <vsync/map/rbtree.h>
#include <test/rand.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

#define NKEYS    1000U
#define NITERS   1000U
#define NREADERS 2U
#define NCHECKS  10U

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

vrbtree_t g_tree;

my_node_t g_nodes[NKEYS];
vsize_t g_perm_add[NKEYS], g_perm_rem[NKEYS];

pthread_t g_readers[NREADERS];
vsize_t g_read_check[NREADERS][NCHECKS];
vatomicsz_t g_readers_await;


void *
reader_add(void *arg)
{
    vsize_t id = (vsize_t)arg;

    vatomicsz_sub(&g_readers_await, 1U);

    vsize_t cnt = 0;
    for (vsize_t j = 0; j < NCHECKS; ++j) {
        vsize_t idx = g_read_check[id][j];
        while (!vrbtree_get(&g_tree, g_nodes[g_perm_add[idx]].key)) {
            ++cnt;
        }
        ASSERT(idx == 0 ||
               vrbtree_get(&g_tree, g_nodes[g_perm_add[idx - 1]].key));
    }
    printf("Reader add %lu: %lu\n", id, cnt);

    return NULL;
}

void *
reader_rem(void *arg)
{
    vsize_t id = (vsize_t)arg;

    vatomicsz_sub(&g_readers_await, 1U);

    vsize_t cnt = 0;
    for (vsize_t j = 0; j < NCHECKS; ++j) {
        vsize_t idx = g_read_check[id][j];
        while (vrbtree_get(&g_tree, g_nodes[g_perm_rem[idx]].key)) {
            ++cnt;
        }
        ASSERT(idx == 0 ||
               !vrbtree_get(&g_tree, g_nodes[g_perm_rem[idx - 1]].key));
    }
    printf("Reader rem %lu: %lu\n", id, cnt);

    return NULL;
}

int
main(void)
{
    vrbtree_init(&g_tree, _retire_fun, NULL, _cmp_fun);

    for (vsize_t iter = 0; iter < NITERS; ++iter) {
        printf("Iter #%lu\n", iter);
        deterministic_random_init(iter);

        // init

        for (vsize_t i = 0; i < NKEYS; ++i) {
            g_nodes[i].key = i;
            g_perm_add[i]  = i;
            g_perm_rem[i]  = i;
        }
        deterministic_random_shuffle(g_perm_add, NKEYS);
        deterministic_random_shuffle(g_perm_rem, NKEYS);

        // add

        vatomicsz_init(&g_readers_await, NREADERS);
        for (vsize_t i = 0; i < NREADERS; ++i) {
            for (vsize_t j = 0; j < NCHECKS; ++j) {
                g_read_check[i][j] =
                    j * (NKEYS / NCHECKS) +
                    deterministic_random_next_int(0, NKEYS / NCHECKS - 1);
            }
            pthread_create(&g_readers[i], NULL, reader_add, (void *)i);
        }

        while (vatomicsz_read(&g_readers_await)) {}

        for (vsize_t i = 0; i < NKEYS; ++i) {
            my_node_t *node = &g_nodes[g_perm_add[i]];
            ASSERT(!vrbtree_get(&g_tree, node->key));
            ASSERT(vrbtree_add(&g_tree, node->key, &node->embed));
            ASSERT(vrbtree_get(&g_tree, node->key) == &node->embed);
        }

        for (vsize_t i = 0; i < NREADERS; ++i) {
            pthread_join(g_readers[i], NULL);
        }

        for (vsize_t i = 0; i < NKEYS; ++i) {
            ASSERT(vrbtree_get(&g_tree, g_nodes[i].key) == &g_nodes[i].embed);
        }

        ASSERT(vrbtree_size(&g_tree) == NKEYS);
        vrbtree_node_t *res_first = vrbtree_first(&g_tree);
        vrbtree_node_t *res_last  = vrbtree_last(&g_tree);
        ASSERT(V_CONTAINER_OF(res_first, my_node_t, embed)->key == 0);
        ASSERT(V_CONTAINER_OF(res_last, my_node_t, embed)->key == NKEYS - 1);

        // rem

        vatomicsz_init(&g_readers_await, NREADERS);
        for (vsize_t i = 0; i < NREADERS; ++i) {
            for (vsize_t j = 0; j < NCHECKS; ++j) {
                g_read_check[i][j] =
                    j * (NKEYS / NCHECKS) +
                    deterministic_random_next_int(0, NKEYS / NCHECKS - 1);
            }
            pthread_create(&g_readers[i], NULL, reader_rem, (void *)i);
        }

        while (vatomicsz_read(&g_readers_await)) {}

        for (vsize_t i = 0; i < NKEYS; ++i) {
            my_node_t *node = &g_nodes[g_perm_rem[i]];
            ASSERT(vrbtree_get(&g_tree, node->key) == &node->embed);
            ASSERT(vrbtree_remove(&g_tree, node->key));
            ASSERT(!vrbtree_get(&g_tree, node->key));
        }

        for (vsize_t i = 0; i < NREADERS; ++i) {
            pthread_join(g_readers[i], NULL);
        }

        for (vsize_t i = 0; i < NKEYS; ++i) {
            ASSERT(!vrbtree_get(&g_tree, g_nodes[i].key));
        }
    }

    vrbtree_destroy(&g_tree);

    return 0;
}
