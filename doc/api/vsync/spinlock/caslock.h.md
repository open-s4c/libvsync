#  [vsync](../README.md) / [spinlock](README.md) / caslock.h
_Simple spinlock based on compare-and-swap (CAS)._ 

CAS is implemented with vatomic32_cmpxchg or a variant.


### Example:



```c
#include <vsync/spinlock/caslock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

caslock_t g_lock = CASLOCK_INIT();
vuint32_t g_x    = 0;
vuint32_t g_y    = 0;

void *
run(void *args)
{
    caslock_acquire(&g_lock);
    g_x++;
    g_y++;
    caslock_release(&g_lock);

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
| [CASLOCK_INIT](caslock.h.md#macro-caslock_init) | Initializer of `caslock_t`.  |

##  Macro `CASLOCK_INIT`

```c
CASLOCK_INIT()
```

 
_Initializer of_ `caslock_t`_._ 



---
# Functions 

| Function | Description |
|---|---|
| [caslock_init](caslock.h.md#function-caslock_init) | Initializes the CAS lock.  |
| [caslock_acquire](caslock.h.md#function-caslock_acquire) | Acquires the CAS lock.  |
| [caslock_tryacquire](caslock.h.md#function-caslock_tryacquire) | Tries to acquire the CAS lock.  |
| [caslock_release](caslock.h.md#function-caslock_release) | Releases the CAS lock.  |

##  Function `caslock_init`

```c
static void caslock_init(caslock_t *l)
``` 
_Initializes the CAS lock._ 




**Parameters:**

- `l`: address of caslock_t object.


> **Note:** alternatively use `CASLOCK_INIT`. 


##  Function `caslock_acquire`

```c
static void caslock_acquire(caslock_t *l)
``` 
_Acquires the CAS lock._ 




**Parameters:**

- `l`: address of caslock_t object. 




##  Function `caslock_tryacquire`

```c
static vbool_t caslock_tryacquire(caslock_t *l)
``` 
_Tries to acquire the CAS lock._ 




**Parameters:**

- `l`: address of caslock_t object. 


**Returns:** true, if lock is acquired successfully. 

**Returns:** false, if failed to acquire the lock. 



##  Function `caslock_release`

```c
static void caslock_release(caslock_t *l)
``` 
_Releases the CAS lock._ 




**Parameters:**

- `l`: address of caslock_t object. 





---
