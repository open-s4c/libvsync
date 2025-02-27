#  [vsync](../README.md) / [queue](README.md) / chaselev.h
_Chase-Lev Work-Stealing bounded deque._ 

**Groups:** [Lock-free](../GROUP_lock_free.md)

The deque has a bounded size and returns errors in case the deque is full, empty, or if there is contention while trying to pop/steal.


### References:

David Chase, Yossi Lev - [Dynamic Circular Work-Stealing Deque](https://dl.acm.org/doi/pdf/10.1145/1073970.1073974)


### Example:



```c
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <vsync/atomic.h>
#include <vsync/queue/chaselev.h>

#define ARRAY_SIZE  5
#define NUM_THREADS 3

vdeque_t q;

typedef struct data_s {
    vsize_t id;
} data_t;

void
owner_thread(vsize_t tid)
{
    vsize_t d    = tid;
    data_t *data = NULL;

    for (vsize_t i = 0; i < NUM_THREADS; i++) {
        data     = malloc(sizeof(data_t));
        data->id = d++;

        if (vdeque_push_bottom(&q, data) != VDEQUE_STATE_OK) {
            // either full/ busy
            free(data);
        }
    }

    data = NULL;

    if (vdeque_pop_bottom(&q, (void **)&data) == VDEQUE_STATE_OK) {
        assert(data != NULL);
        printf("[T%zu] deq %zu\n", tid, data->id);
        free(data);
    }
}

void
stealing_thread(vsize_t tid)
{
    data_t *data = NULL;

    if (vdeque_steal(&q, (void **)&data) == VDEQUE_STATE_OK) {
        assert(data != NULL);
        printf("[T%zu] stole %zu\n", tid, data->id);
        free(data);
    }
}

void *
run(void *args)
{
    vsize_t i = (vsize_t)args;

    assert(i < NUM_THREADS);
    switch (i) {
        case 0:
            // Only one thread is owner
            owner_thread(i);
            break;
        default:
            stealing_thread(i);
            break;
    }
    return NULL;
}

int
main(void)
{
    pthread_t threads[NUM_THREADS];

    // Initialize the array struct
    vuint32_t array_size = vdeque_memsize(ARRAY_SIZE);
    vdeque_array_t *a    = (vdeque_array_t *)malloc(array_size);
    assert(a != NULL);

    // Initialize deque
    vdeque_init(&q, a, ARRAY_SIZE);

    // Create threads
    for (vsize_t i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    // Join threads
    for (vsize_t i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
```

 

---
# Functions 

| Function | Description |
|---|---|
| [vdeque_init](chaselev.h.md#function-vdeque_init) | Initializes chase-lev deque.  |
| [vdeque_memsize](chaselev.h.md#function-vdeque_memsize) | Returns array struct memory size.  |
| [vdeque_push_bottom](chaselev.h.md#function-vdeque_push_bottom) | Tries to push a value to the bottom.  |
| [vdeque_pop_bottom](chaselev.h.md#function-vdeque_pop_bottom) | Tries to pop a value from the bottom.  |
| [vdeque_steal](chaselev.h.md#function-vdeque_steal) | Tries to (steal) pop a value from the top.  |

##  Function `vdeque_init`

```c
static void vdeque_init(vdeque_t *q, vdeque_array_t *a, vsize_t len)
``` 
_Initializes chase-lev deque._ 




**Parameters:**

- `q`: address of vdeque_t object. 
- `a`: address of vdeque_array_t object. 
- `len`: length of array `a`. 




##  Function `vdeque_memsize`

```c
static vsize_t vdeque_memsize(vsize_t cap)
``` 
_Returns array struct memory size._ 




**Parameters:**

- `cap`: capacity of buffer. 


**Returns:** size of the array struct with a buffer of capacity `cap`. 



##  Function `vdeque_push_bottom`

```c
static vdeque_state_t vdeque_push_bottom(vdeque_t *q, void *in_obj)
``` 
_Tries to push a value to the bottom._ 




**Parameters:**

- `q`: address of vdeque_t object. 
- `in_obj`: address of object to push.


**Returns:** VDEQUE_STATE_OK if successful. 

**Returns:** VDEQUE_STATE_FULL if deque is full. 



##  Function `vdeque_pop_bottom`

```c
static vdeque_state_t vdeque_pop_bottom(vdeque_t *q, void **out_obj)
``` 
_Tries to pop a value from the bottom._ 




**Parameters:**

- `q`: address of vdeque_t object. 
- `out_obj`: output parameter address of popped object.


**Returns:** VDEQUE_STATE_OK if successful. 

**Returns:** VDEQUE_STATE_EMPTY if deque is empty, or if it failed to take the last element. 



##  Function `vdeque_steal`

```c
static vdeque_state_t vdeque_steal(vdeque_t *q, void **out_obj)
``` 
_Tries to (steal) pop a value from the top._ 




**Parameters:**

- `q`: address of vdeque_t object. 
- `out_obj`: output parameter address of popped object.


**Returns:** VDEQUE_STATE_OK if successful. 

**Returns:** VDEQUE_STATE_EMPTY if deque is empty. 

**Returns:** VDEQUE_STATE_ABORT if failed to pop because of contention. 




---
