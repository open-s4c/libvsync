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
