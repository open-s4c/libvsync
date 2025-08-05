/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#include <vsync/queue/unbounded_queue_lf.h>
// maximum number of participating threads must be known at compile time
#define SMR_MAX_NTHREADS 12
#include <vsync/smr/cleanup.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define THRESHOLD_MICRO_SEC 1000
#define IT                  1000

typedef struct data_s {
    vsize_t id;
} data_t;

cleanup_t g_cleanup;
vqueue_ub_t g_queue;
pthread_mutex_t g_lock;

static inline void
lock_acq(void *arg)
{
    pthread_mutex_t *lock = (pthread_mutex_t *)arg;
    int ret               = pthread_mutex_lock(lock);
    assert(ret == 0);
}

static inline void
lock_rel(void *arg)
{
    pthread_mutex_t *lock = (pthread_mutex_t *)arg;
    int ret               = pthread_mutex_unlock(lock);
    assert(ret == 0);
}

smr_lock_lib_t g_lock_lib = {lock_acq, lock_rel, &g_lock};

void
free_cb(smr_node_t *node, void *args)
{
    vqueue_ub_node_t *qnode = V_CONTAINER_OF(node, vqueue_ub_node_t, smr_node);
    free(qnode);
    (void)args;
}

void
retire_cb(vqueue_ub_node_t *qnode, void *args)
{
    cleanup_add_to_delete(&g_cleanup, &qnode->smr_node, free_cb, NULL);
    (void)args;
}

void
destroy_cb(vqueue_ub_node_t *qnode, void *args)
{
    free(qnode);
    (void)args;
}

void *
run(void *args)
{
    data_t *data = NULL;
    vsize_t tid  = (vsize_t)args;

    cleanup_register_thread(&g_cleanup);

    for (vsize_t i = 0; i < IT; i++) {
        cleanup_set_signal(&g_cleanup);

        data = vqueue_ub_deq(&g_queue, retire_cb, NULL);
        if (data == NULL) {
            data                    = malloc(sizeof(data_t));
            data->id                = i;
            vqueue_ub_node_t *qnode = malloc(sizeof(vqueue_ub_node_t));
            printf("[T%zu] enq %zu\n", tid, data->id);
            vqueue_ub_enq(&g_queue, qnode, data);
        } else {
            printf("[T%zu] deq %zu\n", tid, data->id);
            free(data);
        }
    }

    cleanup_deregister_thread(&g_cleanup);

    return NULL;
}

int
main(void)
{
    pthread_t threads[SMR_MAX_NTHREADS];
    data_t *data = NULL;

    int ret = pthread_mutex_init(&g_lock, NULL);
    assert(ret == 0);

    /* When NULL is passed as a third parameter the cleaner thread is created
     * with pthread */
    cleanup_init(&g_cleanup, THRESHOLD_MICRO_SEC, NULL, &g_lock_lib);

    vqueue_ub_init(&g_queue);

    for (vsize_t i = 0; i < SMR_MAX_NTHREADS; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < SMR_MAX_NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /* dequeue all remaining nodes, to be able to destroy data */
    while (data = vqueue_ub_deq(&g_queue, destroy_cb, NULL), data) {
        free(data);
    }

    /* destroy the queue to destroy the remaining sentinel */
    vqueue_ub_destroy(&g_queue, destroy_cb, NULL);

    cleanup_destroy(&g_cleanup);

    ret = pthread_mutex_destroy(&g_lock);
    assert(ret == 0);
    return 0;
}
