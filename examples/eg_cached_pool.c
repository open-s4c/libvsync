/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/vtypes.h>
#include <vsync/common/assert.h>
#include <vsync/pool/cached_pool.h>
#include <pthread.h>
#include <stdlib.h>

#define CACHEDP_NUM_ENTRIES 4096U
#define CACHEDP_MAX_THREAD  32U

cached_pool_t *g_pool;
void *g_buff;

typedef struct entry_s {
    vuint32_t x;
    vuint32_t y;
} entry_t;

#define CACHEDP_ENTRY_SIZE sizeof(entry_t)

void
init(void)
{
    vsize_t size = cached_pool_memsize(CACHEDP_MAX_THREAD, CACHEDP_NUM_ENTRIES,
                                       CACHEDP_ENTRY_SIZE);
    g_buff       = malloc(size);
    g_pool = cached_pool_init(g_buff, CACHEDP_MAX_THREAD, CACHEDP_NUM_ENTRIES,
                              CACHEDP_ENTRY_SIZE);
}

void
deinit(void)
{
    free(g_pool);
}

void *
run(void *arg)
{
    vuint64_t tid  = (vuint64_t)(arg);
    entry_t *entry = cached_pool_alloc(g_pool, tid);
    entry->x       = 123;
    entry->y       = 456;
    cached_pool_free(g_pool, tid, entry);
    return NULL;
}

int
main(void)
{
    init();
    pthread_t threads[CACHEDP_MAX_THREAD];
    for (vuint64_t i = 0; i < CACHEDP_MAX_THREAD; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }
    for (vuint64_t i = 0; i < CACHEDP_MAX_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }
    deinit();
}
