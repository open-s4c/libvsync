#  [vsync](../README.md) / [queue](README.md) / unbounded_queue_lf_recycle.h
_Lock-free recycle unbounded queue._ 

**Groups:** [Linearizable](../GROUP_linearizable.md), [Lock-free](../GROUP_lock_free.md), [Unbounded-Queue](GROUP_unbounded_queue.md)

> **Note:** Retired Nodes of this queue **cannot** be freed, but can be recycled This queue makes sure that the occurrence of ABA problem is improbable, by using the 7 LSB bits of nodes addresses as a stamp. `cas` can succeed iff the stamps match.

> **Note:** `vqueue_ub_node_t` objects must be 128byte aligned (i.e. the 7 LSB of their addresses are always zeros). i.e. allocate using `memalign`

> **Note:** maintain a thread local free node list that you place retired nodes on and reuse them for new allocations, unless the list is empty


### Example:



```c

#include <vsync/queue/unbounded_queue_lf_recycle.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#define IT 100
#define N  12

/* this is just a proof of concept and not an optimal. Each thread shall have a
 * its own pool */
vqueue_ub_node_t *local_pools[N] = {0};

typedef struct data_s {
    vsize_t id;
} data_t;

vqueue_ub_t g_queue;

void
destroy_pools(void)
{
    vqueue_ub_node_t *curr = NULL;
    vqueue_ub_node_t *succ = NULL;

    for (vsize_t i = 0; i < N; i++) {
        curr = local_pools[i];
        while (curr) {
            succ = vatomicptr_stamped_get_pointer(&curr->next);
            free(curr);
            curr = succ;
        }
    }
}

void
destroy_cb(vqueue_ub_node_t *qnode, void *args)
{
    /* Nodes can be freed only after queue destruction */
    free(qnode);
    (void)args;
}

void
retire_cb(vqueue_ub_node_t *qnode, void *args)
{
    vsize_t tid = (vsize_t)args;
    /* @note: Users should not rely on the qnode internal fields, they should
     * have an independent pool. The following is just a proof of concept. */
    /* retired nodes are added to the local pool */
    vuint8_t current_stamp = 0;
    (void)vatomicptr_stamped_get(&qnode->next, &current_stamp);

    /* connect the retired node to the local pool */
    vatomicptr_stamped_set(&qnode->next, local_pools[tid], current_stamp + 1);
    local_pools[tid] = qnode;
    (void)args;
}

vqueue_ub_node_t *
alloc_qnode(vsize_t tid)
{
    vqueue_ub_node_t *qnode = NULL;

    if (local_pools[tid]) {
        qnode = local_pools[tid];
        /* if the local pool is not empty use a node from there */
        local_pools[tid] =
            vatomicptr_stamped_get_pointer(&local_pools[tid]->next);
        printf("Recycle allocation\n");
    } else {
        /* otherwise allocated an aligned node */
        qnode =
            memalign(RECYCLE_QUEUE_NODE_ALIGNMENT, sizeof(vqueue_ub_node_t));
        printf("New allocation\n");
    }
    assert(qnode);
    return qnode;
}

void *
run(void *args)
{
    data_t *data = NULL;
    vsize_t tid  = (vsize_t)args;

    for (vsize_t i = 0; i < IT; i++) {
        data = vqueue_ub_deq(&g_queue, retire_cb, (void *)tid);
        if (data == NULL) {
            data                    = malloc(sizeof(data_t));
            data->id                = i;
            vqueue_ub_node_t *qnode = alloc_qnode(tid);
            printf("[T%zu] enq %zu\n", tid, data->id);
            vqueue_ub_enq(&g_queue, qnode, data);
        } else {
            printf("[T%zu] deq %zu\n", tid, data->id);
            free(data);
        }
    }

    return NULL;
}

int
main(void)
{
    pthread_t threads[N];
    data_t *data = NULL;

    vqueue_ub_init(&g_queue);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    /* dequeue all remaining nodes, to be able to destroy data */
    while (data = vqueue_ub_deq(&g_queue, destroy_cb, NULL), data) {
        free(data);
    }
    /* destroy the queue to destroy the remaining sentinel */
    vqueue_ub_destroy(&g_queue, destroy_cb, NULL);

    /* destroy local pools */
    destroy_pools();
    return 0;
}
```



### Operating conditions


- `vqueue_ub_node_t` objects must be 128byte aligned
- Nodes cannot be freed, they can only be recycled


### Recycling tips


- maintain local pool per thread
- when a thread retires a node, add the node to the local pool
- when a thread wants to allocate a new node fetch a node from the local unless it is not empty. If it is empty, allocate it with your normal allocator e.g. memalign> **Note:** the above suggestions works well for threads that perform roughly the same number of enqueues and dequeues. If that is not the case, a thread needs a strategy of stealing nodes from other pools.
### References:


Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 10.6](https://dl.acm.org/doi/pdf/10.5555/2385452) 

---
# Functions 

| Function | Description |
|---|---|
| [vqueue_ub_init](unbounded_queue_lf_recycle.h.md#function-vqueue_ub_init) | Initialize the queue.  |
| [vqueue_ub_destroy](unbounded_queue_lf_recycle.h.md#function-vqueue_ub_destroy) | Destroys all remaining nodes in the queue.  |
| [vqueue_ub_enq](unbounded_queue_lf_recycle.h.md#function-vqueue_ub_enq) | Enqueue the given node `qnode` in the given queue `q`.  |
| [vqueue_ub_empty](unbounded_queue_lf_recycle.h.md#function-vqueue_ub_empty) | Check if the queue is empty.  |
| [vqueue_ub_deq](unbounded_queue_lf_recycle.h.md#function-vqueue_ub_deq) | Dequeue a node from the given queue `q`.  |
| [vqueue_ub_get_length](unbounded_queue_lf_recycle.h.md#function-vqueue_ub_get_length) | Iterate through the queue nodes and count them.  |

##  Function `vqueue_ub_init`

```c
static void vqueue_ub_init(vqueue_ub_t *q)
``` 
_Initialize the queue._ 


> **Note:** Function must be called before threads start operating on the queue.



**Parameters:**

- `q`: address of vqueue_ub_t object 




##  Function `vqueue_ub_destroy`

```c
static void vqueue_ub_destroy(vqueue_ub_t *q, vqueue_ub_node_handler_t retire, void *arg)
``` 
_Destroys all remaining nodes in the queue._ 


> **Note:** call only after all threads finished operating on the queue.

> **Note:** NOT THREAD-SAFE.



**Parameters:**

- `q`: address of vqueue_ub_t object 
- `retire`: function pointer of the callback that handles recycling/freeing the remaining nodes. 
- `arg`: extra parameter passed to `retire`. 




##  Function `vqueue_ub_enq`

```c
static void vqueue_ub_enq(vqueue_ub_t *q, vqueue_ub_node_t *qnode, void *data)
``` 
_Enqueue the given node_ `qnode` _in the given queue_ `q`_._ 




**Parameters:**

- `q`: address of vqueue_ub_t object 
- `qnode`: address of vqueue_ub_node_t 
- `data`: address of data object to enqueue. 




##  Function `vqueue_ub_empty`

```c
static vbool_t vqueue_ub_empty(vqueue_ub_t *q)
``` 
_Check if the queue is empty._ 


> **Note:** Use function with caution. It is subject to ABA problems and might return empty when the queue is not empty.



**Parameters:**

- `q`: address of vqueue_ub_t object 


**Returns:** true the queue is empty 

**Returns:** false the queue is not empty 



##  Function `vqueue_ub_deq`

```c
static void* vqueue_ub_deq(vqueue_ub_t *q, vqueue_ub_node_handler_t retire, void *arg)
``` 
_Dequeue a node from the given queue_ `q`_._ 




**Parameters:**

- `q`: address of vqueue_ub_t object 
- `retire`: function pointer of the callback that recycles a detached `vqueue_ub_node_t`. 
- `arg`: extra parameter passed to `retire`. 


**Returns:** NULL when the queue is empty. 

**Returns:** a non NULL address of the dequeued data object. 



##  Function `vqueue_ub_get_length`

```c
static vsize_t vqueue_ub_get_length(vqueue_ub_t *q)
``` 
_Iterate through the queue nodes and count them._ 


> **Note:** returns an atomic snapshot of the length, which may be stale



**Parameters:**

- `q`: address of vqueue_ub_t object 


**Returns:** vsize_t the length of the queue 




---
