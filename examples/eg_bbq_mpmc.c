#include <vsync/queue/bbq_mpmc.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#define BUFFER_ENTRY_NUM 4096U
#define NUM              10U
#define ENQUEUE_BATCH    5UL
#define DEQUEUE_BATCH    4UL
#define NUM_WRITER       4U
#define NUM_READER       5U

#define NON_BLOCKING false
#define BLOCKING     true

bbq_mpmc_t *g_bbq_mpmc = NULL;

void *
writer(void *arg)
{
    vuintptr_t buf[ENQUEUE_BATCH] = {0};
    vuint64_t ptr                 = 0;
    vuint64_t rest                = NUM * NUM_READER;

    while (rest) {
        vuint32_t count = rest < ENQUEUE_BATCH ? rest : ENQUEUE_BATCH;
        for (vuint32_t i = 0; i < count; i++) {
            buf[i] = ++ptr;
        }
        rest -= bbq_mpmc_enqueue(g_bbq_mpmc, buf, count, BLOCKING);
    }

    (void)arg;
    return NULL;
}

void *
reader(void *arg)
{
    vuintptr_t buf[DEQUEUE_BATCH] = {0};
    vuint64_t rest                = NUM * NUM_WRITER;

    while (rest) {
        vuint32_t batch_size = rest < DEQUEUE_BATCH ? rest : DEQUEUE_BATCH;
        vuint32_t count =
            bbq_mpmc_dequeue(g_bbq_mpmc, buf, batch_size, NON_BLOCKING);

        for (vuint32_t i = 0; i < count; i++) {
            ASSERT(buf[i] > 0);
            printf("dequeue item %lu\n", buf[i]);
        }
        rest -= count;
    }
    (void)arg;
    return NULL;
}

int
main(void)
{
    pthread_t t_writer[NUM_WRITER];
    pthread_t t_reader[NUM_READER];

    // allocate
    vsize_t sz = bbq_mpmc_memsize(BUFFER_ENTRY_NUM);
    g_bbq_mpmc = (bbq_mpmc_t *)malloc(sz);
    ASSERT(g_bbq_mpmc);

    // init
    vbool_t success = bbq_mpmc_init(g_bbq_mpmc, sz);
    ASSERT(success);

    for (vsize_t i = 0; i < NUM_WRITER; i++) {
        pthread_create(&t_writer[i], NULL, writer, NULL);
    }
    for (vsize_t i = 0; i < NUM_READER; i++) {
        pthread_create(&t_reader[i], NULL, reader, NULL);
    }
    for (vsize_t i = 0; i < NUM_WRITER; i++) {
        pthread_join(t_writer[i], NULL);
    }
    for (vsize_t i = 0; i < NUM_READER; i++) {
        pthread_join(t_reader[i], NULL);
    }

    // deallocate
    free(g_bbq_mpmc);
    return 0;
}
