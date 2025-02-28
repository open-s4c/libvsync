/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
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
#include <vsync/queue/cachedq.h>
#include <vsync/utils/math.h>

#define NUM              1000UL
#define WRITER_THREAD    3UL
#define READER_THREAD    4UL
#define BUFFER_ENTRY_NUM 10000UL
#define ENQUEUE_BATCH    4UL
#define DEQUEUE_BATCH    3UL
#define DATA             12345UL

struct cachedq_s *q;

void *
writer(void *data)
{
    ASSERT(data == NULL);
    vuint64_t buf[ENQUEUE_BATCH];
    for (vsize_t i = 0; i < ENQUEUE_BATCH; i++) {
        buf[i] = DATA;
    }
    vuint64_t rest = READER_THREAD * NUM;
    while (rest) {
        vuint64_t count = VMIN(rest, ENQUEUE_BATCH);
        rest -= cachedq_enqueue(q, buf, count);
    }
    return NULL;
}

void *
reader(void *data)
{
    ASSERT(data == NULL);
    vuint64_t loop               = 0;
    vuint64_t buf[DEQUEUE_BATCH] = {0};

    while (loop < WRITER_THREAD * NUM) {
        vuint64_t count = VMIN(WRITER_THREAD * NUM - loop, DEQUEUE_BATCH);
        count           = cachedq_dequeue(q, buf, count);
        for (vsize_t i = 0; i < count; i++) {
            ASSERT(buf[i] == DATA);
        }
        loop += count;
    }
    return NULL;
}

int
main(void)
{
    pthread_t thread_w[WRITER_THREAD];
    pthread_t thread_r[READER_THREAD];

    vsize_t buf_size = cachedq_memsize(BUFFER_ENTRY_NUM);
    void *buf        = malloc(buf_size);

    q = cachedq_init(buf, buf_size);
    if (q == NULL) {
        perror("fail to create the cachedq");
    }

    for (vsize_t i = 0; i < WRITER_THREAD; i++) {
        pthread_create(&thread_w[i], NULL, writer, NULL);
    }
    for (vsize_t i = 0; i < READER_THREAD; i++) {
        pthread_create(&thread_r[i], NULL, reader, NULL);
    }
    for (vsize_t i = 0; i < WRITER_THREAD; i++) {
        pthread_join(thread_w[i], NULL);
    }
    for (vsize_t i = 0; i < READER_THREAD; i++) {
        pthread_join(thread_r[i], NULL);
    }
    vuint64_t total = WRITER_THREAD * READER_THREAD * NUM;

    ASSERT(vatomic64_read_rlx(&q->phead) == total);
    ASSERT(vatomic64_read_rlx(&q->ptail) == total);
    ASSERT(vatomic64_read_rlx(&q->chead) == total);
    ASSERT(vatomic64_read_rlx(&q->ctail) == total);

    return 0;
}
