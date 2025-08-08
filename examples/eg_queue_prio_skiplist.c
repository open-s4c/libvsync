#include <vsync/queue/vqueue_prio_skiplist_based.h>
#include <vsync/smr/gdump.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define IT 2
#define N  3

typedef struct data_s {
    smr_node_t smr_node;
    vsize_t id;
    vqueue_prio_node_t qnode; /* embed as last field it contains VLA */
} data_t;

gdump_t g_gdump;
vqueue_prio_t g_queue;
vqueue_prio_sentinel_t *g_head;
vqueue_prio_sentinel_t *g_tail;

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
    data_t *data = V_CONTAINER_OF(node, data_t, smr_node);
    free(data);
    (void)args;
}

void
retire_cb(vqueue_prio_node_t *node, void *args)
{
    data_t *data = V_CONTAINER_OF(node, data_t, qnode);
    gdump_retire(&g_gdump, &data->smr_node, free_cb, NULL);
    (void)args;
}

void
destroy_cb(vqueue_prio_node_t *node, void *args)
{
    data_t *data = V_CONTAINER_OF(node, data_t, qnode);
    free(data);
    (void)args;
}

vatomic8_t g_stop = VATOMIC_INIT(0);

int
yield_cb(void *args)
{
    (void)args;
    return sched_yield();
}

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
    vqueue_prio_node_t *node = NULL;
    data_t *data             = NULL;
    vsize_t tid              = (vsize_t)args;

    if (tid == 0) {
        reclaim();
    } else {
        gdump_register(&g_gdump, &thread);

        for (vuint32_t i = 0; i < IT; i++) {
            gdump_enter(&g_gdump, &thread);
            node = vqueue_prio_remove_min(&g_queue);
            if (node == NULL) {
                vsize_t height = 0;
                vsize_t sz =
                    vqueue_prio_calc_node_sz(&g_queue, sizeof(data_t), &height);
                data     = malloc(sz);
                data->id = i;
                vqueue_prio_add(&g_queue, &data->qnode, i, height);
                printf("[T%zu] enq %zu\n", tid, data->id);
            } else {
                data = V_CONTAINER_OF(node, data_t, qnode);
                printf("[T%zu] deq %zu\n", tid, data->id);
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

    int ret = pthread_rwlock_init(&g_lock, NULL);
    assert(ret == 0);

    gdump_init(&g_gdump, g_rwlock_lib);

    g_head = malloc(VQUEUE_PRIO_SENTINEL_SZ);
    g_tail = malloc(VQUEUE_PRIO_SENTINEL_SZ);

    vqueue_prio_init(&g_queue, retire_cb, NULL, (vuint32_t)rand(), g_head,
                     g_tail);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 1; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    vatomic8_write(&g_stop, 1);
    pthread_join(threads[0], NULL);

    vqueue_prio_destroy(&g_queue, destroy_cb, NULL);
    gdump_destroy(&g_gdump);

    free(g_head);
    free(g_tail);

    ret = pthread_rwlock_destroy(&g_lock);
    assert(ret == 0);

    return 0;
}
