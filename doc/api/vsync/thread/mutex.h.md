#  [vsync](../README.md) / [thread](README.md) / mutex.h
_Futex-based mutex._ 

This file includes the default mutex implementation. See [mutex/slim.h](mutex/slim.h.md) for details.


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

 


---
