#  [vsync](../README.md) / [queue](README.md) / vqueue_prio_stack_array_based.h
_Array-based bounded priority queue._ 

**Groups:** [Linearizable](../GROUP_linearizable.md), [Lock-free](../GROUP_lock_free.md), [SMR-required](../GROUP_requires_smr.md), [Unbounded-Queue](GROUP_unbounded_queue.md)

This implementation is an array of stacks (see [elimination_stack.h](../stack/elimination_stack.h.md)). Each stack is associated with a priority equals to the array index of the slot that the stack occupies. It has bounded priority range and unbounded capacity.

It requires SMR (see gdump.h)

> **Note:** A different type of stack can be used e.g., [xbo_stack.h](../stack/xbo_stack.h.md), or a stack without a backoff mechanism, which might be better suited to low contention scenarios. Contact the authors, if you need to use different type of stack.


### Example:



```c
#include <vsync/queue/vqueue_prio_stack_array_based.h>
#include <vsync/smr/gdump.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define IT 2
#define N  3

typedef struct data_s {
    vqueue_prio_node_t qnode;
    smr_node_t smr_node;
    vsize_t id;
} data_t;

gdump_t g_gdump;
vqueue_prio_t g_queue;
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
    data_t *data = (data_t *)node;
    gdump_retire(&g_gdump, &data->smr_node, free_cb, NULL);
    (void)args;
}

void
destroy_cb(vqueue_prio_node_t *node, void *args)
{
    data_t *data = (data_t *)node;
    free(data);
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
            data = (data_t *)vqueue_prio_remove_min(&g_queue);
            if (data == NULL) {
                data     = malloc(sizeof(data_t));
                data->id = i;
                vqueue_prio_add(&g_queue, &data->qnode, (vuint32_t)i);
                printf("[T%zu] enq %zu\n", tid, data->id);
            } else {
                printf("[T%zu] deq %zu\n", tid, data->id);
            }
            gdump_exit(&g_gdump, &thread);
        }

        gdump_deregister(&g_gdump, &thread);
    }
    return NULL;
}

static __thread unsigned int g_thread_seed = 0;
vuint32_t
rand_cb(vuint32_t min, vuint32_t max)
{
    if (g_thread_seed == 0) {
        g_thread_seed = time(NULL);
    }
    int r = rand_r(&g_thread_seed);
    if (r < 0) {
        r *= -1;
    }
    return (((vuint32_t)r) % (max - min + 1)) + min;
}

int
main(void)
{
    pthread_t threads[N];

    int ret = pthread_rwlock_init(&g_lock, NULL);
    assert(ret == 0);

    gdump_init(&g_gdump, g_rwlock_lib);

    vqueue_prio_init(&g_queue, retire_cb, NULL, rand_cb);

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

    ret = pthread_rwlock_destroy(&g_lock);
    assert(ret == 0);

    return 0;
}
```




### References:

Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 15.2](https://dl.acm.org/doi/pdf/10.5555/2385452) 

---
# Macros 

| Macro | Description |
|---|---|
| [VQUEUE_PRIO_PRIORITY_RANGE](vqueue_prio_stack_array_based.h.md#macro-vqueue_prio_priority_range) | `VQUEUE_PRIO_PRIORITY_RANGE` configures the queue to accept priorities from [0:VQUEUE_PRIO_PRIORITY_RANGE-1]  |

##  Macro `VQUEUE_PRIO_PRIORITY_RANGE`

 
`VQUEUE_PRIO_PRIORITY_RANGE` _configures the queue to accept priorities from [0:VQUEUE_PRIO_PRIORITY_RANGE-1]_ 


> **Note:** `VQUEUE_PRIO_PRIORITY_RANGE` must be power of two. 


---
# Functions 

| Function | Description |
|---|---|
| [vqueue_prio_init](vqueue_prio_stack_array_based.h.md#function-vqueue_prio_init) | Initializes the queue.  |
| [vqueue_prio_destroy](vqueue_prio_stack_array_based.h.md#function-vqueue_prio_destroy) | Calls the given callback on all remaining nodes.  |
| [vqueue_prio_add](vqueue_prio_stack_array_based.h.md#function-vqueue_prio_add) | Enqueues the given `node`.  |
| [vqueue_prio_remove_min](vqueue_prio_stack_array_based.h.md#function-vqueue_prio_remove_min) | Dequeues a node with highest priority available.  |

##  Function `vqueue_prio_init`

```c
static void vqueue_prio_init(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t retire_fun, void *retire_fun_arg, backoff_rand_fun_t rand_fun)
``` 
_Initializes the queue._ 




**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 
- `retire_cb`: callback function used for retiring detached nodes to the SMR. 
- `retire_cb_arg`: second parameter of `retire_cb`. 
- `rand_fun`: a function pointer to a function that generates a random number. 




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




##  Function `vqueue_prio_add`

```c
static void vqueue_prio_add(vqueue_prio_t *pqueue, vqueue_prio_node_t *node, vsize_t priority)
``` 
_Enqueues the given_ `node`_._ 




**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 
- `node`: address of vqueue_prio_node_t object. 
- `priority`: the priority associated with `node`. 




##  Function `vqueue_prio_remove_min`

```c
static vqueue_prio_node_t* vqueue_prio_remove_min(vqueue_prio_t *pqueue)
``` 
_Dequeues a node with highest priority available._ 




**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 


**Returns:** vqueue_prio_node_t* address of dequeued object. 




---
