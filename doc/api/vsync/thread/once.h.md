#  [vsync](../README.md) / [thread](README.md) / once.h
_One-time initializer._ 

Calls a callback exactly once even if concurrently called. Callback happens before any thread returns [vonce_call()](once.h.md#function-vonce_call).


### Example:



```c
#include <stdio.h>
#include <pthread.h>
#include <vsync/common/assert.h>
#include <vsync/thread/once.h>

#define N 12

vonce_t g_once            = VONCE_INIT();
vuint64_t g_cnt           = 0;
vuint64_t g_cnt_ret       = 0;
vsize_t g_winner          = 0;
vsize_t g_notified_winner = 0;

void *
callback(void *arg)
{
    vsize_t tid = (vsize_t)(vuintptr_t)arg;
    printf("Thread %zu called me\n", tid);
    // initialize resources
    g_winner = tid;
    g_cnt++;
    return arg;
}

void *
run(void *arg)
{
    vsize_t tid = (vsize_t)(vuintptr_t)arg;

    void *ret = vonce_call(&g_once, callback, (void *)(vuintptr_t)tid);
    if (ret != NULL) {
        ASSERT(((vsize_t)(vuintptr_t)ret) == tid);
        printf("Thread %zu is the one that called callback\n", tid);
        g_notified_winner = tid;
        g_cnt_ret++;
    }
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)(vuintptr_t)(i + 1));
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    ASSERT(g_cnt_ret == g_cnt);
    ASSERT(g_cnt_ret == 1U);
    ASSERT(g_notified_winner == g_winner);
    return 0;
}
```



> **Note:** on linux compile with `-D_GNU_SOURCE`.


### References:
 [One-time initializer](https://www.remlab.net/op/futex-misc.shtml) 

---
# Functions 

| Function | Description |
|---|---|
| [vonce_call](once.h.md#function-vonce_call) | Calls `cb(arg)` once.  |

##  Function `vonce_call`

```c
static void* vonce_call(vonce_t *o, vonce_cb cb, void *arg)
``` 
_Calls_ `cb(arg)` _once._ 


The thread that actually executed the callback gets the return value of the callback. Other threads get NULL back.



**Parameters:**

- `o`: address of once_t object. 
- `cb`: address of callback function. 
- `arg`: argument of `cb`. 


**Returns:** void* return value of `cb(arg)` if the thread actually called `cb`. 

**Returns:** NULL if another thread already called `cb`. 




---
