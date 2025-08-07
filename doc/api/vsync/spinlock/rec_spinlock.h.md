#  [vsync](../README.md) / [spinlock](README.md) / rec_spinlock.h
_Recursive spinlock implementation using recursive.h._ 

**Groups:** [Reentrant](GROUP_reentrant.md)

see [caslock.h](caslock.h.md)


### Example:



```c
#include <vsync/spinlock/rec_spinlock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

rec_spinlock_t g_lock = REC_SPINLOCK_INIT();
vuint32_t g_x         = 0;
vuint32_t g_y         = 0;

void *
run(void *args)
{
    vuint32_t tid = (vuint32_t)(vsize_t)args;
    // can be acquired multiple times without causing dead-lock
    rec_spinlock_acquire(&g_lock, tid);
    rec_spinlock_acquire(&g_lock, tid);
    g_x++;
    g_y++;
    rec_spinlock_release(&g_lock);
    rec_spinlock_release(&g_lock);

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
| [REC_SPINLOCK_INIT](rec_spinlock.h.md#macro-rec_spinlock_init) | Initializer of `rec_spinlock`.  |

##  Macro `REC_SPINLOCK_INIT`

```c
REC_SPINLOCK_INIT()
```

 
_Initializer of_ `rec_spinlock`_._ 



---
# Functions 

| Function | Description |
|---|---|
| [rec_spinlock_init](rec_spinlock.h.md#function-rec_spinlock_init) | Initializes the recursive spinlock.  |
| [rec_spinlock_tryacquire](rec_spinlock.h.md#function-rec_spinlock_tryacquire) | Tries to acquire the recursive spinlock.  |
| [rec_spinlock_acquire](rec_spinlock.h.md#function-rec_spinlock_acquire) | Acquires the recursive spinlock.  |
| [rec_spinlock_release](rec_spinlock.h.md#function-rec_spinlock_release) | Releases the recursive spinlock.  |

##  Function `rec_spinlock_init`

```c
static void rec_spinlock_init(struct rec_spinlock_s *l)
``` 
_Initializes the recursive spinlock._ 




**Parameters:**

- `l`: address of rec_spinlock_t object.


> **Note:** alternatively use `REC_SPINLOCK_INIT`. 


##  Function `rec_spinlock_tryacquire`

```c
static vbool_t rec_spinlock_tryacquire(struct rec_spinlock_s *l, vuint32_t id)
``` 
_Tries to acquire the recursive spinlock._ 




**Parameters:**

- `l`: address of rec_spinlock_t object. 
- `id`: thread ID or core ID. 


**Returns:** true, if lock is acquired successfully. 

**Returns:** false, if failed to acquire the lock. 



##  Function `rec_spinlock_acquire`

```c
static void rec_spinlock_acquire(struct rec_spinlock_s *l, vuint32_t id)
``` 
_Acquires the recursive spinlock._ 




**Parameters:**

- `l`: address of recursive spinlock lock object 
- `id`: thread ID or core ID. 




##  Function `rec_spinlock_release`

```c
static void rec_spinlock_release(struct rec_spinlock_s *l)
``` 
_Releases the recursive spinlock._ 




**Parameters:**

- `l`: address of rec_spinlock_t object. 





---
