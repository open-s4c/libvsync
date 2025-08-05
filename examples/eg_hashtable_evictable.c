/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/smr/gdump.h>
#include <vsync/map/hashtable_evictable.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define N         12
#define ITER      10
#define THRESHOLD 2

typedef struct user_key_s {
    vuint32_t x;
    vuint32_t y;
    vuint16_t z;
} user_key_t;

typedef struct entry_s {
    vhashtable_entry_t ht_entry;
    user_key_t key;
    smr_node_t smr_node;
} entry_t;

#define PRINT printf

pthread_rwlock_t g_lock;

vhashtable_t g_hash_tbl;

gdump_t g_gdump;

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
free_callback(smr_node_t *node, void *arg)
{
    entry_t *entry = V_CONTAINER_OF(node, entry_t, smr_node);
    free(entry);
    V_UNUSED(arg);
}

void
retire_callback(vhashtable_entry_t *node, void *arg)
{
    entry_t *entry = V_CONTAINER_OF(node, entry_t, ht_entry);
    gdump_retire(&g_gdump, &entry->smr_node, free_callback, NULL);
    V_UNUSED(arg);
}

int
cmp_callback(vhashtable_entry_t *node, vhashtable_key_t k)
{
    user_key_t given_key = (*(user_key_t *)k);
    entry_t *entry       = V_CONTAINER_OF(node, entry_t, ht_entry);
    user_key_t entry_key = entry->key;

    if (entry_key.x != given_key.x) {
        if (entry_key.x < given_key.x)
            return -1;
        return 1;
    } else if (entry_key.y != given_key.y) {
        if (entry_key.y < given_key.y)
            return -1;
        return 1;
    } else if (entry_key.z != given_key.z) {
        if (entry_key.z < given_key.z)
            return -1;
        return 1;
    } else {
        ASSERT(entry_key.z == given_key.z);
        ASSERT(entry_key.x == given_key.x);
        ASSERT(entry_key.y == given_key.y);
        return 0;
    }
}

vsize_t
hashit(user_key_t key)
{
    return (key.x ^ (key.y << 2) ^ (key.z << 4)) &
           (VHASHTABLE_BUCKET_COUNT - 1);
}

int
yield_cb(void *args)
{
    (void)args;
    return sched_yield();
}

void
producer(vsize_t tid)
{
    gdump_thread_t thread;
    entry_t *entry  = NULL;
    vbool_t success = false;

    gdump_register(&g_gdump, &thread);

    for (vuint16_t i = 0; i < ITER; i++) {
        user_key_t user_key  = {.x = i, .z = i, .y = i};
        vhashtable_key_t key = (vhashtable_key_t)&user_key;
        vsize_t idx          = hashit(user_key);

        gdump_enter(&g_gdump, &thread);

        vhashtable_entry_t *node = vhashtable_get(&g_hash_tbl, key, idx);

        if (node == NULL) {
            entry      = malloc(sizeof(entry_t));
            entry->key = user_key;
            success =
                vhashtable_insert(&g_hash_tbl, key, &entry->ht_entry, idx);

            if (success) {
                printf("T%zu added entry with key {%u, %u, %u} \n", tid,
                       user_key.x, user_key.z, user_key.y);
            } else {
                printf("T%zu key {%u, %u, %u} already exist \n", tid,
                       user_key.x, user_key.z, user_key.y);
                free(entry);
            }
        }

        gdump_exit(&g_gdump, &thread);

        vuint64_t count = vhashtable_get_entries_count(&g_hash_tbl);
        if (count > THRESHOLD) {
            gdump_enter(&g_gdump, &thread);
            vuint64_t evicted =
                vhashtable_evict(&g_hash_tbl, count - THRESHOLD);
            printf("T%zu Evicted %lu\n", tid, evicted);
            gdump_exit(&g_gdump, &thread);
        }
    }

    gdump_deregister(&g_gdump, &thread);
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

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;
    if (tid == 0) {
        reclaim();
    } else {
        producer(tid);
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
    vhashtable_init(&g_hash_tbl, retire_callback, NULL, cmp_callback);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 1; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    vatomic8_write(&g_stop, 1);
    pthread_join(threads[0], NULL);

    vhashtable_destroy(&g_hash_tbl);
    gdump_destroy(&g_gdump);

    ret = pthread_rwlock_destroy(&g_lock);
    assert(ret == 0);
    return 0;
}
