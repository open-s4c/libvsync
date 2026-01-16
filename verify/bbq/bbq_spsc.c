/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2020-2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <pthread.h>
#include <stdlib.h>
#include <vsync/vtypes.h>
#include <vsync/common/assert.h>
#include <test/bbq/bbq_test_helper.h>

/* small enough such that producer may block with queue full */
#define BBQ_BLOCK_NUM_LOG 1U
#define BUFFER_ENTRY_NUM  2

/* large enough such that blocks are reused at least once */
#define NUM 5UL

#include <vsync/queue/bbq_spsc.h>
#include <stdio.h>
struct bbq_spsc_s *q;
uintptr_t total_enq = 0;
uintptr_t total_deq = 0;

static void *
writer(void *arg)
{
    V_UNUSED(arg);
    for (vuintptr_t i = 0; i < NUM; i++) {
        if (bbq_spsc_enqueue(q, &total_enq, 1, true)) {
            total_enq++;
        }
    }
    return NULL;
}

static void *
reader(void *arg)
{
    V_UNUSED(arg);
    vuintptr_t buf;
    for (vuintptr_t i = 0; i < NUM; i++) {
        if (bbq_spsc_dequeue(q, &buf, 1, true)) {
            ASSERT(buf == total_deq);
            total_deq++;
        }
    }
    return NULL;
}

#define SIZE                                                                   \
    ((sizeof(struct bbq_spsc_s)) +                                             \
     (BUFFER_ENTRY_NUM *                                                       \
      (sizeof(struct bbq_spsc_block_s) + sizeof(vuintptr_t))))

int
main(void)
{
    ASSERT((SIZE) <= 1024);

    vsize_t sz = bbq_spsc_memsize(BUFFER_ENTRY_NUM);
    q          = (struct bbq_spsc_s *)malloc(sz);
    ASSERT(q != NULL && "failed to create the ring buffer");
    BBQ_VERIFY_BLK_COUNT(BUFFER_ENTRY_NUM);
    vbool_t success = bbq_spsc_init(q, sz);
    ASSERT(success);
    ASSERT(BBQ_SPSC_COUNT(q) == 0);

    pthread_t t1, t2;

    pthread_create(&t1, NULL, writer, NULL);
    pthread_create(&t2, NULL, reader, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    ASSERT(BBQ_SPSC_COUNT(q) == total_enq - total_deq);

    free(q);

    return 0;
}
