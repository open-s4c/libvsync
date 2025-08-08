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
