
#include <vsync/map/hashset_lf.h>
#include <vsync/smr/gdump.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define N 11

typedef struct data_s {
    vhashset_node_t hs_node; /* embed hashset node */
    vhash_t hash;
    smr_node_t smr_node; /* embed smr node */
} data_t;

#define MIN_KEY 0
#define MAX_KEY 10

gdump_t g_gdump;

vhashset_t g_hs;

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
free_node_cb(smr_node_t *snode, void *args)
{
    data_t *data = V_CONTAINER_OF(snode, data_t, smr_node);
    free(data);
    (void)args;
}

void
redirect_retire_cb(smr_node_t *smr_node, smr_node_free cb, void *args)
{
    gdump_retire(&g_gdump, smr_node, cb, args);
}

void
retire_cb(vhashset_node_t *node, void *args)
{
    data_t *data = (data_t *)node;
    gdump_retire(&g_gdump, &data->smr_node, free_node_cb, NULL);
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
    for (vhash_t key = MIN_KEY; key <= MAX_KEY; key++) {
        gdump_enter(&g_gdump, thread);
        data = (data_t *)vhashset_get(&g_hs, key);
        if (data) {
            assert(key == data->hash);
            printf("Reader found key %lu\n", data->hash);
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
    for (vhash_t key = MIN_KEY; key <= MAX_KEY; key++) {
        gdump_enter(&g_gdump, thread);
        success = vhashset_remove(&g_hs, key);
        gdump_exit(&g_gdump, thread);
        if (success) {
            printf("Removed node with key %lu\n", key);
        } else {
            data       = malloc(sizeof(data_t));
            data->hash = key;
            gdump_enter(&g_gdump, thread);
            success = vhashset_add(&g_hs, key, &data->hs_node);
            gdump_exit(&g_gdump, thread);
            if (success) {
                printf("Added node with key %lu\n", key);
            } else {
                free(data);
            }
        }
    }
    gdump_deregister(&g_gdump, thread);
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

void *
malloc_cb(vsize_t sz, void *arg)
{
    (void)arg;
    return malloc(sz);
}

void
free_cb(void *ptr, void *arg)
{
    (void)arg;
    free(ptr);
}

int
main(void)
{
    pthread_t threads[N];

    vmem_lib_t mem_lib = {.free_fun   = free_cb,
                          .malloc_fun = malloc_cb,
                          .arg        = NULL};

    int ret = pthread_rwlock_init(&g_lock, NULL);
    assert(ret == 0);

    gdump_init(&g_gdump, g_rwlock_lib);
    vhashset_init(&g_hs, mem_lib, redirect_retire_cb, retire_cb, NULL);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 1; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    vatomic8_write(&g_stop, 1);
    pthread_join(threads[0], NULL);

    vhashset_destroy(&g_hs);
    gdump_destroy(&g_gdump);

    ret = pthread_rwlock_destroy(&g_lock);
    assert(ret == 0);
    return 0;
}
