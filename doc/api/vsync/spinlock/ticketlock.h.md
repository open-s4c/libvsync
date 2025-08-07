#  [vsync](../README.md) / [spinlock](README.md) / ticketlock.h
_Classic ticketlock with two 32-bit variables (owner and next)._ 

**Groups:** [Fair locks](GROUP_fair_lock.md)

Ticketlock is a simple, fair lock (guarantees FIFO order).


### Example:



```c
#include <vsync/spinlock/ticketlock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

ticketlock_t g_lock = TICKETLOCK_INIT();
vuint32_t g_x       = 0;
vuint32_t g_y       = 0;

void *
run(void *args)
{
    ticketlock_acquire(&g_lock);
    g_x++;
    g_y++;
    ticketlock_release(&g_lock);

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
# Functions 

| Function | Description |
|---|---|
| [ticketlock_init](ticketlock.h.md#function-ticketlock_init) | Initializes the ticketlock.  |
| [ticketlock_acquire](ticketlock.h.md#function-ticketlock_acquire) | Acquires the ticketlock.  |
| [ticketlock_tryacquire](ticketlock.h.md#function-ticketlock_tryacquire) | Tries to acquire the ticketlock.  |
| [ticketlock_release](ticketlock.h.md#function-ticketlock_release) | Releases the ticketlock.  |
| [ticketlock_has_waiters](ticketlock.h.md#function-ticketlock_has_waiters) | Checks whether there are threads waiting to acquire the lock.  |

##  Function `ticketlock_init`

```c
static void ticketlock_init(ticketlock_t *l)
``` 
_Initializes the ticketlock._ 




**Parameters:**

- `l`: address of ticketlock_t object.


> **Note:** alternatively use `TICKETLOCK_INIT`. 


##  Function `ticketlock_acquire`

```c
static void ticketlock_acquire(ticketlock_t *l)
``` 
_Acquires the ticketlock._ 




**Parameters:**

- `l`: address of ticketlock_t object. 




##  Function `ticketlock_tryacquire`

```c
static vbool_t ticketlock_tryacquire(ticketlock_t *l)
``` 
_Tries to acquire the ticketlock._ 




**Parameters:**

- `l`: address of ticketlock_t object. 


**Returns:** true, if lock is acquired successfully. 

**Returns:** false, if failed to acquire the lock. 



##  Function `ticketlock_release`

```c
static void ticketlock_release(ticketlock_t *l)
``` 
_Releases the ticketlock._ 




**Parameters:**

- `l`: address of ticketlock_t object. 




##  Function `ticketlock_has_waiters`

```c
static vbool_t ticketlock_has_waiters(ticketlock_t *l)
``` 
_Checks whether there are threads waiting to acquire the lock._ 


This can only be called by the thread holding the lock, ie, after acquire but before release.


**Returns:** true, if threads are waiting to acquire the lock. 

**Returns:** false, otherwise. 




---
