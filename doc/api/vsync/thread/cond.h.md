#  [vsync](../README.md) / [thread](README.md) / cond.h
_Condition variable._ 

A very simple condition variable.


### Example:



```c
#include <vsync/thread/mutex.h>
#include <vsync/thread/cond.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

vcond_t g_cnd_count_non_zero;
vmutex_t g_mutex_count;
vuint32_t g_count = 0;

void
dec(void)
{
    for (vsize_t i = 0; i < N - 1; i++) {
        vmutex_acquire(&g_mutex_count);
        while (g_count == 0) {
            vcond_wait(&g_cnd_count_non_zero, &g_mutex_count);
        }
        g_count--;
        vmutex_release(&g_mutex_count);
    }
}

void
inc(void)
{
    vmutex_acquire(&g_mutex_count);
    if (g_count == 0) {
        vcond_signal(&g_cnd_count_non_zero);
    }
    g_count++;
    vmutex_release(&g_mutex_count);
}

void *
run(void *args)
{
    vsize_t id = (vsize_t)args;

    if (id == 0) {
        dec();
    } else {
        inc();
    }
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    vcond_init(&g_cnd_count_non_zero);
    vmutex_init(&g_mutex_count);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    ASSERT(g_count == 0);
    printf("Final value %u\n", g_count);
    return 0;
}
```



> **Note:** include [mutex.h](mutex.h.md) from libvsync before including [cond.h](cond.h.md). Alternatively, users can implement the same interface with pthread_mutex_t or similar and include that to be used by [cond.h](cond.h.md).

> **Note:** on linux compile with `-D_GNU_SOURCE`.


### References:
 [Condition variable with futex](https://www.remlab.net/op/futex-condvar.shtml) 

---
# Functions 

| Function | Description |
|---|---|
| [vcond_init](cond.h.md#function-vcond_init) | Initializes the given condition variable.  |
| [vcond_wait](cond.h.md#function-vcond_wait) | Waits on the given condition variable.  |
| [vcond_signal](cond.h.md#function-vcond_signal) | Signals the condition variable.  |

##  Function `vcond_init`

```c
static void vcond_init(vcond_t *c)
``` 
_Initializes the given condition variable._ 




**Parameters:**

- `c`: address of vcond_t object. 




##  Function `vcond_wait`

```c
static void vcond_wait(vcond_t *c, vmutex_t *m)
``` 
_Waits on the given condition variable._ 


Releases the mutex and waits till the condition variable is signaled, then reacquires the mutex.



**Parameters:**

- `c`: address of vcond_t object. 
- `m`: address of [vmutex_t](structvmutex__t) object. 




##  Function `vcond_signal`

```c
static void vcond_signal(vcond_t *c)
``` 
_Signals the condition variable._ 


Wakes up one sleeping thread waiting on the condition.



**Parameters:**

- `c`: address of vcond_t object. 





---
