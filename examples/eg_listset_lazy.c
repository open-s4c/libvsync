
#include <vsync/map/listset_lazy.h>
#include <vsync/smr/gdump.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define N 11

typedef struct data_s {
    vlistset_node_t lst_node; /* embed list set node */
    vlistset_key_t key;
    smr_node_t smr_node; /* embed smr node */
} data_t;

#define MIN_KEY 0
#define MAX_KEY 10

gdump_t g_gdump;

vlistset_t g_lst;

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
free_cb(smr_node_t *snode, void *args)
{
    data_t *data = V_CONTAINER_OF(snode, data_t, smr_node);
    free(data);
    (void)args;
}

int
cmp_cb(vlistset_node_t *node, vlistset_key_t key)
{
    data_t *data = (data_t *)node;
    if (data->key == key) {
        return 0;
    } else if (data->key < key) {
        return -1;
    } else {
        return 1;
    }
}

void
retire_cb(vlistset_node_t *node, void *args)
{
    data_t *data = (data_t *)node;
    gdump_retire(&g_gdump, &data->smr_node, free_cb, NULL);
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

void
reader(gdump_thread_t *thread)
{
    data_t *data = NULL;
    gdump_register(&g_gdump, thread);
    for (vlistset_key_t key = MIN_KEY; key <= MAX_KEY; key++) {
        gdump_enter(&g_gdump, thread);
        data = (data_t *)vlistset_get(&g_lst, key);
        if (data) {
            assert(key == data->key);
            printf("Reader found key %lu\n", data->key);
        }
        gdump_exit(&g_gdump, thread);
    }
    gdump_deregister(&g_gdump, thread);
}

void
writer(gdump_thread_t *thread)
{
    data_t *data    = NULL;
    vbool_t success = false;

    gdump_register(&g_gdump, thread);
    for (vlistset_key_t key = MIN_KEY; key <= MAX_KEY; key++) {
        gdump_enter(&g_gdump, thread);
        success = vlistset_remove(&g_lst, key);
        gdump_exit(&g_gdump, thread);
        if (success) {
            printf("Removed node with key %lu\n", key);
        } else {
            data      = malloc(sizeof(data_t));
            data->key = key;
            gdump_enter(&g_gdump, thread);
            success = vlistset_add(&g_lst, key, &data->lst_node);
            gdump_exit(&g_gdump, thread);
            if (success) {
                printf("Added node with key %lu\n", key);
            } else {
                free(data);
            }
        }
    }
    gdump_deregister(&g_gdump, thread);

    // writer is done, it can trigger recycle if it wishes
    vsize_t count = gdump_recycle(&g_gdump, yield_cb, NULL, 1);
    printf("writer reclaimed %zu nodes\n", count);
}

void *
run(void *args)
{
    gdump_thread_t thread;
    vsize_t tid = (vsize_t)args;

    if (tid == 0) {
        reclaim();
    } else if (tid % 2 == 0) {
        writer(&thread);
    } else {
        reader(&thread);
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
    vlistset_init(&g_lst, retire_cb, NULL, cmp_cb);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 1; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    vatomic8_write(&g_stop, 1);
    pthread_join(threads[0], NULL);

    vlistset_destroy(&g_lst);
    gdump_destroy(&g_gdump);

    ret = pthread_rwlock_destroy(&g_lock);
    assert(ret == 0);
    return 0;
}
