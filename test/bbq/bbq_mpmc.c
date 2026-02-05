/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <vsync/vtypes.h>
#include <unistd.h>
#include <vsync/common/assert.h>
#include <time.h>

#define NUM        2000000UL
#define NUM_WRITER 4
#define NUM_READER 5

#define BUFFER_ENTRY_NUM 4096
#define ENQUEUE_BATCH    5UL
#define DEQUEUE_BATCH    4UL

#include <vsync/queue/bbq_mpmc.h>

struct bbq_mpmc_s *rb;

void *
writer(void *arg)
{
    vuint64_t *id                 = (vuint64_t *)arg;
    vuintptr_t buf[ENQUEUE_BATCH] = {0};
    vuint64_t ptr                 = 0;
    vuint64_t rest                = NUM * NUM_READER;
    while (rest) {
        vuint32_t count = VMIN(rest, ENQUEUE_BATCH);
        ASSERT(count <= ENQUEUE_BATCH);
        for (vuint32_t i = 0; i < count; i++) {
            buf[i] = ((*id) << 30) | (ptr++);
        }
        rest -= bbq_mpmc_enqueue(rb, buf, count, true);
    }
    free(arg);
    return NULL;
}

void *
reader(void *arg)
{
    V_UNUSED(arg);
    vuintptr_t buf[DEQUEUE_BATCH] = {0};
    vuint64_t last[NUM_WRITER]    = {0};
    vuint64_t rest                = NUM * NUM_WRITER;
    while (rest) {
        vuint32_t count = VMIN(rest, DEQUEUE_BATCH);
        count           = bbq_mpmc_dequeue(rb, buf, count, false);
        for (vuint32_t i = 0; i < count; i++) {
            vuint64_t id   = buf[i] >> 30;
            vuint64_t data = buf[i] & ((1ULL << 30) - 1);
            ASSERT(id < NUM_WRITER);
            ASSERT(last[id] <= data);
            last[id] = data;
        }
        rest -= count;
    }
    return NULL;
}

int
main(void)
{
    vsize_t sz = bbq_mpmc_memsize(BUFFER_ENTRY_NUM);
    rb         = (bbq_mpmc_t *)malloc(sz);
    if (rb == NULL) {
        perror("fail to create the ring buffer");
        abort();
    }
    BBQ_VERIFY_BLK_COUNT(BUFFER_ENTRY_NUM);
    vbool_t success = bbq_mpmc_init(rb, sz);
    ASSERT(success);
    ASSERT(BBQ_MPMC_COUNT(rb) == 0);

    pthread_t t1[NUM_WRITER], t2[NUM_READER];
    for (vsize_t i = 0; i < NUM_WRITER; i++) {
        vuint64_t *arg = malloc(sizeof(*arg));
        *arg           = i;
        pthread_create(&t1[i], NULL, writer, arg);
    }
    for (vsize_t i = 0; i < NUM_READER; i++) {
        pthread_create(&t2[i], NULL, reader, NULL);
    }

    for (vsize_t i = 0; i < NUM_WRITER; i++) {
        pthread_join(t1[i], NULL);
    }
    for (vsize_t i = 0; i < NUM_READER; i++) {
        pthread_join(t2[i], NULL);
    }

    ASSERT(BBQ_MPMC_COUNT(rb) == 0);
    free(rb);
    return 0;
}
