#include <vsync/queue/bbq_spsc.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#define BUFFER_ENTRY_NUM 4096U
#define NUM              10U
#define ENQUEUE_BATCH    5UL
#define DEQUEUE_BATCH    4UL

#define NON_BLOCKING false
#define BLOCKING     true

bbq_spsc_t *g_bbq_spsc = NULL;

void *
writer(void *arg)
{
    vuintptr_t buf[ENQUEUE_BATCH] = {0};
    vuint64_t ptr                 = 0;
    vuint64_t rest                = NUM;

    while (rest) {
        vuint32_t count = rest < ENQUEUE_BATCH ? rest : ENQUEUE_BATCH;
        for (vuint32_t i = 0; i < count; i++) {
            buf[i] = ptr++;
        }
        rest -= bbq_spsc_enqueue(g_bbq_spsc, buf, count, BLOCKING);
    }

    (void)arg;
    return NULL;
}

void *
reader(void *arg)
{
    vuintptr_t buf[DEQUEUE_BATCH] = {0};
    vuint64_t ptr                 = 0;
    vuint64_t rest                = NUM;

    while (rest) {
        vuint32_t count =
            bbq_spsc_dequeue(g_bbq_spsc, buf, DEQUEUE_BATCH, NON_BLOCKING);

        for (vuint32_t i = 0; i < count; i++) {
            ASSERT(buf[i] == ptr++);
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
    pthread_t t_writer;
    pthread_t t_reader;

    // allocate
    vsize_t sz = bbq_spsc_memsize(BUFFER_ENTRY_NUM);
    g_bbq_spsc = (bbq_spsc_t *)malloc(sz);
    ASSERT(g_bbq_spsc);

    // init
    vbool_t success = bbq_spsc_init(g_bbq_spsc, sz);
    ASSERT(success);

    pthread_create(&t_writer, NULL, writer, NULL);
    pthread_create(&t_reader, NULL, reader, NULL);
    pthread_join(t_reader, NULL);
    pthread_join(t_writer, NULL);

    // deallocate
    free(g_bbq_spsc);
    return 0;
}
