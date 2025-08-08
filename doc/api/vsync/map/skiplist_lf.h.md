#  [vsync](../README.md) / [map](README.md) / skiplist_lf.h
_Lock-free concurrent skiplist._ 

**Groups:** [Linearizable](../GROUP_linearizable.md), [Lock-free](../GROUP_lock_free.md), [SMR-required](../GROUP_requires_smr.md)

> **Note:** users can configure the maximum levels the skiplist can use by defining `-DVSKIPLIST_MAX_HEIGHT=H`. `H=32` by default.


### Example:



```c
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
```




### References:

Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 14.4](https://dl.acm.org/doi/pdf/10.5555/2385452) 

---
# Functions 

| Function | Description |
|---|---|
| [vskiplist_init](skiplist_lf.h.md#function-vskiplist_init) | Initializes the skiplist.  |
| [vskiplist_destroy](skiplist_lf.h.md#function-vskiplist_destroy) | Destroys all existing nodes in the skiplist.  |
| [vskiplist_lookup](skiplist_lf.h.md#function-vskiplist_lookup) | Searches for the skiplist node associated with the given key.  |
| [vskiplist_calc_node_sz](skiplist_lf.h.md#function-vskiplist_calc_node_sz) | Calculates the required size and height of `vskiplist_node_t` container.  |
| [vskiplist_add](skiplist_lf.h.md#function-vskiplist_add) | Inserts `node` into the skiplist.  |
| [vskiplist_remove](skiplist_lf.h.md#function-vskiplist_remove) | Removes the skiplist node associated with the given key.  |
| [vskiplist_lookup_next](skiplist_lf.h.md#function-vskiplist_lookup_next) | Searches for the skiplist node associated with the given key.  |
| [vskiplist_get_succ](skiplist_lf.h.md#function-vskiplist_get_succ) | Returns the successor of the given node `curr` if a successor exists.  |
| [vskiplist_get_first](skiplist_lf.h.md#function-vskiplist_get_first) | Returns the first node in the given `lst`.  |
| [vskiplist_remove_node](skiplist_lf.h.md#function-vskiplist_remove_node) | Removes the given node associated with the given key if it exists.  |
| [vskiplist_cleanup](skiplist_lf.h.md#function-vskiplist_cleanup) | Triggers the physical removal of all logically removed nodes.  |

##  Function `vskiplist_init`

```c
static void vskiplist_init(vskiplist_t *lst, vskiplist_cmp_node_t fun_cmp, vskiplist_handle_node_t fun_retire, void *fun_retire_arg, vuint32_t rand_seed, vskiplist_node_t *head, vskiplist_node_t *tail)
``` 
_Initializes the skiplist._ 


> **Note:** call it before threads start accessing the skiplist.



**Parameters:**

- `lst`: address of vskiplist_t object. 
- `fun_cmp`: function pointer. This function is used to compare a given skiplist node against a given key. It must return the following:
- container(node)->key == key -> 0
- container(node)->key < key -> -1
- container(node)->key > key -> 1 

- `fun_retire`: function pointer to callback retire function. 
- `fun_retire_arg`: the second parameter of `fun_retire`. 
- `rand_seed`: a random seed used in random node height generation. 
- `head`: the address of the dynamically allocated head sentinel object of size `VSKIPLIST_SENTINEL_SZ`. 
- `tail`: the address of the dynamically allocated tail sentinel object of size `VSKIPLIST_SENTINEL_SZ`.


> **Note:** both `head` and `tail` must have size `VSKIPLIST_SENTINEL_SZ`.

> **Note:** `head` and `tail` must never be modified directly by the user, and they must be allocated before `vskiplist_init`. They can be freed only after `vskiplist_destroy`. i.e. they must have a life span larger or equal to `lst`. 


##  Function `vskiplist_destroy`

```c
static void vskiplist_destroy(vskiplist_t *lst)
``` 
_Destroys all existing nodes in the skiplist._ 


> **Note:** the destruction is done by calling retire function on all remaining nodes.

> **Note:** call after threads are done accessing the skiplist. 

**Parameters:**

- `lst`: address of vskiplist_t object. 




##  Function `vskiplist_lookup`

```c
static vskiplist_node_t* vskiplist_lookup(vskiplist_t *lst, vskiplist_key_t key)
``` 
_Searches for the skiplist node associated with the given key._ 




**Parameters:**

- `lst`: address of vskiplist_t object. 
- `key`: the key you are looking for. 


**Returns:** vskiplist_node_t* address of the skiplist node associated with the given key if exists. 

**Returns:** NULL if no node associated with the given key was found.

> **Note:** call within SMR critical section. 


##  Function `vskiplist_calc_node_sz`

```c
static vsize_t vskiplist_calc_node_sz(vskiplist_t *lst, vsize_t container_sz, vsize_t *height)
``` 
_Calculates the required size and height of_ `vskiplist_node_t` _container._ 




**Parameters:**

- `lst`: address of vskiplist_t object. 
- `container_sz`: the size of the container object returned by sizeof. 
- `height`: output parameter. Indicates to how many levels the node will be connected to in the skiplist.


**Returns:** vsize_t the minimum required size of the container object of `vskiplist_node_t`.

> **Note:** users must call this function everytime they wish to add a new node to the skiplist. see `vskiplist_add`.

> **Note:** if the node is not embedded in a container struct. Pass `container_sz` as `sizeof(vskiplist_node_t)`. 


##  Function `vskiplist_add`

```c
static vbool_t vskiplist_add(vskiplist_t *lst, vskiplist_key_t key, vskiplist_node_t *node, vskiplist_node_t **out_node, vsize_t height)
``` 
_Inserts_ `node` _into the skiplist._ 




**Parameters:**

- `lst`: address of vskiplist_t object. 
- `key`: the key associated with `node`. 
- `node`: address of vskiplist_node_t object. 
- `out_node`: output parameter contains the address of the existing node associated with the given key, if the key already exists and insertion failed, otherwise it contains NULL. 
- `height`: the value of the output param `height` updated by `vskiplist_calc_node_sz`.


**Returns:** true the key did not exist, and `node` is added successfully. 

**Returns:** false the key already exists, `node` is not added.

> **Note:** call within SMR critical section.

> **Note:** `out_node` can be NULL.

> **Note:** see `vskiplist_calc_node_sz` for container size calculation. 


##  Function `vskiplist_remove`

```c
static vbool_t vskiplist_remove(vskiplist_t *lst, vskiplist_key_t key)
``` 
_Removes the skiplist node associated with the given key._ 




**Parameters:**

- `lst`: address of vskiplist_t object. 
- `key`: the key associated with the node you want to remove. 


**Returns:** true a node associated with the given key has been found and removed. 

**Returns:** false no node associated with the given key was found.

> **Note:** call within SMR critical section. 


##  Function `vskiplist_lookup_next`

```c
static vskiplist_node_t* vskiplist_lookup_next(vskiplist_t *lst, vskiplist_key_t key)
``` 
_Searches for the skiplist node associated with the given key._ 


Similar to `vskiplist_lookup` looks for a node associated with the given `key`, however if no such node is found it returns the address of the node associated with key `k` such that `k > key` and there is no key  `k `such that`key < k' < k`.

> **Note:** extension API 

**Parameters:**

- `lst`: address of vskiplist_t object. 
- `key`: the key you are looking for. 


**Returns:** vskiplist_node_t* the address of the node associated with key if found, otherwise it returns the address of the node associated with `k > key`. If there is no such `k` it returns NULL.

> **Note:** call within SMR critical section. 


##  Function `vskiplist_get_succ`

```c
static vskiplist_node_t* vskiplist_get_succ(vskiplist_t *lst, vskiplist_node_t *curr)
``` 
_Returns the successor of the given node_ `curr` _if a successor exists._ 


> **Note:** a successor satisfies `container(succ)->key > container(curr)->key` and there exists no node n such that `container(succ)->key > container(n)->key <blockquote>container(curr)->key`, otherwise `n` is the successor 

> **Note:** extension API, the user is responsible for making sure that curr points to a valid non-reclaimed node 

**Parameters:**

- `lst`: address of vskiplist_t object. 
- `curr`: address of a skiplist node. 


**Returns:** vskiplist_node_t* address of the successor: if successor exists NULL: if successor does not exist or curr has been removed.

> **Note:** call within the same SMR critical section where `curr` was retrieved. 


##  Function `vskiplist_get_first`

```c
static vskiplist_node_t* vskiplist_get_first(vskiplist_t *lst)
``` 
_Returns the first node in the given_ `lst`_._ 


i.e. the element that is mapped to the smallest key.

> **Note:** nodes that are logically removed (i.e. marked) are treated as nonexistent. 

**Parameters:**

- `lst`: address of vskiplist_t object. 


**Returns:** vskiplist_node_t* address of the first node.

> **Note:** call within SMR critical section. 


##  Function `vskiplist_remove_node`

```c
static vbool_t vskiplist_remove_node(vskiplist_t *lst, vskiplist_key_t key, vskiplist_node_t *node)
``` 
_Removes the given node associated with the given key if it exists._ 


It fails if no node associated with the given key was found, or if the found node is not the same as the given node.

> **Note:** extension API, the user is responsible for making sure that the expected_node `node` is valid and points to a non-reclaimed memory. 

**Parameters:**

- `lst`: address of vskiplist_t object. 
- `key`: the key associated with the node to remove. 
- `node`: address of the node to remove. 


**Returns:** true the node associated with the given key has been removed. 

**Returns:** false there is no node associated with the given key, or the existing node is different than the given node.

> **Note:** call within the same SMR critical section where `node` was retrieved. 


##  Function `vskiplist_cleanup`

```c
static void vskiplist_cleanup(vskiplist_t *lst)
``` 
_Triggers the physical removal of all logically removed nodes._ 


It is a modified copy _vskiplist_find.

> **Note:** this function is experimental and subject to removal. 

**Parameters:**

- `lst`: address of vskiplist_t object.


> **Note:** call within SMR critical section. 



---
