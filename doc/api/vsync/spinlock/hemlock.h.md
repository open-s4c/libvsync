#  [vsync](../README.md) / [spinlock](README.md) / hemlock.h
_Hemlock by Dice and Kogan._ 


### Example:



```c
#include <vsync/spinlock/hemlock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define MAX_THREADS  N
#define EXPECTED_VAL N

hemlock_t g_lock_a = HEMLOCK_INIT();
hemlock_t g_lock_b = HEMLOCK_INIT();

hem_node_t g_nodes[N];

vuint32_t g_x = 0;
vuint32_t g_y = 0;

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;
    // the same thread context object can be used with multiple instances of
    // hemlock
    hemlock_acquire(&g_lock_a, &g_nodes[tid]);
    hemlock_acquire(&g_lock_b, &g_nodes[tid]);
    g_x++;
    g_y++;
    hemlock_release(&g_lock_b, &g_nodes[tid]);
    hemlock_release(&g_lock_a, &g_nodes[tid]);
    (void)args;
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    // use this function as an alternative to HEMLOCK_INIT
    hemlock_init(&g_lock_a);
    hemlock_init(&g_lock_b);

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

Dice and Kogan - [Hemlock : Compact and Scalable Mutual Exclusion](https://arxiv.org/abs/2102.03863) 

---
# Macros 

| Macro | Description |
|---|---|
| [HEMLOCK_INIT](hemlock.h.md#macro-hemlock_init) | Initializer of `hemlock_t`.  |

##  Macro `HEMLOCK_INIT`

```c
HEMLOCK_INIT()
```

 
_Initializer of_ `hemlock_t`_._ 



---
# Functions 

| Function | Description |
|---|---|
| [hemlock_init](hemlock.h.md#function-hemlock_init) | Initializes the given lock object `l`.  |
| [hemlock_tryacquire](hemlock.h.md#function-hemlock_tryacquire) | Tries to acquire the Hemlock.  |
| [hemlock_acquire](hemlock.h.md#function-hemlock_acquire) | Acquires the Hemlock.  |
| [hemlock_release](hemlock.h.md#function-hemlock_release) | Releases the Hemlock.  |
| [hemlock_has_waiters](hemlock.h.md#function-hemlock_has_waiters) | Returns whether there is a thread waiting to acquire the Hemlock.  |

##  Function `hemlock_init`

```c
static void hemlock_init(hemlock_t *l)
``` 
_Initializes the given lock object_ `l`_._ 




**Parameters:**

- `l`: address of hemlock_t object. 




##  Function `hemlock_tryacquire`

```c
static int hemlock_tryacquire(hemlock_t *l, hem_node_t *node)
``` 
_Tries to acquire the Hemlock._ 




**Parameters:**

- `l`: address of hemlock_t object. 
- `node`: address of hem_node_t object. Associated with the calling thread/core. 


**Returns:** 1 on success, 0 on failure 



##  Function `hemlock_acquire`

```c
static void hemlock_acquire(hemlock_t *l, hem_node_t *node)
``` 
_Acquires the Hemlock._ 




**Parameters:**

- `l`: address of hemlock_t object. 
- `node`: address of hem_node_t object. Associated with the calling thread/core. 




##  Function `hemlock_release`

```c
static void hemlock_release(hemlock_t *l, hem_node_t *node)
``` 
_Releases the Hemlock._ 




**Parameters:**

- `l`: address of hemlock_t object. 
- `node`: address of hem_node_t object. Associated with the calling thread/core. 




##  Function `hemlock_has_waiters`

```c
static int hemlock_has_waiters(hemlock_t *l, hem_node_t *node)
``` 
_Returns whether there is a thread waiting to acquire the Hemlock._ 


This function should only be called by the current owner of the lock.



**Parameters:**

- `l`: address of hemlock_t object. 
- `node`: address of hem_node_t object. Associated with the calling thread/core. 





---
