/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#include <vsync/stack/elimination_stack.h>
#include <vsync/smr/gdump.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define IT 10000
#define N  24

typedef struct data_s {
    vstack_node_t stack_node;
    smr_node_t smr_node;
    vsize_t id;
} data_t;

gdump_t g_gdump;
vstack_t g_stack;
pthread_rwlock_t g_lock;
vatomic8_t g_stop = VATOMIC_INIT(0);

static inline void
thread_rw_read_acq(void *arg)
{
    pthread_rwlock_t *lock = (pthread_rwlock_t *)arg;
    int ret                = pthread_rwlock_rdlock(lock);
    assert(ret == 0);
}

static inline void
thread_rw_read_rel(void *arg)
{
    pthread_rwlock_t *lock = (pthread_rwlock_t *)arg;
    int ret                = pthread_rwlock_unlock(lock);
    assert(ret == 0);
}
static inline void
thread_rw_write_acq(void *arg)
{
    pthread_rwlock_t *lock = (pthread_rwlock_t *)arg;
    int ret                = pthread_rwlock_wrlock(lock);
    assert(ret == 0);
}
static inline void
thread_rw_write_rel(void *arg)
{
    pthread_rwlock_t *lock = (pthread_rwlock_t *)arg;
    int ret                = pthread_rwlock_unlock(lock);
    assert(ret == 0);
}

smr_rwlock_lib_t g_rwlock_lib = {thread_rw_read_acq, thread_rw_read_rel,
                                 thread_rw_write_acq, thread_rw_write_rel,
                                 &g_lock};

void
free_cb(smr_node_t *node, void *args)
{
    data_t *data = V_CONTAINER_OF(node, data_t, smr_node);
    free(data);
    (void)args;
}

void
destroy_cb(vstack_node_t *node, void *args)
{
    data_t *data = V_CONTAINER_OF(node, data_t, stack_node);
    free(data);
    (void)args;
}

int
yield_cb(void *args)
{
    (void)args;
    return sched_yield();
}

int
usleep_cb(vuint32_t microsecond)
{
    return usleep(microsecond);
}

static __thread unsigned int g_thread_seed = 0;
vuint32_t
rand_cb(vuint32_t min, vuint32_t max)
{
    if (g_thread_seed == 0) {
        g_thread_seed = time(NULL);
    }
    int r = rand_r(&g_thread_seed);
    if (r < 0) {
        r *= -1;
    }
    return (((vuint32_t)r) % (max - min + 1)) + min;
}

void
reclaim(void)
{
    while (vatomic8_read(&g_stop) == 0) {
        vsize_t count = gdump_recycle(&g_gdump, yield_cb, NULL, 1);
        if (count > 0) {
            printf("%zu node(s) were reclaimed\n", count);
        }
    }
}

void
consume(vsize_t tid)
{
    gdump_thread_t thread = {0};
    vstack_node_t *node   = NULL;
    data_t *data          = NULL;

    gdump_register(&g_gdump, &thread);

    for (vsize_t i = 0; i < IT; i++) {
        gdump_enter(&g_gdump, &thread);
        node = vstack_pop(&g_stack);
        if (node) {
            data = V_CONTAINER_OF(node, data_t, stack_node);
            printf("[T%zu] popped %zu\n", tid, data->id);
        }
        gdump_exit(&g_gdump, &thread);
        if (node) {
            gdump_retire(&g_gdump, &data->smr_node, free_cb, NULL);
        }
    }

    gdump_deregister(&g_gdump, &thread);
}

void
produce(vsize_t tid)
{
    data_t *data = NULL;
    for (vsize_t i = 0; i < IT; i++) {
        data = malloc(sizeof(data_t));
        if (data) {
            data->id = i;
            printf("[T%zu] pushing %zu\n", tid, data->id);
            vstack_push(&g_stack, &data->stack_node);
        }
    }
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;
    if (tid == 0) {
        reclaim();
    } else if (tid % 2 == 0) {
        produce(tid);
    } else {
        consume(tid);
    }
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    int ret = pthread_rwlock_init(&g_lock, NULL);
    assert(ret == 0);

    gdump_init(&g_gdump, g_rwlock_lib);
    vstack_init(&g_stack, rand_cb);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 1; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    // send the signal to stop reclaiming
    vatomic8_write(&g_stop, 1);
    pthread_join(threads[0], NULL);

    vstack_destroy(&g_stack, destroy_cb, NULL);
    gdump_destroy(&g_gdump);

    ret = pthread_rwlock_destroy(&g_lock);
    assert(ret == 0);
    return 0;
}
