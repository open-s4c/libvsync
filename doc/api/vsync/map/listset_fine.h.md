#  [vsync](../README.md) / [map](README.md) / listset_fine.h
_This implementation of listset uses fine grained locking._ 

**Groups:** [Linearizable](../GROUP_linearizable.md), [SMR-required](../GROUP_requires_smr.md)

Uses one lock per node to serialize all operations.


### Example:



```c

#include <vsync/map/listset_fine.h>
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
```



> **Note:** SMR usage is required only in the scenario where users access the node returned by `vlistset_get`, which happens without lock protection. The node might then get deleted by a parallel `vlistset_remove` operation. If the user does not wish to access the node itself, and uses `vlistset_get` only to check if a node exists, then SMR is not necessary as long as no dereferencing of the returned address occurs.


### References:

Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 9.5](https://dl.acm.org/doi/pdf/10.5555/2385452) 

---
# Functions 

| Function | Description |
|---|---|
| [vlistset_init](listset_fine.h.md#function-vlistset_init) | Initializes the given vlistset_t object `lst`.  |
| [vlistset_destroy](listset_fine.h.md#function-vlistset_destroy) | Destroys all the remaining nodes in the listset.  |
| [vlistset_add](listset_fine.h.md#function-vlistset_add) | Inserts the given node into the listset.  |
| [vlistset_remove](listset_fine.h.md#function-vlistset_remove) | Removes the node associated with the given key from the listset.  |
| [vlistset_get](listset_fine.h.md#function-vlistset_get) | Looks for the listset node associated with the given key.  |

##  Function `vlistset_init`

```c
static void vlistset_init(vlistset_t *lst, vlistset_handle_node_t retire_fun, void *retire_fun_arg, vlistset_cmp_key_t cmp_fun)
``` 
_Initializes the given vlistset_t object_ `lst`_._ 


> **Note:** must be called before threads access the listset. 

**Parameters:**

- `lst`: address of vlistset_t object. 
- `retire_fun`: address of the function used to retire removed nodes. 
- `retire_fun_arg`: optional second parameter to pass to `retire_fun`. 
- `cmp_fun`: address of the function used for comparing a key in `vlistset_node_t` object with a given key. 




##  Function `vlistset_destroy`

```c
static void vlistset_destroy(vlistset_t *lst)
``` 
_Destroys all the remaining nodes in the listset._ 


> **Note:** call only after thread join, or after all threads finished accessing the given vlistset_t object. 

**Parameters:**

- `lst`: address of vlistset_t object. 




##  Function `vlistset_add`

```c
static vbool_t vlistset_add(vlistset_t *lst, vlistset_key_t key, vlistset_node_t *node)
``` 
_Inserts the given node into the listset._ 


The node is only inserted if there is no other node associated with the given key exists in the listset.

> **Note:** if the operation fails, users can safely free or reuse the given node. 
**Precondition:** the key must be set in the given `node` before calling add. 


**Parameters:**

- `lst`: address of vlistset_t object. 
- `key`: the key value that is used to identify the node. 
- `node`: address of vlistset_node_t object. 


**Returns:** true operation succeeded, the given node has been added. 

**Returns:** false operation failed, the given node has not been added, because there exists another node associated with the given key.

> **Note:** must be called inside SMR critical section. 


##  Function `vlistset_remove`

```c
static vbool_t vlistset_remove(vlistset_t *lst, vlistset_key_t key)
``` 
_Removes the node associated with the given key from the listset._ 


> **Note:** the registered retire callback will be called on the removed node. 

**Parameters:**

- `lst`: address of vlistset_t object. 
- `key`: the key identifying the node to remove. 


**Returns:** true the node associated with the given key was removed successfully. 

**Returns:** false could not find a node holding the given key to remove.

> **Note:** must be called inside SMR critical section. 


##  Function `vlistset_get`

```c
static vlistset_node_t* vlistset_get(vlistset_t *lst, vlistset_key_t key)
``` 
_Looks for the listset node associated with the given key._ 




**Parameters:**

- `lst`: address of vlistset_t object. 
- `key`: key value. 


**Returns:** vlistset_node_t* address of the listset node associated with the given key. 

**Returns:** NULL if no node associated with the given key was found.

> **Note:** must be called inside SMR critical section. 



---
