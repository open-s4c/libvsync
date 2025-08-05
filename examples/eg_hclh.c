/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


/* you can use the default value or redefine it to indicate how many     \
         clusters you need */
#define HCLH_MAX_CLUSTERS 2
#include <vsync/spinlock/hclhlock.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>

#define N            10
#define IT           10
#define EXPECTED_VAL (N) * (IT)

vuint32_t g_x = 0;
vuint32_t g_y = 0;

hclh_lock_t g_lock;
hclh_qnode_t g_qnode[N] = {0};
hclh_tnode_t g_tnode[N] = {0};

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;

    /* this should be assigned to either numa group or cache group where the
     * thread is running */
    vuint32_t cluster = tid % HCLH_MAX_CLUSTERS;

    hclhlock_init_tnode(&g_tnode[tid], &g_qnode[tid], cluster);

    for (vsize_t i = 0; i < IT; i++) {
        hclhlock_acquire(&g_lock, &g_tnode[tid]);
        g_x++;
        g_y++;
        assert(g_x == g_y);
        hclhlock_release(&g_tnode[tid]);
    }

    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    hclhlock_init(&g_lock);

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
