#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <vsync/common/assert.h>
#include <vsync/queue/cachedq.h>

#define BUFFER_ENTRY_NUM 6UL
#define WRITER_THREAD    2UL
#define READER_THREAD    2UL
#define ENQUEUE_BATCH    4UL
#define DEQUEUE_BATCH    3UL
#define DATA             12345UL

cachedq_t *q;

void *
writer(void *id)
{
    vuint64_t buf[ENQUEUE_BATCH] = {0};
    for (vsize_t i = 0; i < ENQUEUE_BATCH; i++) {
        buf[i] = DATA;
    }
    vuint64_t count = cachedq_enqueue(q, buf, ENQUEUE_BATCH);
    printf(
        "Writer #%zu tries to write a batch with %ld items, %ld items are "
        "success.\n",
        (vsize_t)id, ENQUEUE_BATCH, count);
    return NULL;
}

void *
reader(void *id)
{
    vuint64_t buf[DEQUEUE_BATCH] = {0};
    vuint64_t count              = cachedq_dequeue(q, buf, DEQUEUE_BATCH);
    for (vsize_t i = 0; i < count; i++) {
        ASSERT(buf[i] == DATA);
    }
    printf(
        "Reader #%zu tries to read a batch with %ld items, %ld items are "
        "success.\n",
        (vsize_t)id, DEQUEUE_BATCH, count);
    return NULL;
}

int
main(void)
{
    vsize_t buf_size = cachedq_memsize(BUFFER_ENTRY_NUM);
    void *buf        = malloc(buf_size);

    q = cachedq_init(buf, buf_size);
    if (q == NULL) {
        perror("fail to create the cachedq");
    }

    pthread_t thread_w[WRITER_THREAD] = {0};
    for (vsize_t i = 0; i < WRITER_THREAD; i++) {
        pthread_create(&thread_w[i], NULL, writer, (void *)i);
    }

    pthread_t thread_r[READER_THREAD] = {0};
    for (vsize_t i = 0; i < READER_THREAD; i++) {
        pthread_create(&thread_r[i], NULL, reader, (void *)i);
    }

    for (vsize_t i = 0; i < WRITER_THREAD; i++) {
        pthread_join(thread_w[i], NULL);
    }

    for (vsize_t i = 0; i < READER_THREAD; i++) {
        pthread_join(thread_r[i], NULL);
    }

    free(buf);
    return 0;
}
