#  [vsync](../README.md) / [queue](README.md) / bounded_spsc.h
_Single-producer, single-consumer, wait-free bounded queue._ 

**Groups:** [Linearizable](../GROUP_linearizable.md), [Lock-free](../GROUP_lock_free.md)

Implementation of the classic single-producer, single-consumer, wait-free bounded queue algorithm.


### Example:



```c
#include <vsync/queue/bounded_spsc.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#define IT      10
#define BUFF_SZ 1000

void *g_buffer[BUFF_SZ];

bounded_spsc_t g_queue;

typedef struct data_s {
    vsize_t id;
} data_t;

void *
producer(void *args)
{
    data_t *data         = NULL;
    bounded_ret_t result = 0;
    for (vsize_t i = 0; i < IT; i++) {
        data     = malloc(sizeof(data_t));
        data->id = i;
        result   = bounded_spsc_enq(&g_queue, data);
        if (result == QUEUE_BOUNDED_OK) {
            printf("[producer] enq %zu\n", data->id);
        } else {
            // queue is full
            free(data);
        }
    }

    (void)args;
    return NULL;
}

void *
consumer(void *args)
{
    void *data           = NULL;
    bounded_ret_t result = 0;
    for (vsize_t i = 0; i < IT; i++) {
        result = bounded_spsc_deq(&g_queue, &data);
        if (result == QUEUE_BOUNDED_OK) {
            printf("[consumer] deq %zu\n", ((data_t *)data)->id);
            free(data);
        }
    }

    (void)args;
    return NULL;
}

int
main(void)
{
    void *data = NULL;

    pthread_t t_consumer;
    pthread_t t_producer;

    bounded_spsc_init(&g_queue, g_buffer, BUFF_SZ);

    pthread_create(&t_producer, NULL, producer, NULL);
    pthread_create(&t_consumer, NULL, consumer, NULL);

    pthread_join(t_consumer, NULL);
    pthread_join(t_producer, NULL);

    while (bounded_spsc_deq(&g_queue, &data) != QUEUE_BOUNDED_EMPTY) {
        free(data);
    }
    return 0;
}
```




### References:

L. Lamport. - [Specifying concurrent program modules](ACM Transactions on Programming Languages and Systems, 5(2):190-222, 1983.) 

---
# Functions 

| Function | Description |
|---|---|
| [bounded_spsc_init](bounded_spsc.h.md#function-bounded_spsc_init) | Initializes the given queue object.  |
| [bounded_spsc_enq](bounded_spsc.h.md#function-bounded_spsc_enq) | Tries to enqueue a value.  |
| [bounded_spsc_deq](bounded_spsc.h.md#function-bounded_spsc_deq) | Tries to dequeue a value.  |

##  Function `bounded_spsc_init`

```c
static void bounded_spsc_init(bounded_spsc_t *q, void **b, vuint32_t s)
``` 
_Initializes the given queue object._ 




**Parameters:**

- `q`: address of [bounded_spsc_t](structbounded__spsc__t) object. 
- `b`: address of an array of void* slots with size `s`. 
- `s`: number of elements in array b (does not have to be power of 2). 




##  Function `bounded_spsc_enq`

```c
static bounded_ret_t bounded_spsc_enq(bounded_spsc_t *q, void *v)
``` 
_Tries to enqueue a value._ 




**Parameters:**

- `q`: address of [bounded_spsc_t](structbounded__spsc__t) object. 
- `v`: address of object to enqueue.


**Returns:** QUEUE_BOUNDED_OK if successful. 

**Returns:** QUEUE_BOUNDED_FULL if queue is full. 



##  Function `bounded_spsc_deq`

```c
static bounded_ret_t bounded_spsc_deq(bounded_spsc_t *q, void **v)
``` 
_Tries to dequeue a value._ 




**Parameters:**

- `q`: address of [bounded_spsc_t](structbounded__spsc__t) object. 
- `v`: output parameter of type (void**). Contains the address of the dequeued object, if the queue is not empty.


**Returns:** QUEUE_BOUNDED_OK if successful. 

**Returns:** QUEUE_BOUNDED_EMPTY if queue is empty. 




---
