#  [vsync](../README.md) / [spinlock](README.md) / semaphore.h
_Counting Semaphore._ 

see [vsync/spinlock/rwlock.h](rwlock.h.md)


### Example:



```c

#include <vsync/spinlock/semaphore.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N             12
#define EXPECTED_VAL  N
#define NUM_RESOURCES 1

semaphore_t g_semaphore = SEMAPHORE_INIT(NUM_RESOURCES);
vuint32_t g_x           = 0;
vuint32_t g_y           = 0;

void *
run(void *args)
{
    semaphore_acquire(&g_semaphore, NUM_RESOURCES);
    g_x++;
    g_y++;
    semaphore_release(&g_semaphore, NUM_RESOURCES);
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
| [SEMAPHORE_INIT](semaphore.h.md#macro-semaphore_init) | Initializer of `semaphore_t` with n resources.  |

##  Macro `SEMAPHORE_INIT`

```c
SEMAPHORE_INIT(n)
```

 
_Initializer of_ `semaphore_t` _with n resources._ 



---
# Functions 

| Function | Description |
|---|---|
| [semaphore_init](semaphore.h.md#function-semaphore_init) | Initializes the semaphore.  |
| [semaphore_acquire](semaphore.h.md#function-semaphore_acquire) | Acquires i resources of the semaphore if available.  |
| [semaphore_tryacquire](semaphore.h.md#function-semaphore_tryacquire) | Tries to acquire i resources of the semaphore if available.  |
| [semaphore_release](semaphore.h.md#function-semaphore_release) | Releases i resources of the semaphore.  |

##  Function `semaphore_init`

```c
static void semaphore_init(semaphore_t *s, vuint32_t n)
``` 
_Initializes the semaphore._ 




**Parameters:**

- `s`: address of semaphore_t object. 
- `n`: number of resources.


> **Note:** alternatively use `SEMAPHORE_INIT` 


##  Function `semaphore_acquire`

```c
static void semaphore_acquire(semaphore_t *s, vuint32_t i)
``` 
_Acquires i resources of the semaphore if available._ 


other blocks until enough resources are available in the semaphore.



**Parameters:**

- `s`: address of semaphore_t object. 
- `i`: number of resources to acquire.


**Precondition:** call `semaphore_acquire` with i <= N. 



##  Function `semaphore_tryacquire`

```c
static vbool_t semaphore_tryacquire(semaphore_t *s, vuint32_t i)
``` 
_Tries to acquire i resources of the semaphore if available._ 




**Parameters:**

- `s`: address of semaphore_t object. 
- `i`: number of resources to acquire. 


**Returns:** true, if i resources were acquired. 

**Returns:** false, if failed to acquire i resources.

**Precondition:** call `semaphore_tryacquire` with i <= N. 



##  Function `semaphore_release`

```c
static void semaphore_release(semaphore_t *s, vuint32_t i)
``` 
_Releases i resources of the semaphore._ 




**Parameters:**

- `s`: address of semaphore_t object. 
- `i`: number of resources to release. 





---
