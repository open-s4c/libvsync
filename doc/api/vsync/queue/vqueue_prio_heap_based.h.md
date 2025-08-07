#  [vsync](../README.md) / [queue](README.md) / vqueue_prio_heap_based.h
_Unbounded heap-based priority queue._ 

**Groups:** [Linearizable](../GROUP_linearizable.md)

This is a fine-grained blocking implementation, which uses [rec_spinlock.h](../spinlock/rec_spinlock.h.md). It has an unbounded priority range, and bounded capacity.


### Example:



```c
#include <vsync/queue/vqueue_prio_heap_based.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define IT 2
#define N  3

typedef struct data_s {
    vsize_t id;
} data_t;

__thread vuint32_t my_tid; /* this is just an example, thread local storage is
                              not the best option  */
vqueue_prio_t g_queue;

/* a unique number for the tid */
vuint32_t
get_tid_cb(void)
{
    return my_tid;
}

void
destroy_cb(void *node, void *args)
{
    /* the cast is not necessary it is just to emphasize that the
                 object you get here is the same as what has been inserted */
    data_t *data = (data_t *)node;
    free(data);
    (void)args;
}

void *
run(void *args)
{
    data_t *data = NULL;
    /* this cast is not recommended it is just to demonstrate that each thread
     * has a unique id */
    my_tid = (vuint32_t)(vuintptr_t)args;

    for (vsize_t i = 0; i < IT; i++) {
        data = (data_t *)vqueue_prio_remove_min(&g_queue);
        if (data == NULL) {
            data            = malloc(sizeof(data_t));
            data->id        = i;
            vbool_t success = vqueue_prio_add(&g_queue, data, i);
            if (success) {
                printf("[T%u] enq %zu\n", my_tid, data->id);
            } else {
                printf("The queue is full");
                free(data);
            }
        } else {
            printf("[T%u] deq %zu\n", my_tid, data->id);
            free(data);
        }
    }

    return NULL;
}

int
main(void)
{
    pthread_t threads[N];
    vqueue_prio_init(&g_queue, get_tid_cb);
    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }
    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    vqueue_prio_destroy(&g_queue, destroy_cb, NULL);
    return 0;
}
```



> **Note:** If you wish to use this implementation in user-space. Contact the authors for recursive `pthread_mutex` support.


### References:

Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 15.4](https://dl.acm.org/doi/pdf/10.5555/2385452) 

---
# Macros 

| Macro | Description |
|---|---|
| [VQUEUE_PRIO_HEAP_CAPACITY](vqueue_prio_heap_based.h.md#macro-vqueue_prio_heap_capacity) | `VQUEUE_PRIO_HEAP_CAPACITY` is equal to `1024` by default.  |

##  Macro `VQUEUE_PRIO_HEAP_CAPACITY`

 
`VQUEUE_PRIO_HEAP_CAPACITY` _is equal to_ `1024` _by default._ 


Users may overwrite it by defining `VQUEUE_PRIO_HEAP_CAPACITY=N`. This value determines how many nodes can be stored in the queue.

> **Note:** `VQUEUE_PRIO_HEAP_CAPACITY` must be power of two. 


---
# Functions 

| Function | Description |
|---|---|
| [vqueue_prio_init](vqueue_prio_heap_based.h.md#function-vqueue_prio_init) | Initializes the given priority queue object `pqueue`.  |
| [vqueue_prio_destroy](vqueue_prio_heap_based.h.md#function-vqueue_prio_destroy) | Destroys all remaining enqueued objects.  |
| [vqueue_prio_add](vqueue_prio_heap_based.h.md#function-vqueue_prio_add) | Inserts the given object `data` into the heap.  |
| [vqueue_prio_remove_min](vqueue_prio_heap_based.h.md#function-vqueue_prio_remove_min) | Retrieves an object with the highest available priority.  |

##  Function `vqueue_prio_init`

```c
static void vqueue_prio_init(vqueue_prio_t *pqueue, vqueue_prio_fun_get_tid get_tid_fun)
``` 
_Initializes the given priority queue object_ `pqueue`_._ 




**Parameters:**

- `get_tid_fun`: function pointer that returns the calling thread id `vuint32_t`. 
- `pqueue`: address of vqueue_prio_t object. 




##  Function `vqueue_prio_destroy`

```c
static void vqueue_prio_destroy(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t destroy_cb, void *args)
``` 
_Destroys all remaining enqueued objects._ 


> **Note:** this is not thread safe and must be called iff all threads are done accessing the queue.



**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 
- `destroy_cb`: address of a callback function to call on each queue object to destroy. 
- `args`: second argument of `destroy_cb`. 




##  Function `vqueue_prio_add`

```c
static vbool_t vqueue_prio_add(vqueue_prio_t *pqueue, void *data, vsize_t priority)
``` 
_Inserts the given object_ `data` _into the heap._ 




**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 
- `priority`: priority of the given object. 
- `data`: address of the object to enqueue. 


**Returns:** true insertion succeeded. 

**Returns:** false insertion failed the heap is full. 



##  Function `vqueue_prio_remove_min`

```c
static void* vqueue_prio_remove_min(vqueue_prio_t *pqueue)
``` 
_Retrieves an object with the highest available priority._ 




**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 


**Returns:** void* address of the retrieved object. 




---
