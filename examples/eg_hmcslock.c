/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#include <vsync/spinlock/hmcslock.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>

// To build such a hierarchical tree:               //
//       R                                          //
//     /   \                                        //
//    A     B                                       //
//   / \   / \                                      //
//  C   D  E  F                                     //
//                                                  //
// The following specs are needed:                  //
// level 1, has a root (1 node)                     //
// level 2, has two nodes connected to R the parent //
// level 3, has two nodes per parent (A/B)          //
//                                                  //
// number of levels = 3                             //
// number of locks  = 7                             //

#define NUM_LEVELS 3

#define LEVEL_1 1
#define LEVEL_2 2
#define LEVEL_3 2

#define LEVEL_1_THRESHOLD 1
#define LEVEL_2_THRESHOLD 1
#define LEVEL_3_THRESHOLD 1

hmcslock_level_spec_t g_level_specs[NUM_LEVELS] = {
    {LEVEL_1, LEVEL_1_THRESHOLD},
    {LEVEL_2, LEVEL_2_THRESHOLD},
    {LEVEL_3, LEVEL_3_THRESHOLD},
};

#define NUM_LOCKS                                                              \
    ((LEVEL_3 * LEVEL_2 * LEVEL_1) + (LEVEL_1 * LEVEL_2) + (LEVEL_1))

#define N            10
#define IT           10
#define EXPECTED_VAL (N) * (IT)

vuint32_t g_x = 0;
vuint32_t g_y = 0;

hmcslock_t g_locks[NUM_LOCKS];
hmcs_node_t g_qnode[N] = {0};

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;

    /* the thread should be pinned to the core */

    /* find the leaf lock associated with this code */
    hmcslock_t *lock = hmcslock_which_lock(g_locks, NUM_LOCKS, g_level_specs,
                                           NUM_LEVELS, LEVEL_3, tid);

    for (vsize_t i = 0; i < IT; i++) {
        hmcslock_acquire(lock, &g_qnode[tid], NUM_LEVELS);
        g_x++;
        g_y++;
        assert(g_x == g_y);
        hmcslock_release(lock, &g_qnode[tid], NUM_LEVELS);
    }

    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    hmcslock_init(g_locks, NUM_LOCKS, g_level_specs, NUM_LEVELS);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final value %u %u\n", g_x, g_y);
    assert(g_x == EXPECTED_VAL);
    assert(g_x == g_y);
    return 0;
}
