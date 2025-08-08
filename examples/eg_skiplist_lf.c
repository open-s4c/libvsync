
#include <vsync/map/skiplist_lf.h>
#include <vsync/smr/gdump.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define N       11U
#define MIN_KEY 0U
#define MAX_KEY 10U

/* container struct embeds vskiplist_node_t. */
typedef struct data_s {
    vskiplist_key_t key;
    smr_node_t smr_node;       /* embed an smr node */
    vskiplist_node_t lst_node; /* must be last field, contains a VLA */
} data_t;

gdump_t g_gdump;         /* SMR scheme */
pthread_rwlock_t g_lock; /* lock used by gdump */
vatomic8_t g_stop = VATOMIC_INIT(0);

vskiplist_t g_lst;        /* skiplist object */
vskiplist_node_t *g_head; /* sentinel object passed to vskiplist_init */
vskiplist_node_t *g_tail; /* sentinel object passed to vskiplist_init */

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

/* callback functions used by gdump */
smr_rwlock_lib_t g_rwlock_lib = {thread_rw_read_acq, thread_rw_read_rel,
                                 thread_rw_write_acq, thread_rw_write_rel,
                                 &g_lock};

/* free callback, gets called when it is safe to free the object */
void
free_cb(smr_node_t *snode, void *args)
{
    data_t *data = V_CONTAINER_OF(snode, data_t, smr_node);
    free(data);
    (void)args;
}
/* compare callback, used to compare keys in the skiplist */
int
cmp_cb(vskiplist_node_t *node, vskiplist_key_t key)
{
    data_t *data = V_CONTAINER_OF(node, data_t, lst_node);
    if (data->key == key) {
        return 0;
    } else if (data->key < key) {
        return -1;
    } else {
        return 1;
    }
}
/* retire callback, gets called when a node is fully detached from the skiplist
 * and it is time to hand it to the SMR scheme */
void
retire_cb(vskiplist_node_t *node, void *args)
{
    data_t *data = V_CONTAINER_OF(node, data_t, lst_node);
    gdump_retire(&g_gdump, &data->smr_node, free_cb, NULL);
    (void)args;
}
/* yield callback used by gdump */
int
yield_cb(void *args)
{
    (void)args;
    return sched_yield();
}
/* reclaim memory in the back ground by calling gdump_recycle */
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
    vskiplist_node_t *node = NULL;
    data_t *data           = NULL;

    gdump_register(&g_gdump, thread);
    for (vskiplist_key_t key = MIN_KEY; key <= MAX_KEY; key++) {
        /* operations of the skiplist must be called in an SMR critical
         * section */
        gdump_enter(&g_gdump, thread);
        node = vskiplist_lookup(&g_lst, key);
        if (node) {
            data = V_CONTAINER_OF(node, data_t, lst_node);
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
    for (vskiplist_key_t key = MIN_KEY; key <= MAX_KEY; key++) {
        gdump_enter(&g_gdump, thread);
        success = vskiplist_remove(&g_lst, key);
        gdump_exit(&g_gdump, thread);
        if (success) {
            printf("Removed node with key %lu\n", key);
        } else {
            vsize_t height = 0;
            /* vskiplist_calc_node_sz must be called everytime the user wants to
             * add a new node. */
            vsize_t sz =
                vskiplist_calc_node_sz(&g_lst, sizeof(data_t), &height);
            assert(height > 0);
            /* container must have the size returned by vskiplist_calc_node_sz
             */
            data      = malloc(sz);
            data->key = key;
            gdump_enter(&g_gdump, thread);
            /* pass height from vskiplist_calc_node_sz */
            success = vskiplist_add(&g_lst, key, &data->lst_node, NULL, height);
            gdump_exit(&g_gdump, thread);
            if (success) {
                printf("Added node with key %lu\n", key);
            } else {
                /* the node was never attached to the skiplist, so it is safe to
                 * free it directly */
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

int
main(void)
{
    pthread_t threads[N];

    int ret = pthread_rwlock_init(&g_lock, NULL);
    assert(ret == 0);

    /* generate a random seed */
    vuint32_t rand_seed = (vuint32_t)time(0);

    gdump_init(&g_gdump, g_rwlock_lib);

    /* allocate sentinels needed by the skiplist. They must have size =
     * VSKIPLIST_SENTINEL_SZ */
    g_head = malloc(VSKIPLIST_SENTINEL_SZ);
    g_tail = malloc(VSKIPLIST_SENTINEL_SZ);
    vskiplist_init(&g_lst, cmp_cb, retire_cb, NULL, rand_seed, g_head, g_tail);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 1; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    vatomic8_write(&g_stop, 1);
    pthread_join(threads[0], NULL);

    vskiplist_destroy(&g_lst);
    gdump_destroy(&g_gdump);

    ret = pthread_rwlock_destroy(&g_lock);
    /* It is safe to free the allocated sentinel only after the skiplist object
     * is destroyed */
    free(g_head);
    free(g_tail);
    assert(ret == 0);
    return 0;
}
