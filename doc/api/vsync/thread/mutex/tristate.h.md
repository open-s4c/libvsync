#  [vsync](../../README.md) / [thread](../README.md) / [mutex](README.md) / tristate.h
_3-state mutex._ 


### Example:



```c
#include <vsync/thread/mutex.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

vmutex_t g_mutex;
vuint32_t g_x = 0;
vuint32_t g_y = 0;

void *
run(void *args)
{
    vmutex_acquire(&g_mutex);
    g_x++;
    g_y++;
    vmutex_release(&g_mutex);

    (void)args;
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    vmutex_init(&g_mutex);

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



> **Note:** replace `#include <`[`vsync/thread/mutex.h`](../mutex.h.md)`>` with `#include <`[`vsync/thread/mutex/tristate.h`](tristate.h.md)`>` in the example above.

> **Note:** on linux compile with -D_GNU_SOURCE


### References:
 [Ulrich Drepper - Futexes Are Tricky](https://cis.temple.edu/~ingargio/old/cis307s07/readings/futex.pdf) 

---
# Functions 

| Function | Description |
|---|---|
| [vmutex_init](tristate.h.md#function-vmutex_init) | Initializes the mutex `m`.  |
| [vmutex_acquire](tristate.h.md#function-vmutex_acquire) | Acquires the mutex `m`.  |
| [vmutex_release](tristate.h.md#function-vmutex_release) | Releases the mutex `m`.  |

##  Function `vmutex_init`

```c
static void vmutex_init(vmutex_t *m)
``` 
_Initializes the mutex_ `m`_._ 




**Parameters:**

- `m`: address of [vmutex_t](structvmutex__t) object. 




##  Function `vmutex_acquire`

```c
static void vmutex_acquire(vmutex_t *m)
``` 
_Acquires the mutex_ `m`_._ 




**Parameters:**

- `m`: address of [vmutex_t](structvmutex__t) object. 




##  Function `vmutex_release`

```c
static void vmutex_release(vmutex_t *m)
``` 
_Releases the mutex_ `m`_._ 




**Parameters:**

- `m`: address of [vmutex_t](structvmutex__t) object. 





---
