#  [vsync](../README.md) / [spinlock](README.md) / rec_mcslock.h
_Recursive MCS lock implementation using recursive.h._ 

**Groups:** [Fair locks](GROUP_fair_lock.md)

`rec_mcslock` is a MCS lock that supports recursion, i.e., is reentrant.

see [mcslock.h](mcslock.h.md)


### Example:



```c
#include <vsync/spinlock/rec_mcslock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

rec_mcslock_t g_lock = REC_MCSLOCK_INIT();
mcs_node_t g_nodes[N];

vuint32_t g_x = 0;
vuint32_t g_y = 0;

void *
run(void *args)
{
    vuint32_t tid    = (vuint32_t)(vsize_t)args;
    mcs_node_t *node = &g_nodes[tid];
    // can be acquired multiple times without causing dead-lock
    rec_mcslock_acquire(&g_lock, tid, node);
    rec_mcslock_acquire(&g_lock, tid, node);
    g_x++;
    g_y++;
    rec_mcslock_release(&g_lock, node);
    rec_mcslock_release(&g_lock, node);

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

 

---
# Macros 

| Macro | Description |
|---|---|
| [REC_MCSLOCK_INIT](rec_mcslock.h.md#macro-rec_mcslock_init) | Initializer of `rec_mcslock`.  |

##  Macro `REC_MCSLOCK_INIT`

```c
REC_MCSLOCK_INIT()
```

 
_Initializer of_ `rec_mcslock`_._ 



---
# Functions 

| Function | Description |
|---|---|
| [rec_mcslock_init](rec_mcslock.h.md#function-rec_mcslock_init) | Initializes the recursive MCS lock.  |
| [rec_mcslock_acquire](rec_mcslock.h.md#function-rec_mcslock_acquire) | Acquires the recursive MCS lock.  |
| [rec_mcslock_release](rec_mcslock.h.md#function-rec_mcslock_release) | Releases the recursive MCS lock.  |
| [rec_mcslock_tryacquire](rec_mcslock.h.md#function-rec_mcslock_tryacquire) | Tries to acquire the recursive mcslock.  |

##  Function `rec_mcslock_init`

```c
static void rec_mcslock_init(struct rec_mcslock_s *l)
``` 
_Initializes the recursive MCS lock._ 




**Parameters:**

- `l`: address of rec_mcslock_t object.


> **Note:** alternatively use `REC_MCSLOCK_INIT`. 


##  Function `rec_mcslock_acquire`

```c
static void rec_mcslock_acquire(struct rec_mcslock_s *l, vuint32_t id, mcs_node_t *ctx)
``` 
_Acquires the recursive MCS lock._ 


This function can be called multiple times by the same thread, consecutively.



**Parameters:**

- `l`: address of rec_mcslock_t object. 
- `id`: thread ID or core ID 
- `ctx`: address of rec_mcslock_t object associated with the caller thread/core. 




##  Function `rec_mcslock_release`

```c
static void rec_mcslock_release(struct rec_mcslock_s *l, mcs_node_t *ctx)
``` 
_Releases the recursive MCS lock._ 


To fully release the lock, this function should be called the same number of times as the acquire was consecutively called by thread that currently owns the lock.



**Parameters:**

- `l`: address of rec_mcslock_t object. 
- `ctx`: address of rec_mcslock_t object associated with the caller thread/core. 




##  Function `rec_mcslock_tryacquire`

```c
static vbool_t rec_mcslock_tryacquire(struct rec_mcslock_s *l, vuint32_t id, mcs_node_t *ctx)
``` 
_Tries to acquire the recursive mcslock._ 


This can be called multiple times by the same thread, consecutively.



**Parameters:**

- `l`: address of rec_mcslock_t object. 
- `id`: thread ID or core ID. 


**Returns:** true, if lock is acquired successfully. 

**Returns:** false, if failed to acquire the lock. 




---
