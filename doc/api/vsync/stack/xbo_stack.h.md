#  [vsync](../README.md) / [stack](README.md) / xbo_stack.h
_Unbounded lock-free stack with exponential backoff._ 

**Groups:** [Linearizable](../GROUP_linearizable.md), [Lock-free](../GROUP_lock_free.md), [SMR-required](../GROUP_requires_smr.md)

When an attempt to pop/push fails, the calling thread is put to sleep before it reattempts the operation. The sleep duration is determined randomly and it may not exceed the current limit, which doubles on each backoff. The current limit cannot exceed `max_backoff` passed to `vstack_init`.


### Example:



```c
#include <vsync/stack/xbo_stack.h>
#include <vsync/smr/gdump.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define IT 10000
#define N  12

#define VSTACK_MIN_BACKOFF_MICRO_SEC 1
#define VSTACK_MAX_BACKOFF_MICRO_SEC 1000

typedef struct data_s {
    vstack_node_t stack_node;
    smr_node_t smr_node;
    vsize_t id;
} data_t;

gdump_t g_gdump;
vstack_t g_stack;
pthread_rwlock_t g_lock;
vatomic8_t g_stop = VATOMIC_INIT(0);

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
destroy_cb(vstack_node_t *node, void *args)
{
    data_t *data = V_CONTAINER_OF(node, data_t, stack_node);
    free(data);
    (void)args;
}

int
usleep_cb(vuint32_t microsecond)
{
    return usleep(microsecond);
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
yield_cb(void *args)
{
    (void)args;
    return sched_yield();
}

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

void
consume(vsize_t tid)
{
    gdump_thread_t thread = {0};
    vstack_node_t *node   = NULL;
    data_t *data          = NULL;

    gdump_register(&g_gdump, &thread);

    for (vsize_t i = 0; i < IT; i++) {
        gdump_enter(&g_gdump, &thread);
        node = vstack_pop(&g_stack);
        if (node) {
            data = V_CONTAINER_OF(node, data_t, stack_node);
            printf("[T%zu] popped %zu\n", tid, data->id);
        }
        gdump_exit(&g_gdump, &thread);
        if (node) {
            gdump_retire(&g_gdump, &data->smr_node, free_cb, NULL);
        }
    }

    gdump_deregister(&g_gdump, &thread);
}

void
produce(vsize_t tid)
{
    data_t *data = NULL;
    for (vsize_t i = 0; i < IT; i++) {
        data = malloc(sizeof(data_t));
        if (data) {
            data->id = i;
            printf("[T%zu] pushing %zu\n", tid, data->id);
            vstack_push(&g_stack, &data->stack_node);
        }
    }
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;
    if (tid == 0) {
        reclaim();
    } else if (tid % 2 == 0) {
        produce(tid);
    } else {
        consume(tid);
    }
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    int ret = pthread_rwlock_init(&g_lock, NULL);
    assert(ret == 0);

    gdump_init(&g_gdump, g_rwlock_lib);
    vstack_init(&g_stack, usleep_cb, rand_cb, VSTACK_MIN_BACKOFF_MICRO_SEC,
                VSTACK_MAX_BACKOFF_MICRO_SEC);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 1; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    // send the signal to stop reclaiming
    vatomic8_write(&g_stop, 1);
    pthread_join(threads[0], NULL);

    vstack_destroy(&g_stack, destroy_cb, NULL);
    gdump_destroy(&g_gdump);

    ret = pthread_rwlock_destroy(&g_lock);
    assert(ret == 0);
    return 0;
}
```




### References:

Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 11](https://dl.acm.org/doi/pdf/10.5555/2385452) 

---
# Functions 

| Function | Description |
|---|---|
| [vstack_init](xbo_stack.h.md#function-vstack_init) | Initializes the given `stack`.  |
| [vstack_push](xbo_stack.h.md#function-vstack_push) | Pushes the given `node` to the top of the given `stack`.  |
| [vstack_pop](xbo_stack.h.md#function-vstack_pop) | Pops the top of the given `stack`.  |
| [vstack_destroy](xbo_stack.h.md#function-vstack_destroy) | Pops all remaining nodes in the stack and calls `destroy` on them.  |

##  Function `vstack_init`

```c
static void vstack_init(vstack_t *stack, backoff_usleep_fun_t vstack_usleep, backoff_rand_fun_t rand_fun, vuint32_t min_backoff, vuint32_t max_backoff)
``` 
_Initializes the given_ `stack`_._ 




**Parameters:**

- `stack`: address of vstack_t object. 
- `vstack_usleep`: address of `usleep` like function. 
- `rand_fun`: a function pointer to a function that generates a random number. 
- `min_backoff`: minimum amount of microseconds a thread is allowed to sleep. 
- `max_backoff`: maximum allowed amount of microseconds to sleep. 




##  Function `vstack_push`

```c
static void vstack_push(vstack_t *stack, vstack_node_t *node)
``` 
_Pushes the given_ `node` _to the top of the given_ `stack`_._ 




**Parameters:**

- `stack`: address of vstack_t object. 
- `node`: address of vstack_node_t object.


> **Note:** this operation always succeeds. 


##  Function `vstack_pop`

```c
static vstack_node_t* vstack_pop(vstack_t *stack)
``` 
_Pops the top of the given_ `stack`_._ 




**Parameters:**

- `stack`: address of vstack_t object. 


**Returns:** address of vstack_node_t object. If the stack is not empty. 

**Returns:** NULL on empty stack.

> **Note:** must be called inside an SMR critical section. 


##  Function `vstack_destroy`

```c
static void vstack_destroy(vstack_t *stack, vstack_node_handler_t destroy, void *arg)
``` 
_Pops all remaining nodes in the stack and calls_ `destroy` _on them._ 




**Parameters:**

- `stack`: address of vstack_t object. 
- `destroy`: function address of type vstack_node_handler_t. 
- `arg`: second argument of `destroy`, can be NULL. 





---
