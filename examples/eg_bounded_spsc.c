
#include <vsync/queue/bounded_spsc.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#define IT      10
#define BUFF_SZ 1000

void *g_buffer[BUFF_SZ];

bounded_spsc_t g_queue;

typedef struct data_s {
    vsize_t id;
} data_t;

void *
producer(void *args)
{
    data_t *data         = NULL;
    bounded_ret_t result = 0;
    for (vsize_t i = 0; i < IT; i++) {
        data     = malloc(sizeof(data_t));
        data->id = i;
        result   = bounded_spsc_enq(&g_queue, data);
        if (result == QUEUE_BOUNDED_OK) {
            printf("[producer] enq %zu\n", data->id);
        } else {
            // queue is full
            free(data);
        }
    }

    (void)args;
    return NULL;
}

void *
consumer(void *args)
{
    void *data           = NULL;
    bounded_ret_t result = 0;
    for (vsize_t i = 0; i < IT; i++) {
        result = bounded_spsc_deq(&g_queue, &data);
        if (result == QUEUE_BOUNDED_OK) {
            printf("[consumer] deq %zu\n", ((data_t *)data)->id);
            free(data);
        }
    }

    (void)args;
    return NULL;
}

int
main(void)
{
    void *data = NULL;

    pthread_t t_consumer;
    pthread_t t_producer;

    bounded_spsc_init(&g_queue, g_buffer, BUFF_SZ);

    pthread_create(&t_producer, NULL, producer, NULL);
    pthread_create(&t_consumer, NULL, consumer, NULL);

    pthread_join(t_consumer, NULL);
    pthread_join(t_producer, NULL);

    while (bounded_spsc_deq(&g_queue, &data) != QUEUE_BOUNDED_EMPTY) {
        free(data);
    }
    return 0;
}
