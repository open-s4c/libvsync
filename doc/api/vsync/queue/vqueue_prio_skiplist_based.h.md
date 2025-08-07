#  [vsync](../README.md) / [queue](README.md) / vqueue_prio_skiplist_based.h
_Skiplist-based priority queue implementation._ 

**Groups:** [Linearizable](../GROUP_linearizable.md), [Lock-free](../GROUP_lock_free.md), [SMR-required](../GROUP_requires_smr.md), [Unbounded-Queue](GROUP_unbounded_queue.md)

This implementation uses a lock-free skiplist (see [skiplist_lf.h](../map/skiplist_lf.h.md)) to implement the priority queue. It has unbounded priority range and unbounded capacity.

It requires SMR (see gdump.h)

### Configuration

Check [skiplist_lf.h](../map/skiplist_lf.h.md) for configuring the number of levels of the skiplist.


### Example:



```c
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
```




### References:

Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 15.5](https://dl.acm.org/doi/pdf/10.5555/2385452) 

---
# Functions 

| Function | Description |
|---|---|
| [vqueue_prio_init](vqueue_prio_skiplist_based.h.md#function-vqueue_prio_init) | Initializes the queue.  |
| [vqueue_prio_destroy](vqueue_prio_skiplist_based.h.md#function-vqueue_prio_destroy) | Calls the given callback on all remaining nodes.  |
| [vqueue_prio_calc_node_sz](vqueue_prio_skiplist_based.h.md#function-vqueue_prio_calc_node_sz) | Calculates the required size and height of `vqueue_prio_node_t` container.  |
| [vqueue_prio_add](vqueue_prio_skiplist_based.h.md#function-vqueue_prio_add) | Enqueues the given `node`.  |
| [vqueue_prio_remove_min](vqueue_prio_skiplist_based.h.md#function-vqueue_prio_remove_min) | Dequeues a node with highest priority available.  |

##  Function `vqueue_prio_init`

```c
static void vqueue_prio_init(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t retire_cb, void *retire_cb_arg, vuint32_t rand_seed, vqueue_prio_sentinel_t *head, vqueue_prio_sentinel_t *tail)
``` 
_Initializes the queue._ 




**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 
- `retire_cb`: callback function used for retiring detached nodes to the SMR. 
- `retire_cb_arg`: second parameter of `retire_cb`. 
- `rand_seed`: passed down to the skiplist for random height generation.
- `head`: the address of the dynamically allocated head sentinel object of size `VQUEUE_PRIO_SENTINEL_SZ`. 
- `tail`: the address of the dynamically allocated tail sentinel object of size `VQUEUE_PRIO_SENTINEL_SZ`.


> **Note:** both `head` and `tail` must have size `VQUEUE_PRIO_SENTINEL_SZ`.

> **Note:** `head` and `tail` must never be modified directly by the user, and they must be allocated before `vqueue_prio_init`. They can be freed only after `vqueue_prio_destroy`. i.e. they must have a life span larger or equal to `pqueue`. 


##  Function `vqueue_prio_destroy`

```c
static void vqueue_prio_destroy(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t destroy_cb, void *args)
``` 
_Calls the given callback on all remaining nodes._ 


Nodes can be freed in `destroy_cb`

> **Note:** this is not thread safe and must be called if and only if all threads are done accessing the queue



**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 
- `destroy_cb`: address of a callback function to call on each remaining node. 
- `args`: second argument of `destroy_cb`. 




##  Function `vqueue_prio_calc_node_sz`

```c
static vsize_t vqueue_prio_calc_node_sz(vqueue_prio_t *pqueue, vsize_t container_sz, vsize_t *height)
``` 
_Calculates the required size and height of_ `vqueue_prio_node_t` _container._ 




**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 
- `container_sz`: the size of the container object returned by sizeof. 
- `height`: output parameter. Indicates to how many levels the node will be connected to in the skiplist.


**Returns:** vsize_t the minimum required size of the container object of `vqueue_prio_node_t`.

> **Note:** users must call this function everytime they wish to add a new node to the priority queue. see `vqueue_prio_add`.

> **Note:** if the queue node is not embedded in a container struct. Pass `container_sz` as `sizeof(vqueue_prio_node_t)`. 


##  Function `vqueue_prio_add`

```c
static void vqueue_prio_add(vqueue_prio_t *pqueue, vqueue_prio_node_t *node, vuint32_t priority, vsize_t height)
``` 
_Enqueues the given_ `node`_._ 




**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 
- `node`: address of vqueue_prio_node_t object. 
- `priority`: the priority associated with `node`. 
- `height`: the value of the output param `height` updated by `vqueue_prio_calc_node_sz`.


> **Note:** see `vqueue_prio_calc_node_sz` for container size calculation. 


##  Function `vqueue_prio_remove_min`

```c
static vqueue_prio_node_t* vqueue_prio_remove_min(vqueue_prio_t *pqueue)
``` 
_Dequeues a node with highest priority available._ 




**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 


**Returns:** vqueue_prio_node_t* address of dequeued object. 




---
