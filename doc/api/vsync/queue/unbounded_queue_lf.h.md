#  [vsync](../README.md) / [queue](README.md) / unbounded_queue_lf.h
_Lock-free unbounded queue._ 

**Groups:** [Linearizable](../GROUP_linearizable.md), [Lock-free](../GROUP_lock_free.md), [SMR-required](../GROUP_requires_smr.md), [Unbounded-Queue](GROUP_unbounded_queue.md)

Operating conditions
- This queue requires the presence of an SMR scheme vsync/smr e.g. vsync/smr/gdump.h
- Concurrent Queue operations must be called inside the critical section of SMR.
- It is recommended to dedicate a special thread for SMR recycling, which operates in the background and periodically attempts to recycle retired nodes.
- Detached/retired nodes **must be retired** to the SMR and not freed directly.
### Example:




```c

#include <vsync/queue/unbounded_queue_lf.h>
/* define VGDUMP_ENABLE_BUFF to enable thread local buffer of retired nodes */
#define VGDUMP_ENABLE_BUFF
#include <vsync/smr/gdump.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define IT 10000
#define N  12

typedef struct data_s {
    vsize_t id;
} data_t;

gdump_t g_gdump;
vqueue_ub_t g_queue;
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
    vqueue_ub_node_t *qnode = V_CONTAINER_OF(node, vqueue_ub_node_t, smr_node);
    free(qnode);
    (void)args;
}

void
retire_cb(vqueue_ub_node_t *qnode, void *args)
{
    gdump_thread_t *thrd = args;
    gdump_retire_local(&g_gdump, thrd, &qnode->smr_node, free_cb, NULL);
}

void
destroy_cb(vqueue_ub_node_t *qnode, void *args)
{
    free(qnode);
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

            data = vqueue_ub_deq(&g_queue, retire_cb, &thread);
            if (data == NULL) {
                data                    = malloc(sizeof(data_t));
                data->id                = i;
                vqueue_ub_node_t *qnode = malloc(sizeof(vqueue_ub_node_t));
                printf("[T%zu] enq %zu\n", tid, data->id);
                vqueue_ub_enq(&g_queue, qnode, data);
            } else {
                printf("[T%zu] deq %zu\n", tid, data->id);
                free(data);
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
    data_t *data = NULL;

    int ret = pthread_rwlock_init(&g_lock, NULL);
    assert(ret == 0);

    gdump_init(&g_gdump, g_rwlock_lib);
    vqueue_ub_init(&g_queue);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 1; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    vatomic8_write(&g_stop, 1);
    pthread_join(threads[0], NULL);

    /* dequeue all remaining nodes, to be able to destroy data */
    while (data = vqueue_ub_deq(&g_queue, destroy_cb, NULL), data) {
        free(data);
    }

    /* destroy the queue to destroy the remaining sentinel */
    vqueue_ub_destroy(&g_queue, destroy_cb, NULL);

    gdump_destroy(&g_gdump);

    ret = pthread_rwlock_destroy(&g_lock);
    assert(ret == 0);
    return 0;
}
```




### References:

Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 10.5](https://dl.acm.org/doi/pdf/10.5555/2385452) 

---
# Functions 

| Function | Description |
|---|---|
| [vqueue_ub_init](unbounded_queue_lf.h.md#function-vqueue_ub_init) | Initializes the queue.  |
| [vqueue_ub_destroy](unbounded_queue_lf.h.md#function-vqueue_ub_destroy) | Destroys all remaining nodes in the queue.  |
| [vqueue_ub_enq](unbounded_queue_lf.h.md#function-vqueue_ub_enq) | Enqueues the given node `qnode` in the given queue `q`.  |
| [vqueue_ub_deq](unbounded_queue_lf.h.md#function-vqueue_ub_deq) | Dequeues a node from the given queue `q`.  |
| [vqueue_ub_get_length](unbounded_queue_lf.h.md#function-vqueue_ub_get_length) | Returns the length of the queue.  |
| [vqueue_ub_empty](unbounded_queue_lf.h.md#function-vqueue_ub_empty) | Checks if the queue is empty.  |

##  Function `vqueue_ub_init`

```c
static void vqueue_ub_init(vqueue_ub_t *q)
``` 
_Initializes the queue._ 


> **Note:** must be called before threads start operating on the queue.

> **Note:** NOT THREAD-SAFE. 

**Parameters:**

- `q`: address of vqueue_ub_t object. 




##  Function `vqueue_ub_destroy`

```c
static void vqueue_ub_destroy(vqueue_ub_t *q, vqueue_ub_node_handler_t retire, void *arg)
``` 
_Destroys all remaining nodes in the queue._ 


> **Note:** call only after all threads finished operating on the queue.

> **Note:** NOT THREAD-SAFE. 

**Parameters:**

- `q`: address of vqueue_ub_t object. 
- `retire`: function pointer of the callback that handles retiring/freeing the remaining nodes. 
- `arg`: extra parameter passed to `retire`. 




##  Function `vqueue_ub_enq`

```c
static void vqueue_ub_enq(vqueue_ub_t *q, vqueue_ub_node_t *qnode, void *data)
``` 
_Enqueues the given node_ `qnode` _in the given queue_ `q`_._ 


> **Note:** call in an SMR critical section. 

**Parameters:**

- `q`: address of vqueue_ub_t object. 
- `qnode`: address of vqueue_ub_node_t. 
- `data`: address of data object to enqueue. 




##  Function `vqueue_ub_deq`

```c
static void* vqueue_ub_deq(vqueue_ub_t *q, vqueue_ub_node_handler_t retire, void *retire_arg)
``` 
_Dequeues a node from the given queue_ `q`_._ 


> **Note:** call in an SMR critical section. 

**Parameters:**

- `q`: address of vqueue_ub_t object. 
- `retire`: function pointer of the callback that retires a detached `vqueue_ub_node_t` object to the SMR. 
- `arg`: extra parameter passed to `retire`. 


**Returns:** NULL when the queue is empty. 

**Returns:** a non NULL address of the dequeued data object. 



##  Function `vqueue_ub_get_length`

```c
static vsize_t vqueue_ub_get_length(vqueue_ub_t *q)
``` 
_Returns the length of the queue._ 


> **Note:** call in an SMR critical section.

> **Note:** returns an atomic snapshot of the length, which may be stale. 

**Parameters:**

- `q`: address of vqueue_ub_t object. 


**Returns:** vsize_t the length of the queue. 



##  Function `vqueue_ub_empty`

```c
static vbool_t vqueue_ub_empty(vqueue_ub_t *q)
``` 
_Checks if the queue is empty._ 


> **Note:** Use with caution this is subject to ABA problems, might return empty when the queue is not empty.

> **Note:** can be called outside the SMR critical section. 

**Parameters:**

- `q`: address of vqueue_ub_t object 


**Returns:** true the queue is empty 

**Returns:** false the queue is not empty 




---
