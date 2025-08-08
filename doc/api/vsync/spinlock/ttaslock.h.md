#  [vsync](../README.md) / [spinlock](README.md) / ttaslock.h
_Test, Test and Set lock._ 


### Example:



```c
#include <vsync/spinlock/ttaslock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

ttaslock_t g_lock = TTASLOCK_INIT();
vuint32_t g_x     = 0;
vuint32_t g_y     = 0;

void *
run(void *args)
{
    ttaslock_acquire(&g_lock);
    g_x++;
    g_y++;
    ttaslock_release(&g_lock);

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

Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 7.3](https://dl.acm.org/doi/pdf/10.5555/2385452) 

---
# Macros 

| Macro | Description |
|---|---|
| [TTASLOCK_INIT](ttaslock.h.md#macro-ttaslock_init) | Initializer of `ttaslock_t`.  |

##  Macro `TTASLOCK_INIT`

```c
TTASLOCK_INIT()
```

 
_Initializer of_ `ttaslock_t`_._ 



---
# Functions 

| Function | Description |
|---|---|
| [ttaslock_init](ttaslock.h.md#function-ttaslock_init) | Initializes the TTAS lock.  |
| [ttaslock_acquire](ttaslock.h.md#function-ttaslock_acquire) | Acquires the TAS lock.  |
| [ttaslock_tryacquire](ttaslock.h.md#function-ttaslock_tryacquire) | Tries to acquire the TTAS lock.  |
| [ttaslock_release](ttaslock.h.md#function-ttaslock_release) | Releases the TTAS lock.  |

##  Function `ttaslock_init`

```c
static void ttaslock_init(ttaslock_t *l)
``` 
_Initializes the TTAS lock._ 




**Parameters:**

- `l`: address of ttaslock_t object.


> **Note:** alternatively use `TTASLOCK_INIT`. 


##  Function `ttaslock_acquire`

```c
static void ttaslock_acquire(ttaslock_t *l)
``` 
_Acquires the TAS lock._ 




**Parameters:**

- `l`: address of ttaslock_t object. 




##  Function `ttaslock_tryacquire`

```c
static vbool_t ttaslock_tryacquire(ttaslock_t *l)
``` 
_Tries to acquire the TTAS lock._ 




**Parameters:**

- `l`: address of ttaslock_t object. 


**Returns:** true, if lock is acquired successfully. 

**Returns:** false, if failed to acquire the lock. 



##  Function `ttaslock_release`

```c
static void ttaslock_release(ttaslock_t *l)
``` 
_Releases the TTAS lock._ 




**Parameters:**

- `l`: address of ttaslock_t object. 





---
