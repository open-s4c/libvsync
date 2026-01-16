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

#define NUM              200000000UL
#define BUFFER_ENTRY_NUM 4096
#define ENQUEUE_BATCH    5UL
#define DEQUEUE_BATCH    4UL

#include <vsync/queue/bbq_spsc.h>

bbq_spsc_t *rb;

void *
writer(void *arg)
{
    V_UNUSED(arg);
    vuintptr_t buf[ENQUEUE_BATCH] = {0};
    vuint64_t ptr                 = 0;
    vuint64_t rest                = NUM;
    while (rest) {
        vuint32_t count = VMIN(rest, ENQUEUE_BATCH);
        ASSERT(count <= ENQUEUE_BATCH);
        for (vuint32_t i = 0; i < count; i++) {
            buf[i] = ptr++;
        }
        rest -= bbq_spsc_enqueue(rb, buf, count, true);
    }
    return NULL;
}

void *
reader(void *arg)
{
    V_UNUSED(arg);
    vuintptr_t buf[DEQUEUE_BATCH] = {0};
    vuint64_t ptr                 = 0;
    vuint64_t rest                = NUM;
    while (rest) {
        vuint32_t count = bbq_spsc_dequeue(rb, buf, DEQUEUE_BATCH, false);
        for (vuint32_t i = 0; i < count; i++) {
            ASSERT(buf[i] == ptr++);
        }
        rest -= count;
    }
    return NULL;
}

int
main(void)
{
    vuint32_t sz = bbq_spsc_memsize(BUFFER_ENTRY_NUM);
    rb           = (struct bbq_spsc_s *)malloc(sz);
    if (rb == NULL) {
        perror("fail to create the ring buffer");
        abort();
    }
    BBQ_VERIFY_BLK_COUNT(BUFFER_ENTRY_NUM);

    vbool_t success = bbq_spsc_init(rb, sz);
    ASSERT(success);
    ASSERT(BBQ_SPSC_COUNT(rb) == 0);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, writer, NULL);
    pthread_create(&t2, NULL, reader, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    ASSERT(BBQ_SPSC_COUNT(rb) == 0);
    free(rb);
    return 0;
}
