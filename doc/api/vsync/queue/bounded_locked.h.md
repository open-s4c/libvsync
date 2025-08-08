#  [vsync](../README.md) / [queue](README.md) / bounded_locked.h
_Multi-producer, multi-consumer bounded queue protected by a spinlock._ 

**Groups:** [Linearizable](../GROUP_linearizable.md)

This is a non-blocking, multi-producer, multi-consumer queue protected by a spinlock. The queue has a bounded size and returns errors in case the queue is full, empty or the spinlock cannot be acquired.

> **Note:** This queue implementation is very simple and may be inefficient due to the spinlock bottleneck. Prefer using [bounded_mpmc.h](bounded_mpmc.h.md) instead.


### Example:



```c
#include <vsync/queue/bounded_locked.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#define IT      10
#define N       12
#define BUFF_SZ 1000

void *g_buffer[BUFF_SZ];
bounded_locked_t g_queue;

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
        result   = bounded_locked_enq(&g_queue, data);
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
        if (bounded_locked_empty(&g_queue)) {
            continue;
        }
        result = bounded_locked_deq(&g_queue, &data);
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

    bounded_locked_init(&g_queue, g_buffer, BUFF_SZ);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    while (bounded_locked_deq(&g_queue, &data) != QUEUE_BOUNDED_EMPTY) {
        free(data);
    }
    return 0;
}
```

 

---
# Functions 

| Function | Description |
|---|---|
| [bounded_locked_init](bounded_locked.h.md#function-bounded_locked_init) | Initializes the given queue object.  |
| [bounded_locked_enq](bounded_locked.h.md#function-bounded_locked_enq) | Tries to enqueue a value.  |
| [bounded_locked_deq](bounded_locked.h.md#function-bounded_locked_deq) | Tries to dequeue a value.  |
| [bounded_locked_empty](bounded_locked.h.md#function-bounded_locked_empty) | Tests whether queue is empty without performing an action.  |

##  Function `bounded_locked_init`

```c
static void bounded_locked_init(bounded_locked_t *q, void **b, vuint32_t s)
``` 
_Initializes the given queue object._ 




**Parameters:**

- `q`: address of bounded_locked_t object. 
- `b`: address of an array of void* slots with size `s`. 
- `s`: capacity of `b` (in number of array elements/slots). 




##  Function `bounded_locked_enq`

```c
static bounded_ret_t bounded_locked_enq(bounded_locked_t *q, void *v)
``` 
_Tries to enqueue a value._ 




**Parameters:**

- `q`: address of bounded_locked_t object. 
- `v`: address of object to enqueue.


**Returns:** QUEUE_BOUNDED_OK if successful. 

**Returns:** QUEUE_BOUNDED_FULL if queue is full. 

**Returns:** QUEUE_AGAIN if queue is currently locked and the caller should try again. 



##  Function `bounded_locked_deq`

```c
static bounded_ret_t bounded_locked_deq(bounded_locked_t *q, void **v)
``` 
_Tries to dequeue a value._ 




**Parameters:**

- `q`: address of bounded_locked_t object. 
- `v`: output parameter of type (void**). Contains the address of the dequeued object, if the dequeue was successful.


**Returns:** QUEUE_BOUNDED_OK if successful. 

**Returns:** QUEUE_BOUNDED_EMPTY if queue is empty. 

**Returns:** QUEUE_BOUNDED_AGAIN if queue is currently locked and the caller should try again.

> **Note:** The output parameter `v` is only set when QUEUE_BOUNDED_OK is returned. 


##  Function `bounded_locked_empty`

```c
static vbool_t bounded_locked_empty(bounded_locked_t *q)
``` 
_Tests whether queue is empty without performing an action._ 


> **Note:** this function is blocking.



**Parameters:**

- `q`: address of bounded_locked_t object.


**Returns:** true if empty. 

**Returns:** false if not empty. 




---
