#include <vsync/map/simpleht.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N                  3U
#define HASHTABLE_CAPACITY 32U
#define MIN_KEY            1U
#define MAX_KEY            (HASHTABLE_CAPACITY / 2)

typedef struct data_s {
    vuintptr_t key;
    // data
} data_t;

vsimpleht_t g_hashtable;

vuint64_t
hash_cb(vuintptr_t key)
{
    vuint64_t h = key;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

vint8_t
cmp_key_cb(vuintptr_t a, vuintptr_t b)
{
    if (a < b) {
        return -1;
    } else if (a > b) {
        return 1;
    } else {
        return 0;
    }
}

void
destroy_cb(void *data)
{
    free(data);
}

void *
run(void *args)
{
    vsize_t tid         = (vsize_t)(vuintptr_t)args;
    data_t *data        = NULL;
    vsimpleht_ret_t ret = VSIMPLEHT_RET_OK;

    vsimpleht_thread_register(&g_hashtable);

    for (vuintptr_t key = MIN_KEY; key < MAX_KEY; key++) {
        data = vsimpleht_get(&g_hashtable, key);
        if (data) {
            printf("T%zu: found key %lu\n", tid, key);
            ASSERT(data->key == key);
            ret = vsimpleht_remove(&g_hashtable, key);
            if (ret == VSIMPLEHT_RET_OK) {
                printf("T%zu: removed key %lu\n", tid, key);
            } else {
                printf("T%zu: key %lu does not exist\n", tid, key);
            }
        } else {
            data      = malloc(sizeof(data_t));
            data->key = key;
            ret       = vsimpleht_add(&g_hashtable, key, data);
            if (ret == VSIMPLEHT_RET_OK) {
                printf("T%zu: added key %lu\n", tid, key);
            } else if (ret == VSIMPLEHT_RET_TBL_FULL) {
                printf("T%zu: table is full! cannot add\n", tid);
                free(data);
            } else if (ret == VSIMPLEHT_RET_KEY_EXISTS) {
                printf("T%zu: key %lu already exists\n", tid, key);
                free(data);
            }
        }
    }
    vsimpleht_thread_deregister(&g_hashtable);
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    vsize_t size = vsimpleht_buff_size(HASHTABLE_CAPACITY);
    void *buffer = malloc(size);

    vsimpleht_init(&g_hashtable, buffer, HASHTABLE_CAPACITY, cmp_key_cb,
                   hash_cb, destroy_cb);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    vsimpleht_destroy(&g_hashtable);
    free(buffer);
    return 0;
}
