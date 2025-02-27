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

#ifdef VSYNC_VERIFICATION_QUICK
    #define WRITER_THREAD    1
    #define READER_THREAD    1
    #define ENQUEUE_BATCH    1
    #define DEQUEUE_BATCH    1
    #define BUFFER_ENTRY_NUM 1
#else
    #define WRITER_THREAD    2
    #define READER_THREAD    2
    #define ENQUEUE_BATCH    2
    #define DEQUEUE_BATCH    2
    #define BUFFER_ENTRY_NUM 3
#endif
#define LOOP_BOUND 2

struct cachedq_s *q;
uint64_t write_total[WRITER_THREAD]    = {0};
uint64_t read_total[READER_THREAD + 1] = {0};

void *
writer(void *data)
{
    int id = *(int *)data;
    vuint64_t buf[ENQUEUE_BATCH];
    for (int i = 0; i < LOOP_BOUND * READER_THREAD / WRITER_THREAD; i++) {
        for (int j = 0; j < ENQUEUE_BATCH; j++) {
            vuint64_t offset =
                id * LOOP_BOUND * ENQUEUE_BATCH + i * ENQUEUE_BATCH + j;
            buf[j] = 1UL << offset;
        }
        vuint64_t num = cachedq_enqueue(q, buf, ENQUEUE_BATCH);
        for (vuint64_t k = 0; k < num; k++) {
            write_total[id] += buf[k];
        }
    }
    free(data);
    return NULL;
}

void *
reader(void *data)
{
    int id = *(int *)data;
    for (int i = 0; i < LOOP_BOUND * WRITER_THREAD / READER_THREAD; i++) {
        vuint64_t buf[DEQUEUE_BATCH] = {0};
        vuint64_t num                = cachedq_dequeue(q, buf, DEQUEUE_BATCH);
        for (vuint64_t k = 0; k < num; k++) {
            read_total[id] += buf[k];
        }
    }
    free(data);
    return NULL;
}

int
main(void)
{
    // Sanity check!
    ASSERT(LOOP_BOUND * DEQUEUE_BATCH * WRITER_THREAD / READER_THREAD >=
           BUFFER_ENTRY_NUM);

    vsize_t buf_size = cachedq_memsize(BUFFER_ENTRY_NUM);
    void *buf        = malloc(buf_size);
    q                = cachedq_init(buf, buf_size);
    if (q == NULL)
        exit(1);

    int TW = WRITER_THREAD;
    pthread_t thread_w[TW];
    for (int i = 0; i < TW; i++) {
        int *arg = (int *)malloc(sizeof(int));
        *arg     = i;
        pthread_create(&thread_w[i], NULL, writer, (void *)arg);
    }

    int TR = READER_THREAD;
    pthread_t thread_r[TR + 1];
    for (int i = 0; i < TR; i++) {
        int *arg = (int *)malloc(sizeof(int));
        *arg     = i;
        pthread_create(&thread_r[i], NULL, reader, (void *)arg);
    }

    for (int i = 0; i < TW; i++)
        pthread_join(thread_w[i], NULL);
    for (int i = 0; i < TR; i++)
        pthread_join(thread_r[i], NULL);

    vuint64_t write_total_sum = 0;
    for (int i = 0; i < WRITER_THREAD; i++) {
        write_total_sum += write_total[i];
    }

    {
        int *arg = (int *)malloc(sizeof(int));
        *arg     = TR;
        pthread_create(&thread_r[TR], NULL, reader, (void *)arg);
        pthread_join(thread_r[TR], NULL);
    }

    vuint64_t read_total_sum = 0;
    for (int i = 0; i < READER_THREAD + 1; i++) {
        read_total_sum += read_total[i];
    }

    ASSERT(write_total_sum == read_total_sum);
    ASSERT(cachedq_count(q) == 0);
    free(buf);
    return 0;
}
