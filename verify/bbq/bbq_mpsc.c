/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2020-2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <vsync/common/assert.h>
#include <time.h>
#include <vsync/vtypes.h>
#include <vsync/common/assert.h>
#include <test/bbq/bbq_test_helper.h>

/* small enough such that producer may block with queue full */
#define BBQ_BLOCK_NUM_LOG 1U
#define BUFFER_ENTRY_NUM  2
#define NUM_WRITER        2
#define NUM_READER        1

#include <vsync/queue/bbq_mpmc.h>

struct bbq_mpmc_s *q;

static void *
writer(void *arg)
{
    vuint64_t *id   = (vuint64_t *)arg;
    vuint64_t count = (*id) + 1;
    vuintptr_t ptr  = 1;
    while (count) {
        vuint32_t r = bbq_mpmc_enqueue(q, &ptr, 1, true);
        if (r) {
            count--;
            ptr++;
        }
    }
    free(arg);
    return NULL;
}

static void *
reader(void *arg)
{
    vuint64_t count = 0;
    vuintptr_t exp  = 1;

    while (count < 1) {
        vuintptr_t ptr;
        vuint32_t r = bbq_mpmc_dequeue(q, &ptr, 1, true);
        if (r) {
            ASSERT(ptr == 1);
            count++;
            exp++;
        }
    }
    free(arg);
    return NULL;
}

#define SIZE                                                                   \
    ((sizeof(struct bbq_mpmc_s)) +                                             \
     (BUFFER_ENTRY_NUM *                                                       \
      (sizeof(struct bbq_mpmc_block_s) + sizeof(vuintptr_t))))

int
main(void)
{
    ASSERT((SIZE) <= 2048);

    vuint32_t sz = bbq_mpmc_memsize(BUFFER_ENTRY_NUM);
    q            = (struct bbq_mpmc_s *)malloc(sz);
    ASSERT(q != NULL && "failed to create the ring buffer");
    BBQ_VERIFY_BLK_COUNT(BUFFER_ENTRY_NUM);
    vbool_t success = bbq_mpmc_init(q, sz);
    ASSERT(success);
    ASSERT(BBQ_MPMC_COUNT(q) == 0);

    pthread_t t1[NUM_WRITER], t2[NUM_READER];

    for (int i = 0; i < NUM_WRITER; i++) {
        vuint64_t *arg = malloc(sizeof(*arg));
        *arg           = i;
        pthread_create(&t1[i], NULL, writer, arg);
    }

    for (int i = 0; i < NUM_READER; i++) {
        vuint64_t *arg = malloc(sizeof(*arg));
        *arg           = i;
        pthread_create(&t2[i], NULL, reader, arg);
    }

    for (int i = 0; i < NUM_WRITER; i++)
        pthread_join(t1[i], NULL);
    for (int i = 0; i < NUM_READER; i++)
        pthread_join(t2[i], NULL);

    free(q);

    return 0;
}
