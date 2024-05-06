#  [vsync](../README.md) / [spinlock](README.md) / clhlock.h
_List-based lock attributed to Craig, Landin and Hagersten._ 


### Example:



```c
#include <vsync/spinlock/clhlock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define MAX_THREADS  N
#define EXPECTED_VAL N

clhlock_t g_lock;
clh_node_t g_nodes[N];

vuint32_t g_x = 0;
vuint32_t g_y = 0;

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;

    clhlock_node_init(&g_nodes[tid]);

    clhlock_acquire(&g_lock, &g_nodes[tid]);
    g_x++;
    g_y++;
    clhlock_release(&g_lock, &g_nodes[tid]);

    (void)args;
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    clhlock_init(&g_lock);

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

Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 7.5](https://dl.acm.org/doi/pdf/10.5555/2385452) 

---
# Functions 

| Function | Description |
|---|---|
| [clhlock_node_init](clhlock.h.md#function-clhlock_node_init) | Initializes the given node.  |
| [clhlock_init](clhlock.h.md#function-clhlock_init) | Initializes the given lock.  |
| [clhlock_acquire](clhlock.h.md#function-clhlock_acquire) | Acquires the lock.  |
| [clhlock_release](clhlock.h.md#function-clhlock_release) | Releases the lock.  |
| [clhlock_has_waiters](clhlock.h.md#function-clhlock_has_waiters) | Returns whether there is a thread waiting to acquire the clhlock.  |

##  Function `clhlock_node_init`

```c
static void clhlock_node_init(clh_node_t *node)
``` 
_Initializes the given node._ 




**Parameters:**

- `node`: address of clh_node_t object. 




##  Function `clhlock_init`

```c
static void clhlock_init(clhlock_t *lock)
``` 
_Initializes the given lock._ 




**Parameters:**

- `lock`: address of clhlock_t object. 




##  Function `clhlock_acquire`

```c
static void clhlock_acquire(clhlock_t *lock, clh_node_t *node)
``` 
_Acquires the lock._ 




**Parameters:**

- `lock`: address of clhlock_t object. 
- `node`: address of clh_node_t object associated with the calling thread.


> **Note:** `node` has to continue to exist even if the thread dies. 


##  Function `clhlock_release`

```c
static void clhlock_release(clhlock_t *lock, clh_node_t *node)
``` 
_Releases the lock._ 




**Parameters:**

- `lock`: address of clhlock_t object. 
- `node`: address of clh_node_t object associated with the calling thread.


> **Note:** It hijacks its predecessor's queue node as its own. 


##  Function `clhlock_has_waiters`

```c
static vbool_t clhlock_has_waiters(clhlock_t *lock, clh_node_t *node)
``` 
_Returns whether there is a thread waiting to acquire the clhlock._ 


This function should only be called by the current owner of the lock.



**Parameters:**

- `lock`: address of clhlock_t object. 
- `node`: address of clh_node_t object associated with the calling thread.


**Returns:** true if there are waiters. 

**Returns:** false if there are no waiters.

> **Note:** this function is not part of the standard lock API. 



---
