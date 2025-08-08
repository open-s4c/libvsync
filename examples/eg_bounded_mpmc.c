
#include <vsync/queue/bounded_mpmc.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#define IT      10
#define N       12
#define BUFF_SZ 1000

void *g_buffer[BUFF_SZ];
bounded_mpmc_t g_queue;

typedef struct data_s {
    vsize_t id;
} data_t;

void
produce(vsize_t tid)
{
    data_t *data         = NULL;
    bounded_ret_t result = 0;
    for (vsize_t i = 0; i < IT; i++) {
        data     = malloc(sizeof(data_t));
        data->id = i;
        result   = bounded_mpmc_enq(&g_queue, data);
        if (result == QUEUE_BOUNDED_OK) {
            printf("[T%zu] enq %zu\n", tid, data->id);
        } else {
            // either full/ busy
            free(data);
        }
    }
}

void
consume(vsize_t tid)
{
    void *data           = NULL;
    bounded_ret_t result = 0;
    for (vsize_t i = 0; i < IT; i++) {
        result = bounded_mpmc_deq(&g_queue, &data);
        if (result == QUEUE_BOUNDED_OK) {
            printf("[T%zu] deq %zu\n", tid, ((data_t *)data)->id);
            free(data);
        }
    }
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;
    if (tid % 2 == 0) {
        produce(tid);
    } else {
        consume(tid);
    }
    return NULL;
}

int
main(void)
{
    void *data = NULL;
    pthread_t threads[N];

    bounded_mpmc_init(&g_queue, g_buffer, BUFF_SZ);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    while (bounded_mpmc_deq(&g_queue, &data) != QUEUE_BOUNDED_EMPTY) {
        free(data);
    }
    return 0;
}
