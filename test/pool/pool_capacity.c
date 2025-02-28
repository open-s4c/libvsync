/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <vsync/pool/cached_pool.h>
#include <vsync/common/assert.h>

#define CACHEDP_NUM_ENTRIES 4096U
#define CACHEDP_MAX_THREAD  32U
#define CACHEDP_ENTRY_SIZE  8U

uint8_t buf[cached_pool_memsize(CACHEDP_MAX_THREAD, CACHEDP_NUM_ENTRIES,
                                CACHEDP_ENTRY_SIZE)];

int
main(void)
{
    cached_pool_t *t = cached_pool_init(
        buf, CACHEDP_MAX_THREAD, CACHEDP_NUM_ENTRIES, CACHEDP_ENTRY_SIZE);

    vuint32_t len = (CACHEDP_MAX_THREAD + 1) * t->conf.threshold_init;
    void *data_0[len];
    void *data_1[t->conf.threshold_init];
    void *data_2[len];

    for (vuint64_t i = 0; i < CACHEDP_MAX_THREAD + 1; i++) {
        for (vuint64_t j = 0; j < t->conf.threshold_init; j++) {
            void *p = cached_pool_alloc(t, 0);
            ASSERT(p);
            data_0[i * t->conf.threshold_init + j] = p;
        }
    }

    for (vuint64_t i = 0; i < len; i++) {
        for (vuint64_t j = 0; j < len; j++) {
            if (i != j) {
                ASSERT(data_0[i] != data_0[j]);
            }
        }
    }
    ASSERT(!cached_pool_alloc(t, 0));

    for (vuint64_t j = 0; j < t->conf.threshold_init; j++) {
        void *p = cached_pool_alloc(t, 1);
        ASSERT(p);
        data_1[j] = p;
    }
    ASSERT(!cached_pool_alloc(t, 1));

    for (vuint64_t i = 0; i < CACHEDP_MAX_THREAD + 1; i++) {
        for (vuint64_t j = 0; j < t->conf.threshold_init; j++) {
            cached_pool_free(t, 0, data_0[i * t->conf.threshold_init + j]);
        }
    }

    for (vuint64_t i = 0; i < CACHEDP_MAX_THREAD + 1; i++) {
        for (vuint64_t j = 0; j < t->conf.threshold_init; j++) {
            void *p = cached_pool_alloc(t, 2);
            ASSERT(p);
            data_2[i * t->conf.threshold_init + j] = p;
        }
    }

    V_UNUSED(data_1);
    V_UNUSED(data_2);
}
