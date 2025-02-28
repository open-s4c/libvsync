/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <vsync/pool/cached_pool.h>
#include <vsync/common/assert.h>
#include <vsync/common/dbg.h>
#include <sys/time.h>
#include <stdlib.h>

#define CACHEDP_NUM_ENTRIES 4096U
#define CACHEDP_MAX_THREAD  32U
#define CACHEDP_ENTRY_SIZE  8U

uint8_t buf[cached_pool_memsize(CACHEDP_MAX_THREAD, CACHEDP_NUM_ENTRIES,
                                CACHEDP_ENTRY_SIZE)];

double
now_ms()
{
    struct timeval now;
    int ret;

    ret = gettimeofday(&now, NULL);
    if (ret != 0) {
        DBG_RED("Error: could not get time of day\n");
        exit(-1);
    }
    return (double)now.tv_sec * 1000.0 + (double)now.tv_usec / 1000.0;
}

void
run_cached_pool()
{
    cached_pool_t *t = cached_pool_init(
        buf, CACHEDP_MAX_THREAD, CACHEDP_NUM_ENTRIES, CACHEDP_ENTRY_SIZE);
    void *data[CACHEDP_NUM_ENTRIES];
    vuint64_t ITERS = 100000;
    double start_ms = now_ms();
    for (vuint64_t i = 0; i < ITERS; i++) {
        for (vuint64_t j = 0; j < CACHEDP_NUM_ENTRIES; j++) {
            void *p = cached_pool_alloc(t, 0);
            ASSERT(p);
            data[j] = p;
        }
        for (vuint64_t j = 0; j < CACHEDP_NUM_ENTRIES; j++) {
            cached_pool_free(t, 0, data[j]);
        }
    }
    double elapsed_ms = now_ms() - start_ms;
    printf("cached_pool throughput: %.3lf iter/s\n",
           1000.0 * ITERS * CACHEDP_NUM_ENTRIES * 2 / elapsed_ms);
}

void
run_baseline()
{
    void *data[CACHEDP_NUM_ENTRIES];
    vuint64_t ITERS = 100000;
    double start_ms = now_ms();
    for (vuint64_t i = 0; i < ITERS; i++) {
        for (vuint64_t j = 0; j < CACHEDP_NUM_ENTRIES; j++) {
            void *p = malloc(8);
            ASSERT(p);
            data[j] = p;
        }
        for (vuint64_t j = 0; j < CACHEDP_NUM_ENTRIES; j++) {
            free(data[j]);
        }
    }
    double elapsed_ms = now_ms() - start_ms;
    printf("baseline throughput: %.3lf iter/s\n",
           1000.0 * ITERS * CACHEDP_NUM_ENTRIES * 2 / elapsed_ms);
}

int
main(void)
{
    run_cached_pool();
    run_baseline();
}
