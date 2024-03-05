#  [vsync](../README.md) / [queue](README.md) / bounded_mpmc.h
_Lockless, multi-producer, multi-consumer bounded queue._ 

**Groups:** [Linearizable](../GROUP_linearizable.md)

The queue has a bounded size and returns errors in case the queue is full, empty, or if there is contention while trying to reserve an entry to enqueue or dequeue.


### Example:



```c

#include <vsync/queue/bounded_mpmc.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#define IT      10
#define N       12
#define BUFF_SZ 1000

void *g_buffer[BUFF_SZ];
bounded_mpmc_t g_queue;

typedef struct data_s {
    vsize_t id;
} data_t;

void
produce(vsize_t tid)
{
    data_t *data         = NULL;
    bounded_ret_t result = 0;
    for (vsize_t i = 0; i < IT; i++) {
        data     = malloc(sizeof(data_t));
        data->id = i;
        result   = bounded_mpmc_enq(&g_queue, data);
        if (result == QUEUE_BOUNDED_OK) {
            printf("[T%zu] enq %zu\n", tid, data->id);
        } else {
            // either full/ busy
            free(data);
        }
    }
}

void
consume(vsize_t tid)
{
    void *data           = NULL;
    bounded_ret_t result = 0;
    for (vsize_t i = 0; i < IT; i++) {
        result = bounded_mpmc_deq(&g_queue, &data);
        if (result == QUEUE_BOUNDED_OK) {
            printf("[T%zu] deq %zu\n", tid, ((data_t *)data)->id);
            free(data);
        }
    }
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;
    if (tid % 2 == 0) {
        produce(tid);
    } else {
        consume(tid);
    }
    return NULL;
}

int
main(void)
{
    void *data = NULL;
    pthread_t threads[N];

    bounded_mpmc_init(&g_queue, g_buffer, BUFF_SZ);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    while (bounded_mpmc_deq(&g_queue, &data) != QUEUE_BOUNDED_EMPTY) {
        free(data);
    }
    return 0;
}
```



Typically, one tries to enqueue or dequeue as long the return value is different from QUEUE_BOUNDED_OK.

The retrying loop can be combined with exponential backoff.


### References:
 This is a variation of the algorithm in buf_ring.h of FreeBSD 8.0.0. 

---
# Functions 

| Function | Description |
|---|---|
| [bounded_mpmc_init](bounded_mpmc.h.md#function-bounded_mpmc_init) | Initializes the given queue object.  |
| [bounded_mpmc_enq](bounded_mpmc.h.md#function-bounded_mpmc_enq) | Tries to enqueue a value.  |
| [bounded_mpmc_deq](bounded_mpmc.h.md#function-bounded_mpmc_deq) | Tries to dequeue a value.  |

##  Function `bounded_mpmc_init`

```c
static void bounded_mpmc_init(bounded_mpmc_t *q, void **b, vuint32_t s)
``` 
_Initializes the given queue object._ 




**Parameters:**

- `q`: address of bounded_mpmc_t object. 
- `b`: address of an array of void* slots with size `s`. 
- `s`: number of elements in array b (does not have to be power of 2). 




##  Function `bounded_mpmc_enq`

```c
static bounded_ret_t bounded_mpmc_enq(bounded_mpmc_t *q, void *v)
``` 
_Tries to enqueue a value._ 




**Parameters:**

- `q`: address of bounded_mpmc_t object. 
- `v`: address of object to enqueue.


**Returns:** QUEUE_BOUNDED_OK if successful. 

**Returns:** QUEUE_BOUNDED_FULL if queue is full. 

**Returns:** QUEUE_BOUNDED_AGAIN if failed to enqueue, the caller should try again. 



##  Function `bounded_mpmc_deq`

```c
static bounded_ret_t bounded_mpmc_deq(bounded_mpmc_t *q, void **v)
``` 
_Tries to dequeue a value._ 




**Parameters:**

- `q`: address of bounded_mpmc_t object. 
- `v`: output parameter of type (void**). Contains the address of the dequeued object, if the dequeue was successful.


**Returns:** QUEUE_BOUNDED_OK if successful. 

**Returns:** QUEUE_BOUNDED_EMPTY if queue is empty. 

**Returns:** QUEUE_BOUNDED_AGAIN if failed to dequeue, the caller should try again. 




---
