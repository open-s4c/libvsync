#  [vsync](../README.md) / [map](README.md) / hashtable_standard.h
_This is a lock-free listset based hashtable._ 

**Groups:** [Linearizable](../GROUP_linearizable.md), [Lock-free](../GROUP_lock_free.md), [SMR-required](../GROUP_requires_smr.md)

This hashtable uses a lock-free listset per bucket. The listset is ordered by keys, and keys are expected to be unique.

The table consists of `VHASHTABLE_BUCKET_COUNT` number of buckets with a default value of `1024`. This can be overwritten by the user by compiling with `-DVHASHTABLE_BUCKET_COUNT=N`. User control the mapping between keys and buckets by passing `hash_idx` to the APIs. It is expected that users use their own hash functions that map a key to a certain bucket index. Thus the given `hash_idx` is expected to be `< VHASHTABLE_BUCKET_COUNT`.

> **Note:** users may easily build their own hashtables e.g. when dynamic allocation of the table is needed. Users can use [vsync/map/listset_lf.h](listset_lf.h.md) or any other listset e.g. [vsync/map/listset_lazy.h](listset_lazy.h.md) depending on the user needs. [listset_lf.h](listset_lf.h.md) and [listset_lazy.h](listset_lazy.h.md) can provide performance benefits under high contention and oversubscription scenarios. If memory overhead is not an issue and the buckets can grow long then [skiplist_lf.h](skiplist_lf.h.md) can be a better fit.


### Example:



```c
#include <vsync/smr/gdump.h>
#include <vsync/map/hashtable_standard.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define N    12
#define ITER 10

typedef struct entry_s {
    vhashtable_entry_t ht_entry;
    vhashtable_key_t key;
    smr_node_t smr_node;
} entry_t;

pthread_rwlock_t g_lock;

vhashtable_t g_hash_tbl;

gdump_t g_gdump;

vatomic8_t g_stop = VATOMIC_INIT(0);

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
free_callback(smr_node_t *node, void *arg)
{
    entry_t *entry = V_CONTAINER_OF(node, entry_t, smr_node);
    free(entry);
    V_UNUSED(arg);
}

void
retire_callback(vhashtable_entry_t *node, void *arg)
{
    entry_t *entry = V_CONTAINER_OF(node, entry_t, ht_entry);
    gdump_retire(&g_gdump, &entry->smr_node, free_callback, NULL);
    V_UNUSED(arg);
}

int
cmp_callback(vhashtable_entry_t *node, vhashtable_key_t key)
{
    entry_t *entry = V_CONTAINER_OF(node, entry_t, ht_entry);

    if (entry->key == key)
        return 0;
    else if (entry->key < key)
        return -1;
    else
        return 1;
}

vsize_t
hashit(vhashtable_key_t key)
{
    return key % VHASHTABLE_BUCKET_COUNT;
}

void
reader_writer(void)
{
    gdump_thread_t thread;
    entry_t *entry  = NULL;
    vbool_t success = false;

    gdump_register(&g_gdump, &thread);

    for (vsize_t i = 0; i < ITER; i++) {
        vhashtable_key_t key = (vhashtable_key_t)i;

        gdump_enter(&g_gdump, &thread);

        vhashtable_entry_t *node =
            vhashtable_get(&g_hash_tbl, key, hashit(key));
        if (node) {
            entry = V_CONTAINER_OF(node, entry_t, ht_entry);
            printf("entry with key %lu exists\n", entry->key);
            assert(key == entry->key);
            success = vhashtable_remove(&g_hash_tbl, key, hashit(key));
            if (success) {
                printf("removed entry with key %lu \n", entry->key);
            }
        } else {
            entry      = malloc(sizeof(entry_t));
            entry->key = key;
            success    = vhashtable_insert(&g_hash_tbl, key, &entry->ht_entry,
                                           hashit(key));
            if (success) {
                printf("added entry with key %lu \n", entry->key);
            } else {
                free(entry);
            }
        }

        gdump_exit(&g_gdump, &thread);
    }

    gdump_deregister(&g_gdump, &thread);
}

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
    vsize_t tid = (vsize_t)args;
    if (tid == 0) {
        reclaim();
    } else {
        reader_writer();
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
    vhashtable_init(&g_hash_tbl, retire_callback, NULL, cmp_callback);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 1; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    vatomic8_write(&g_stop, 1);
    pthread_join(threads[0], NULL);

    vhashtable_destroy(&g_hash_tbl);
    gdump_destroy(&g_gdump);

    ret = pthread_rwlock_destroy(&g_lock);
    assert(ret == 0);
    return 0;
}
```

 

---
# Functions 

| Function | Description |
|---|---|
| [vhashtable_init](hashtable_standard.h.md#function-vhashtable_init) | Initializes the hashtable.  |
| [vhashtable_destroy](hashtable_standard.h.md#function-vhashtable_destroy) | Retires all entries in the hashtable.  |
| [vhashtable_insert](hashtable_standard.h.md#function-vhashtable_insert) | Inserts the given `entry` into the hashtable.  |
| [vhashtable_get](hashtable_standard.h.md#function-vhashtable_get) | Looks for the entry associated with the given `key`.  |
| [vhashtable_remove](hashtable_standard.h.md#function-vhashtable_remove) | Removes the entry associated with the given key from the the hashtable.  |
| [vhashtable_get_entries_count](hashtable_standard.h.md#function-vhashtable_get_entries_count) | Returns the count of entries currently available in the hashtable.  |

##  Function `vhashtable_init`

```c
static void vhashtable_init(vhashtable_t *table, vhashtable_entry_retire_t retire_cb, void *retire_cb_arg, vhashtable_cmp_key_t cmp_cb)
``` 
_Initializes the hashtable._ 




**Parameters:**

- `table`: address of vhashtable_t object. 
- `retire_cb`: address of a callback function of type `vhashtable_entry_retire_t`. The function is called whenever an entry is detached from the hashtable. 
- `retire_cb_arg`: address of an extra argument of `retire_cb`. 
- `cmp_cb`: address of callback function of type `vhashtable_cmp_key_t`. Used for comparing an entry key to a given key.


> **Note:** must be called before threads start accessing the hashtable. 


##  Function `vhashtable_destroy`

```c
static void vhashtable_destroy(vhashtable_t *table)
``` 
_Retires all entries in the hashtable._ 




**Parameters:**

- `table`: address of vhashtable_t object.


> **Note:** this function is not thread-safe, can be called iff all threads are done accessing the hashtable. 


##  Function `vhashtable_insert`

```c
static vbool_t vhashtable_insert(vhashtable_t *table, vhashtable_key_t key, vhashtable_entry_t *val, vsize_t hash_idx)
``` 
_Inserts the given_ `entry` _into the hashtable._ 




**Parameters:**

- `table`: address of vhashtable_t object. 
- `key`: value of the key object. 
- `entry`: address of vhashtable_entry_t object. 
- `hash_idx`: a hash value of the key used as a bucket index. must be `< VHASHTABLE_BUCKET_COUNT` 


**Returns:** true successful insertion. 

**Returns:** false failed insertion, an entry associated with the given `key` already exists.

> **Note:** this function must be called inside an SMR critical section. 


##  Function `vhashtable_get`

```c
static vhashtable_entry_t* vhashtable_get(vhashtable_t *table, vhashtable_key_t key, vsize_t hash_idx)
``` 
_Looks for the entry associated with the given_ `key`_._ 




**Parameters:**

- `table`: address of vhashtable_t object. 
- `key`: value of the key object. 
- `hash_idx`: a hash value of the key used as a bucket index. must be `< VHASHTABLE_BUCKET_COUNT` 


**Returns:** vhashtable_entry_t* address of the `vhashtable_entry_t` object associated with the given key, if found. 

**Returns:** `NULL` if key is not found.

> **Note:** this function must be called inside an SMR critical section. 


##  Function `vhashtable_remove`

```c
static vbool_t vhashtable_remove(vhashtable_t *table, vhashtable_key_t key, vsize_t hash_idx)
``` 
_Removes the entry associated with the given key from the the hashtable._ 




**Parameters:**

- `table`: address of vhashtable_t object. 
- `key`: value of the key object. 
- `hash_idx`: a hash value of the key used as a bucket index. must be `< VHASHTABLE_BUCKET_COUNT`. 


**Returns:** true successful remove, the entry associated with the given key was removed. 

**Returns:** false failed remove, no entry associated with the given key exists. 



##  Function `vhashtable_get_entries_count`

```c
static vuint64_t vhashtable_get_entries_count(vhashtable_t *table)
``` 
_Returns the count of entries currently available in the hashtable._ 




**Parameters:**

- `table`: address of vhashtable_t object. 


**Returns:** vuint64_t an approximate count of the entries available in the hashtable. 




---
