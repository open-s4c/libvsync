#  [vsync](../README.md) / [queue](README.md) / mpsc.h
_Multi-producer single-consumer queue._ 

**Groups:** [Unbounded-Queue](GROUP_unbounded_queue.md)

Enqueue operation is wait-free.


### Example:



```c
#include <vsync/queue/mpsc.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

vmpsc_t g_queue;

#define NUM_PRODUCERS  3U
#define NUM_PROD_ITEMS 10U
#define NUM_CONS_ITEMS (NUM_PRODUCERS * NUM_PROD_ITEMS)

typedef struct data_s {
    vuint32_t x;
    vuint32_t y;
} data_t;

void
destroy_cb(vmpsc_node_t *node, void *args)
{
    free(node->data);
    free(node);
    V_UNUSED(args);
}

void *
consume(void *args)
{
    vmpsc_node_t *n = NULL;
    data_t *d       = NULL;
    for (vuint32_t i = 0; i < NUM_CONS_ITEMS; i++) {
        while (d = vmpsc_deq(&g_queue, &n), d == NULL) {
            /* busy wait till an item is consumed. Repeat on empty! */
        }
        ASSERT(d);
        ASSERT(d->x == d->y);
        printf("consumed item: {%u, %u}\n", d->x, d->y);
        free(d);
        /* n can be NULL even if d is not. */
        free(n);
    }
    V_UNUSED(args);
    return NULL;
}

void *
produce(void *args)
{
    vuint32_t tid = (vuint32_t)(vuintptr_t)args;
    for (vuint32_t i = 0; i < NUM_PROD_ITEMS; i++) {
        data_t *d = malloc(sizeof(*d));
        ASSERT(d);
        d->x = d->y     = (tid * NUM_PRODUCERS) + i;
        vmpsc_node_t *n = malloc(sizeof(*n));
        ASSERT(n);
        vmpsc_enq(&g_queue, n, d);
    }
    V_UNUSED(args);
    return NULL;
}

int
main(void)
{
    vmpsc_init(&g_queue);
    pthread_t consumer;
    pthread_t producer[NUM_PRODUCERS];

    pthread_create(&consumer, NULL, consume, NULL);
    for (vsize_t i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producer[i], NULL, produce, (void *)(vuintptr_t)i);
    }
    for (vsize_t i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producer[i], NULL);
    }
    pthread_join(consumer, NULL);
    vmpsc_destroy(&g_queue, destroy_cb, NULL);
    return 0;
}
```




### References:
 [Non-intrusive MPSC node-based queue](https://www.1024cores.net/home/lock-free-algorithms/queues/non-intrusive-mpsc-node-based-queue) 

---
# Functions 

| Function | Description |
|---|---|
| [vmpsc_init](mpsc.h.md#function-vmpsc_init) | Initializes the given queue object `q`.  |
| [vmpsc_foreach](mpsc.h.md#function-vmpsc_foreach) | Calls the given `f` function on each enqueued node's data.  |
| [vmpsc_destroy](mpsc.h.md#function-vmpsc_destroy) | Calls the given `f` function on each enqueued node.  |
| [vmpsc_enq](mpsc.h.md#function-vmpsc_enq) | Enqueues the given node into the given queue.  |
| [vmpsc_deq](mpsc.h.md#function-vmpsc_deq) | Dequeues a node and data object from the queue.  |
| [vmpsc_empty](mpsc.h.md#function-vmpsc_empty) | Checks if the current queue is empty or not.  |
| [vmpsc_length](mpsc.h.md#function-vmpsc_length) | Calculates how many nodes are enqueued in the queue.  |

##  Function `vmpsc_init`

```c
static void vmpsc_init(vmpsc_t *q)
``` 
_Initializes the given queue object_ `q`_._ 




**Parameters:**

- `q`: address of vmpsc_t object.


> **Note:** call only before threads start accessing the queue. 


##  Function `vmpsc_foreach`

```c
static void vmpsc_foreach(const vmpsc_t *q, vmpsc_data_visit_t f, void *args)
``` 
_Calls the given_ `f` _function on each enqueued node's data._ 




**Parameters:**

- `q`: address of vmpsc_t object. 
- `f`: function pointer of type vmpsc_data_visit_t. 
- `args`: extra arguments of `f`.


> **Note:** Don't call while producer threads are active. 


##  Function `vmpsc_destroy`

```c
static void vmpsc_destroy(vmpsc_t *q, vmpsc_visit_t f, void *args)
``` 
_Calls the given_ `f` _function on each enqueued node._ 




**Parameters:**

- `q`: address of vmpsc_t object. 
- `f`: function pointer of type vmpsc_visit_t. 
- `args`: extra arguments of `f`.


> **Note:** call only after all threads are done accessing the queue.

> **Note:** use the callback function to destruct nodes and data objects. 


##  Function `vmpsc_enq`

```c
static void vmpsc_enq(vmpsc_t *q, vmpsc_node_t *node, void *data)
``` 
_Enqueues the given node into the given queue._ 




**Parameters:**

- `q`: address of vmpsc_t object. 
- `node`: address of vmpsc_node_t object to enqueue. 
- `data`: address of data object to be associated with the given node. 




##  Function `vmpsc_deq`

```c
static void* vmpsc_deq(vmpsc_t *q, vmpsc_node_t **discarded_node)
``` 
_Dequeues a node and data object from the queue._ 




**Parameters:**

- `q`: address of vmpsc_t object. 
- `discarded_node`: output parameter. Address of vmpsc_node_t sentinel object to destroy, or NULL.


**Returns:** void* address of dequeued data object. 

**Returns:** NULL when the queue is empty.

> **Note:** on each successful dequeue there will be a sentinel node object kicked out of the queue. 


##  Function `vmpsc_empty`

```c
static vbool_t vmpsc_empty(vmpsc_t *q)
``` 
_Checks if the current queue is empty or not._ 




**Parameters:**

- `q`: address of vmpsc_t object.


**Returns:** true the queue is empty. 

**Returns:** false the queue is not empty.

> **Note:** to be called only by the consumer. 


##  Function `vmpsc_length`

```c
static vsize_t vmpsc_length(vmpsc_t *q)
``` 
_Calculates how many nodes are enqueued in the queue._ 




**Parameters:**

- `q`: address of vmpsc_t object. 


**Returns:** vsize_t number of available elements in the queue.

> **Note:** to be called only by the consumer.

> **Note:** only non-sentinel nodes are counted. 



---
