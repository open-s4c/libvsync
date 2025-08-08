#  [vsync](../README.md) / [spinlock](README.md) / rec_ticketlock.h
_Recursive ticketlock implementation using recursive.h._ 

**Groups:** [Fair locks](GROUP_fair_lock.md), [Reentrant](GROUP_reentrant.md)

`rec_ticketlock` is a ticketlock that supports recursion, i.e., is reentrant.

see [vsync/spinlock/ticketlock.h](ticketlock.h.md)


### Example:



```c
#include <vsync/spinlock/rec_ticketlock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

rec_ticketlock_t g_lock = REC_TICKETLOCK_INIT();
vuint32_t g_x           = 0;
vuint32_t g_y           = 0;

void *
run(void *args)
{
    vuint32_t tid = (vuint32_t)(vsize_t)args;
    // can be acquired multiple times without causing dead-lock
    rec_ticketlock_acquire(&g_lock, tid);
    rec_ticketlock_acquire(&g_lock, tid);
    g_x++;
    g_y++;
    rec_ticketlock_release(&g_lock);
    rec_ticketlock_release(&g_lock);

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
| [REC_TICKETLOCK_INIT](rec_ticketlock.h.md#macro-rec_ticketlock_init) | Initializer of `rec_ticketlock`.  |

##  Macro `REC_TICKETLOCK_INIT`

```c
REC_TICKETLOCK_INIT()
```

 
_Initializer of_ `rec_ticketlock`_._ 



---
# Functions 

| Function | Description |
|---|---|
| [rec_ticketlock_init](rec_ticketlock.h.md#function-rec_ticketlock_init) | Initializes the recursive ticketlock.  |
| [rec_ticketlock_acquire](rec_ticketlock.h.md#function-rec_ticketlock_acquire) | Acquires the recursive ticketlock.  |
| [rec_ticketlock_release](rec_ticketlock.h.md#function-rec_ticketlock_release) | Releases the recursive ticketlock.  |
| [rec_ticketlock_tryacquire](rec_ticketlock.h.md#function-rec_ticketlock_tryacquire) | Tries to acquire the recursive ticketlock.  |

##  Function `rec_ticketlock_init`

```c
static void rec_ticketlock_init(struct rec_ticketlock_s *l)
``` 
_Initializes the recursive ticketlock._ 




**Parameters:**

- `l`: address of rec_ticketlock_t object.


> **Note:** alternatively use `REC_TICKETLOCK_INIT`. 


##  Function `rec_ticketlock_acquire`

```c
static void rec_ticketlock_acquire(struct rec_ticketlock_s *l, vuint32_t id)
``` 
_Acquires the recursive ticketlock._ 


This function can be called multiple times by the same thread, consecutively.



**Parameters:**

- `l`: address of rec_ticketlock_t object. 
- `id`: thread ID or core ID. 




##  Function `rec_ticketlock_release`

```c
static void rec_ticketlock_release(struct rec_ticketlock_s *l)
``` 
_Releases the recursive ticketlock._ 


To fully release the lock, this function should be called the same number of times as the acquire was consecutively called by thread that currently owns the lock.



**Parameters:**

- `l`: address of rec_ticketlock_t object. 




##  Function `rec_ticketlock_tryacquire`

```c
static vbool_t rec_ticketlock_tryacquire(struct rec_ticketlock_s *l, vuint32_t id)
``` 
_Tries to acquire the recursive ticketlock._ 


This can be called multiple times by the same thread, consecutively.



**Parameters:**

- `l`: address of rec_ticketlock_t object. 
- `id`: thread ID or core ID. 


**Returns:** true, if lock is acquired successfully. 

**Returns:** false, if failed to acquire the lock. 




---
