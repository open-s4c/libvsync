#  [vsync](../README.md) / [queue](README.md) / unbounded_queue_total.h
_Unbounded blocking total queue._ 

**Groups:** [Linearizable](../GROUP_linearizable.md), [Unbounded-Queue](GROUP_unbounded_queue.md)

Consumers can run in parallel to producers. Consumers block other consumers, but not producers and vice versa.


### Example:



```c
#include <vsync/queue/unbounded_queue_total.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#define IT 10000
#define N  12

typedef struct data_s {
    vsize_t id;
} data_t;

vqueue_ub_t g_queue;

void
destroy_cb(vqueue_ub_node_t *qnode, void *args)
{
    free(qnode);
    (void)args;
}

void *
run(void *args)
{
    data_t *data = NULL;
    vsize_t tid  = (vsize_t)args;

    for (vsize_t i = 0; i < IT; i++) {
        data = vqueue_ub_deq(&g_queue, destroy_cb, NULL);
        if (data == NULL) {
            data                    = malloc(sizeof(data_t));
            data->id                = i;
            vqueue_ub_node_t *qnode = malloc(sizeof(vqueue_ub_t));
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
    return 0;
}
```




### References:

Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 10.4](https://dl.acm.org/doi/pdf/10.5555/2385452) 

---
# Functions 

| Function | Description |
|---|---|
| [vqueue_ub_init](unbounded_queue_total.h.md#function-vqueue_ub_init) | PUBLIC FUNCTIONS.  |
| [vqueue_ub_enq](unbounded_queue_total.h.md#function-vqueue_ub_enq) | Enqueue the given node `qnode` in the given queue `q`.  |
| [vqueue_ub_get_length](unbounded_queue_total.h.md#function-vqueue_ub_get_length) | Returns the length of the queue.  |
| [vqueue_ub_deq](unbounded_queue_total.h.md#function-vqueue_ub_deq) | Dequeue a node from the given queue `q`.  |
| [vqueue_ub_destroy](unbounded_queue_total.h.md#function-vqueue_ub_destroy) | Destroys all remaining nodes in the queue.  |
| [vqueue_ub_empty](unbounded_queue_total.h.md#function-vqueue_ub_empty) | Check if the queue is empty.  |

##  Function `vqueue_ub_init`

```c
static void vqueue_ub_init(vqueue_ub_t *q)
``` 
_PUBLIC FUNCTIONS._ 


Initialize queue.

> **Note:** must be called before threads start operating on the queue 

**Parameters:**

- `q`: address of vqueue_ub_t object 




##  Function `vqueue_ub_enq`

```c
static void vqueue_ub_enq(vqueue_ub_t *q, vqueue_ub_node_t *qnode, void *data)
``` 
_Enqueue the given node_ `qnode` _in the given queue_ `q`_._ 




**Parameters:**

- `q`: address of vqueue_ub_t object 
- `qnode`: address of vqueue_ub_node_t 
- `data`: address of data object to enqueue. 




##  Function `vqueue_ub_get_length`

```c
static vsize_t vqueue_ub_get_length(vqueue_ub_t *q)
``` 
_Returns the length of the queue._ 


> **Note:** this function will block both dequeues and enqueues 

**Parameters:**

- `q`: address of vqueue_ub_t object 


**Returns:** vsize_t the length of the queue 



##  Function `vqueue_ub_deq`

```c
static void * vqueue_ub_deq(vqueue_ub_t *q, vqueue_ub_node_handler_t retire, void *retire_arg)
``` 
_Dequeue a node from the given queue_ `q`_._ 




**Parameters:**

- `q`: address of vqueue_ub_t object. 
- `retire`: function pointer of the callback that frees the detached `vqueue_ub_node_t`. 
- `arg`: extra parameter passed to `retire`. 


**Returns:** NULL when the queue is empty. 

**Returns:** a non NULL address of the dequeued data object. 



##  Function `vqueue_ub_destroy`

```c
static void vqueue_ub_destroy(vqueue_ub_t *q, vqueue_ub_node_handler_t retire, void *retire_arg)
``` 
_Destroys all remaining nodes in the queue._ 


> **Note:** call only after all threads finished operating on the queue.

> **Note:** NOT THREAD-SAFE. 

**Parameters:**

- `q`: address of vqueue_ub_t object. 
- `retire`: function pointer of the callback that handles freeing the remaining nodes. 
- `arg`: extra parameter passed to `retire`. 




##  Function `vqueue_ub_empty`

```c
static vbool_t vqueue_ub_empty(vqueue_ub_t *q)
``` 
_Check if the queue is empty._ 




**Parameters:**

- `q`: address of vqueue_ub_t object 


**Returns:** true the queue is empty 

**Returns:** false the queue is not empty 




---
