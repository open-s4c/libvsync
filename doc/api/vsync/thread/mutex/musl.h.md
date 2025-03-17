#  [vsync](../../README.md) / [thread](../README.md) / [mutex](README.md) / musl.h
_A simplified version of the mutex algorithm in musl libc._ 


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



> **Note:** replace `#include <`[`vsync/thread/mutex.h`](../mutex.h.md)`>` with `#include <`[`vsync/thread/mutex/musl.h`](musl.h.md)`>` in the example above.

> **Note:** on linux compile with `-D_GNU_SOURCE`.


### References:
 [Check mutex implementation in libc](http://musl.libc.org)

> **Note:** It is the normal mutex without support for reentrance. 

---
# Macros 

| Macro | Description |
|---|---|
| [MUSL_MAX_SPIN](musl.h.md#macro-musl_max_spin) | times of spinning before going to the macro.  |

##  Macro `MUSL_MAX_SPIN`

 
_times of spinning before going to the macro._ 


default value is 100, compile with -DMUSL_MAX_SPIN=N to overwrite the default.

> **Note:** spinning is deactivated on verification. 


---
# Functions 

| Function | Description |
|---|---|
| [vmutex_init](musl.h.md#function-vmutex_init) | Initializes the mutex `m`.  |
| [vmutex_acquire](musl.h.md#function-vmutex_acquire) | Acquires the mutex `m`.  |
| [vmutex_release](musl.h.md#function-vmutex_release) | Releases the mutex `m`.  |

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
