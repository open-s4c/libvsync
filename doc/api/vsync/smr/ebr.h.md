#  [vsync](../README.md) / [smr](README.md) / ebr.h
_Epoch Based Reclamation (EBR) SMR scheme._ 

EBR relies on global and local epoch counters to determine when it is safe to reclaim memory.


- global epoch = `e+1` => all active threads must have a local epoch in {`e`, `e+1`}
- global epoch = `e+1` => nodes retired on epoch `e-1` are safe to recycle.
### Example:




```c
#include <vsync/queue/unbounded_queue_lf.h>
#include <vsync/smr/ebr.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define IT 10000
#define N  12

typedef struct data_s {
    vsize_t id;
} data_t;

vebr_t g_vebr;
vqueue_ub_t g_queue;
pthread_mutex_t g_lock;

static inline void
lock_acq(void *arg)
{
    pthread_mutex_t *lock = (pthread_mutex_t *)arg;
    int ret               = pthread_mutex_lock(lock);
    assert(ret == 0);
}

static inline void
lock_rel(void *arg)
{
    pthread_mutex_t *lock = (pthread_mutex_t *)arg;
    int ret               = pthread_mutex_unlock(lock);
    assert(ret == 0);
}

smr_lock_lib_t g_lock_lib = {lock_acq, lock_rel, &g_lock};

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
    vebr_thread_t *thrd = args;
    vebr_retire(&g_vebr, thrd, &qnode->smr_node, free_cb, NULL);
}

void
destroy_cb(vqueue_ub_node_t *qnode, void *args)
{
    free(qnode);
    (void)args;
}

vatomic8_t g_stop = VATOMIC_INIT(0);

void
reclaim(void)
{
    while (vatomic8_read(&g_stop) == 0) {
        vsize_t count = vebr_recycle(&g_vebr);
        if (count > 0) {
            printf("%zu node(s) were reclaimed\n", count);
        }
    }
}

void *
run(void *args)
{
    vebr_thread_t thread;
    data_t *data = NULL;
    vsize_t tid  = (vsize_t)args;

    if (tid == 0) {
        reclaim();
    } else {
        vebr_register(&g_vebr, &thread);

        for (vsize_t i = 0; i < IT; i++) {
            vebr_enter(&g_vebr, &thread);

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
            vebr_exit(&g_vebr, &thread);
        }

        vebr_deregister(&g_vebr, &thread);
    }
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];
    data_t *data = NULL;

    int ret = pthread_mutex_init(&g_lock, NULL);
    assert(ret == 0);

    vebr_init(&g_vebr, g_lock_lib);
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

    vebr_destroy(&g_vebr);

    ret = pthread_mutex_destroy(&g_lock);
    assert(ret == 0);
    return 0;
}
```




### References:

Keir Fraser - [Practical lock-freedom](https://www.cl.cam.ac.uk/techreports/UCAM-CL-TR-579.pdf) 

---
# Functions 

| Function | Description |
|---|---|
| [vebr_init](ebr.h.md#function-vebr_init) | Initializes the given object `ebr`.  |
| [vebr_destroy](ebr.h.md#function-vebr_destroy) | Destroys the resources associated with given `ebr` object.  |
| [vebr_register](ebr.h.md#function-vebr_register) | Registers and initializes the given `ebr_thrd`.  |
| [vebr_deregister](ebr.h.md#function-vebr_deregister) | Deregisters the given `ebr_thrd`.  |
| [vebr_enter](ebr.h.md#function-vebr_enter) | Marks the beginning of a critical section, indicates that a reader is active.  |
| [vebr_exit](ebr.h.md#function-vebr_exit) | Marks the end of a critical section, indicates that a reader is inactive.  |
| [vebr_retire](ebr.h.md#function-vebr_retire) | Defers reclamation of objects until it is safe to do so.  |
| [vebr_recycle](ebr.h.md#function-vebr_recycle) | Recycles/frees nodes that can safely be freed.  |
| [vebr_sync](ebr.h.md#function-vebr_sync) | Synchronizes active threads.  |

##  Function `vebr_init`

```c
static void vebr_init(vebr_t *ebr, smr_lock_lib_t lock_lib)
``` 
_Initializes the given object_ `ebr`_._ 




**Parameters:**

- `ebr`: address of vebr_t object. 
- `lock_lib`: smr_lock_lib_t object. 




##  Function `vebr_destroy`

```c
static void vebr_destroy(vebr_t *ebr)
``` 
_Destroys the resources associated with given_ `ebr` _object._ 




**Parameters:**

- `ebr`: address of vebr_t object. 




##  Function `vebr_register`

```c
static void vebr_register(vebr_t *ebr, vebr_thread_t *ebr_thrd)
``` 
_Registers and initializes the given_ `ebr_thrd`_._ 


> **Note:** each thread must be associated with a unique `ebr_thrd` that lives till the thread deregisters.



**Parameters:**

- `ebr`: address of vebr_t object. 
- `ebr_thrd`: address of vebr_thread_t object. 




##  Function `vebr_deregister`

```c
static void vebr_deregister(vebr_t *ebr, vebr_thread_t *ebr_thrd)
``` 
_Deregisters the given_ `ebr_thrd`_._ 


> **Note:** the thread is associated with the given `ebr_thrd` should not attempt to access the given `ebr` instance after this call. 
**Precondition:** `vebr_register`. 


**Parameters:**

- `ebr`: address of vebr_t object. 
- `ebr_thrd`: address of vebr_thread_t object. 




##  Function `vebr_enter`

```c
static void vebr_enter(vebr_t *ebr, vebr_thread_t *ebr_thrd)
``` 
_Marks the beginning of a critical section, indicates that a reader is active._ 



**Postcondition:** `vebr_exit` 


**Parameters:**

- `ebr`: address of vebr_t object. 
- `ebr_thrd`: address of vebr_thread_t object. 




##  Function `vebr_exit`

```c
static void vebr_exit(vebr_t *ebr, vebr_thread_t *ebr_thrd)
``` 
_Marks the end of a critical section, indicates that a reader is inactive._ 



**Precondition:** `vebr_enter` 


**Parameters:**

- `ebr`: address of vebr_t object. 
- `ebr_thrd`: address of vebr_thread_t object. 




##  Function `vebr_retire`

```c
static void vebr_retire(vebr_t *ebr, vebr_thread_t *ebr_thrd, smr_node_t *smr_node, smr_node_destroy_fun destructor, void *destructor_args)
``` 
_Defers reclamation of objects until it is safe to do so._ 


> **Note:** must be called within the critical section. This is important to avoid data race on the local queue when recycle is being called by another thread. 

**Parameters:**

- `ebr`: address of vebr_t object. 
- `ebr_thrd`: address of vebr_thread_t object. 
- `smr_node`: address of smr_node_t object. 
- `dest`: address of callback function used for destroying the retired `smr_node`. 




##  Function `vebr_recycle`

```c
static vsize_t vebr_recycle(vebr_t *ebr)
``` 
_Recycles/frees nodes that can safely be freed._ 


Assesses whether all active threads have observed the current epoch_global. If so, advances the global epoch:  `epoch_global  = epoch_global + 1`. And frees nodes associated with with `epoch_global - 1`, i.e.  `epoch_global  * - 2`

> **Note:** It is recommended that you call this function periodically in its own dedicated thread that does not interfere with the readers. As this function acquires a lock and might block the calling thread for a while. 

**Parameters:**

- `ebr`: address of vebr_t object.


**Returns:** count of recycled nodes. 



##  Function `vebr_sync`

```c
static void vebr_sync(vebr_t *ebr)
``` 
_Synchronizes active threads._ 


The caller can free nodes detached before this call directly after this function returns. In order to give this guarantee. The observed global epoch `global_epoch` must be advanced twice  `global_epoch  = global_epoch + 2` or all threads are observed as inactive. If a thread is active it must have already observed `global_epoch + 1` by the return of this function.



```
// detach node
vebr_sync(&ebr);
free(node);
```



> **Note:** this function is designed for the above use-case, and it should not be used unless it is absolutely. This function acquires a lock and uses spin-loops. Thus, it is recommended to use `vebr_retire` and `vebr_recycle` instead. 

**Parameters:**

- `ebr`: address of vebr_t object. 





---
