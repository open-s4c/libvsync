#  [vsync](../README.md) / [spinlock](README.md) / mcslock.h
_Mellor-Crummey Scott Lock - the well-known scalable lock._ 

**Groups:** [Fair locks](GROUP_fair_lock.md)

MCS lock is a scalable lock that guarantees FIFO order. The MCS lock forms a queue of waiting threads (each thread has one node in the queue), and lets them spin on local variables of their nodes.

Note that this implementation is not reentrant.


### Example:



```c
/*
 * The following example shows how to use the MCS lock. Each threads needs a
 * context, i.e., an instance of `mcs_node_t`. When acquiring or releasing the
 * lock, one needs to pass the lock and the context as arguments. The context
 * of a thread should not be used in multiple MCS locks at the same time.
 */
#include <vsync/spinlock/mcslock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define MAX_THREADS  N
#define EXPECTED_VAL N

mcslock_t g_lock = MCSLOCK_INIT();
mcs_node_t g_msc_nodes[MAX_THREADS];

vuint32_t g_x = 0;
vuint32_t g_y = 0;

void *
run(void *args)
{
    vsize_t tid      = (vsize_t)args;
    mcs_node_t *node = &g_msc_nodes[tid];

    mcslock_acquire(&g_lock, node);
    g_x++;
    g_y++;
    mcslock_release(&g_lock, node);

    (void)args;
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    ASSERT(g_x == EXPECTED_VAL);
    ASSERT(g_x == g_y);
    printf("Final value %u\n", g_x);
    return 0;
}
```




### References:

J.Mellor-Crummey, M.L.Scott - [Algorithms for scalable synchronization on shared-memory multiprocessors. ACM Trans. on Computer Systems. 1991] 

---
# Macros 

| Macro | Description |
|---|---|
| [MCSLOCK_INIT](mcslock.h.md#macro-mcslock_init) | Initializer of `mcslock_t`.  |

##  Macro `MCSLOCK_INIT`

```c
MCSLOCK_INIT()
```

 
_Initializer of_ `mcslock_t`_._ 



---
# Functions 

| Function | Description |
|---|---|
| [mcslock_init](mcslock.h.md#function-mcslock_init) | Initializes the MCS lock.  |
| [mcslock_tryacquire](mcslock.h.md#function-mcslock_tryacquire) | Tries to acquire the MCS lock.  |
| [mcslock_acquire](mcslock.h.md#function-mcslock_acquire) | Acquires the MCS lock.  |
| [mcslock_release](mcslock.h.md#function-mcslock_release) | Releases the MCS lock.  |
| [mcslock_has_waiters](mcslock.h.md#function-mcslock_has_waiters) | Returns whether there is a thread waiting to acquire the MCS lock.  |

##  Function `mcslock_init`

```c
static void mcslock_init(mcslock_t *l)
``` 
_Initializes the MCS lock._ 




**Parameters:**

- `l`: address of mcslock_t object.


> **Note:** alternatively use `MCSLOCK_INIT`. 


##  Function `mcslock_tryacquire`

```c
static vbool_t mcslock_tryacquire(mcslock_t *l, mcs_node_t *node)
``` 
_Tries to acquire the MCS lock._ 




**Parameters:**

- `l`: address of mcslock_t object. 
- `node`: address of mcs_node_t object, associated with the calling thread/core. 


**Returns:** true, on success. 

**Returns:** false, on fail. 



##  Function `mcslock_acquire`

```c
static void mcslock_acquire(mcslock_t *l, mcs_node_t *node)
``` 
_Acquires the MCS lock._ 




**Parameters:**

- `l`: address of mcslock_t object. 
- `node`: address of mcs_node_t object, associated with the calling thread/core. 




##  Function `mcslock_release`

```c
static void mcslock_release(mcslock_t *l, mcs_node_t *node)
``` 
_Releases the MCS lock._ 




**Parameters:**

- `l`: address of mcslock_t object. 
- `node`: address of mcs_node_t object, associated with the calling thread/core. 




##  Function `mcslock_has_waiters`

```c
static vbool_t mcslock_has_waiters(mcslock_t *l, mcs_node_t *node)
``` 
_Returns whether there is a thread waiting to acquire the MCS lock._ 


This function should only be called by the current owner of the lock.



**Parameters:**

- `l`: address of mcslock_t object. 
- `node`: address of mcs_node_t object, associated with the calling thread/core. 


**Returns:** true, if threads are waiting to acquire the lock. 

**Returns:** false, otherwise. 




---
