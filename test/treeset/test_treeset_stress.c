/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <test/map/itreeset.h>
#include <test/map/treeset_test_interface.h>

#include <test/thread_launcher.h>

#ifndef NTHREADS
    #error "Choose number of threads by setting NTHREADS"
#endif

#define NKEYS  200000
#define NITERS 400

// Each thread gets a pool of keys (consecutive block in g_keys array)
// and it will be adding/removing them in patterns, chosen randomly and
// independently from other threads

#define BLOCK_LEN (NKEYS / NTHREADS)

treeset_key_t *g_keys;
vbool_t *g_exist;

enum {
    ITER_ONLY_ADDING,
    ITER_RANDOM_ADDING,
    ITER_ONLY_REMOVING,
    ITER_RANDOM_REMOVING,
    ITER_ONLY_SEARCHING,
    ITER_RANDOM_SEARCHING,
    ITER_ONLY_FLIPPING,
    ITER_RANDOM_FLIPPING,
    NITER_TYPES
};

void
init(void)
{
    g_keys  = malloc(BLOCK_LEN * NTHREADS * sizeof(treeset_key_t));
    g_exist = malloc(BLOCK_LEN * NTHREADS * sizeof(vbool_t));
    for (vsize_t i = 0; i < BLOCK_LEN * NTHREADS; ++i) {
        g_keys[i]  = i;
        g_exist[i] = false;
    }

    // Random shuffle using Fisher-Yates algorithm
    for (vsize_t i = 1; i < BLOCK_LEN * NTHREADS; ++i) {
        vsize_t index      = rand() % i;
        treeset_key_t temp = g_keys[i];
        g_keys[i]          = g_keys[index];
        g_keys[index]      = temp;
    }
}

void *
run(void *arg)
{
    vsize_t tid       = (vsize_t)(vuintptr_t)arg;
    unsigned int seed = tid;

    vsize_t begin = tid * BLOCK_LEN;
    vsize_t end   = (tid + 1) * BLOCK_LEN;

    for (vsize_t iter = 0; iter < NITERS; ++iter) {
        int iter_type  = rand_r(&seed) % NITER_TYPES;
        vbool_t norand = false;

        switch (iter_type) {
            case ITER_ONLY_ADDING:
            case ITER_ONLY_REMOVING:
            case ITER_ONLY_SEARCHING:
            case ITER_ONLY_FLIPPING:
                norand = true;
                break;
        }

        switch (iter_type) {
            case ITER_ONLY_ADDING:
            case ITER_RANDOM_ADDING:
                for (vsize_t i = begin; i < end; ++i) {
                    if (norand || rand_r(&seed) % 2) {
                        vbool_t res = tr_add(g_keys[i]);
                        ASSERT(res != g_exist[i]);
                        g_exist[i] = true;
                    }
                }
                break;
            case ITER_ONLY_REMOVING:
            case ITER_RANDOM_REMOVING:
                for (vsize_t i = begin; i < end; ++i) {
                    if (norand || rand_r(&seed) % 2) {
                        vbool_t res = tr_rem(g_keys[i]);
                        ASSERT(res == g_exist[i]);
                        g_exist[i] = false;
                    }
                }
                break;
            case ITER_ONLY_SEARCHING:
            case ITER_RANDOM_SEARCHING:
                for (vsize_t i = begin; i < end; ++i) {
                    if (norand || rand_r(&seed) % 2) {
                        vbool_t res = tr_con(g_keys[i]);
                        ASSERT(res == g_exist[i]);
                    }
                }
                break;
            case ITER_ONLY_FLIPPING:
            case ITER_RANDOM_FLIPPING:
                for (vsize_t i = begin; i < end; ++i) {
                    if (norand || rand_r(&seed) % 2) {
                        if (g_exist[i]) {
                            ASSERT(tr_rem(g_keys[i]));
                            g_exist[i] = false;
                        } else {
                            ASSERT(tr_add(g_keys[i]));
                            g_exist[i] = true;
                        }
                    }
                }
                break;
            default:
                ASSERT(0);
        }

        int count = 0;
        for (vsize_t i = begin; i < end; ++i) {
            count += g_exist[i];
        }
        printf("Thr %zu, iter %zu, type %d, count %d\n", tid, iter, iter_type,
               count);
    }

    return NULL;
}

void
destroy(void)
{
    free(g_keys);
    free(g_exist);
}

int
main(void)
{
    tr_init();

    init();
    launch_threads(NTHREADS, run);
    destroy();

    tr_verify();

    tr_destroy();
}
