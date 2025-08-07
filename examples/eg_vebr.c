
#include <vsync/queue/unbounded_queue_lf.h>
#include <vsync/smr/ebr.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define IT 10000
#define N  12

typedef struct data_s {
    vsize_t id;
} data_t;

vebr_t g_vebr;
vqueue_ub_t g_queue;
pthread_mutex_t g_lock;

static inline void
lock_acq(void *arg)
{
    pthread_mutex_t *lock = (pthread_mutex_t *)arg;
    int ret               = pthread_mutex_lock(lock);
    assert(ret == 0);
}

static inline void
lock_rel(void *arg)
{
    pthread_mutex_t *lock = (pthread_mutex_t *)arg;
    int ret               = pthread_mutex_unlock(lock);
    assert(ret == 0);
}

smr_lock_lib_t g_lock_lib = {lock_acq, lock_rel, &g_lock};

void
free_cb(smr_node_t *node, void *args)
{
    vqueue_ub_node_t *qnode = V_CONTAINER_OF(node, vqueue_ub_node_t, smr_node);
    free(qnode);
    (void)args;
}

void
retire_cb(vqueue_ub_node_t *qnode, void *args)
{
    vebr_thread_t *thrd = args;
    vebr_retire(&g_vebr, thrd, &qnode->smr_node, free_cb, NULL);
}

void
destroy_cb(vqueue_ub_node_t *qnode, void *args)
{
    free(qnode);
    (void)args;
}

vatomic8_t g_stop = VATOMIC_INIT(0);

void
reclaim(void)
{
    while (vatomic8_read(&g_stop) == 0) {
        vsize_t count = vebr_recycle(&g_vebr);
        if (count > 0) {
            printf("%zu node(s) were reclaimed\n", count);
        }
    }
}

void *
run(void *args)
{
    vebr_thread_t thread;
    data_t *data = NULL;
    vsize_t tid  = (vsize_t)args;

    if (tid == 0) {
        reclaim();
    } else {
        vebr_register(&g_vebr, &thread);

        for (vsize_t i = 0; i < IT; i++) {
            vebr_enter(&g_vebr, &thread);

            data = vqueue_ub_deq(&g_queue, retire_cb, &thread);
            if (data == NULL) {
                data                    = malloc(sizeof(data_t));
                data->id                = i;
                vqueue_ub_node_t *qnode = malloc(sizeof(vqueue_ub_node_t));
                printf("[T%zu] enq %zu\n", tid, data->id);
                vqueue_ub_enq(&g_queue, qnode, data);
            } else {
                printf("[T%zu] deq %zu\n", tid, data->id);
                free(data);
            }
            vebr_exit(&g_vebr, &thread);
        }

        vebr_deregister(&g_vebr, &thread);
    }
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];
    data_t *data = NULL;

    int ret = pthread_mutex_init(&g_lock, NULL);
    assert(ret == 0);

    vebr_init(&g_vebr, g_lock_lib);
    vqueue_ub_init(&g_queue);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 1; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    vatomic8_write(&g_stop, 1);
    pthread_join(threads[0], NULL);

    /* dequeue all remaining nodes, to be able to destroy data */
    while (data = vqueue_ub_deq(&g_queue, destroy_cb, NULL), data) {
        free(data);
    }

    /* destroy the queue to destroy the remaining sentinel */
    vqueue_ub_destroy(&g_queue, destroy_cb, NULL);

    vebr_destroy(&g_vebr);

    ret = pthread_mutex_destroy(&g_lock);
    assert(ret == 0);
    return 0;
}
