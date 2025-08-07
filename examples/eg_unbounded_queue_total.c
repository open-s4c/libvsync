
#include <vsync/queue/unbounded_queue_total.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#define IT 10000
#define N  12

typedef struct data_s {
    vsize_t id;
} data_t;

vqueue_ub_t g_queue;

void
destroy_cb(vqueue_ub_node_t *qnode, void *args)
{
    free(qnode);
    (void)args;
}

void *
run(void *args)
{
    data_t *data = NULL;
    vsize_t tid  = (vsize_t)args;

    for (vsize_t i = 0; i < IT; i++) {
        data = vqueue_ub_deq(&g_queue, destroy_cb, NULL);
        if (data == NULL) {
            data                    = malloc(sizeof(data_t));
            data->id                = i;
            vqueue_ub_node_t *qnode = malloc(sizeof(vqueue_ub_t));
            printf("[T%zu] enq %zu\n", tid, data->id);
            vqueue_ub_enq(&g_queue, qnode, data);
        } else {
            printf("[T%zu] deq %zu\n", tid, data->id);
            free(data);
        }
    }

    return NULL;
}

int
main(void)
{
    pthread_t threads[N];
    data_t *data = NULL;

    vqueue_ub_init(&g_queue);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    /* dequeue all remaining nodes, to be able to destroy data */
    while (data = vqueue_ub_deq(&g_queue, destroy_cb, NULL), data) {
        free(data);
    }

    /* destroy the queue to destroy the remaining sentinel */
    vqueue_ub_destroy(&g_queue, destroy_cb, NULL);
    return 0;
}
