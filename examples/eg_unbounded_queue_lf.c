
#include <vsync/queue/unbounded_queue_lf.h>
/* define VGDUMP_ENABLE_BUFF to enable thread local buffer of retired nodes */
#define VGDUMP_ENABLE_BUFF
#include <vsync/smr/gdump.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define IT 10000
#define N  12

typedef struct data_s {
    vsize_t id;
} data_t;

gdump_t g_gdump;
vqueue_ub_t g_queue;
pthread_rwlock_t g_lock;

static inline void
thread_rw_read_acq(void *arg)
{
    pthread_rwlock_t *lock = (pthread_rwlock_t *)arg;
    int ret                = pthread_rwlock_rdlock(lock);
    assert(ret == 0);
}

static inline void
thread_rw_read_rel(void *arg)
{
    pthread_rwlock_t *lock = (pthread_rwlock_t *)arg;
    int ret                = pthread_rwlock_unlock(lock);
    assert(ret == 0);
}
static inline void
thread_rw_write_acq(void *arg)
{
    pthread_rwlock_t *lock = (pthread_rwlock_t *)arg;
    int ret                = pthread_rwlock_wrlock(lock);
    assert(ret == 0);
}
static inline void
thread_rw_write_rel(void *arg)
{
    pthread_rwlock_t *lock = (pthread_rwlock_t *)arg;
    int ret                = pthread_rwlock_unlock(lock);
    assert(ret == 0);
}

smr_rwlock_lib_t g_rwlock_lib = {thread_rw_read_acq, thread_rw_read_rel,
                                 thread_rw_write_acq, thread_rw_write_rel,
                                 &g_lock};

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
    gdump_thread_t *thrd = args;
    gdump_retire_local(&g_gdump, thrd, &qnode->smr_node, free_cb, NULL);
}

void
destroy_cb(vqueue_ub_node_t *qnode, void *args)
{
    free(qnode);
    (void)args;
}

int
yield_cb(void *args)
{
    (void)args;
    return sched_yield();
}

vatomic8_t g_stop = VATOMIC_INIT(0);

void
reclaim(void)
{
    while (vatomic8_read(&g_stop) == 0) {
        vsize_t count = gdump_recycle(&g_gdump, yield_cb, NULL, 1);
        if (count > 0) {
            printf("%zu node(s) were reclaimed\n", count);
        }
    }
}

void *
run(void *args)
{
    gdump_thread_t thread;
    data_t *data = NULL;
    vsize_t tid  = (vsize_t)args;

    if (tid == 0) {
        reclaim();
    } else {
        gdump_register(&g_gdump, &thread);

        for (vsize_t i = 0; i < IT; i++) {
            gdump_enter(&g_gdump, &thread);

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
            gdump_exit(&g_gdump, &thread);
        }

        gdump_deregister(&g_gdump, &thread);
    }
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];
    data_t *data = NULL;

    int ret = pthread_rwlock_init(&g_lock, NULL);
    assert(ret == 0);

    gdump_init(&g_gdump, g_rwlock_lib);
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

    gdump_destroy(&g_gdump);

    ret = pthread_rwlock_destroy(&g_lock);
    assert(ret == 0);
    return 0;
}
