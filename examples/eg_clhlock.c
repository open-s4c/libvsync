#include <vsync/spinlock/clhlock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define MAX_THREADS  N
#define EXPECTED_VAL N

clhlock_t g_lock;
clh_node_t g_nodes[N];

vuint32_t g_x = 0;
vuint32_t g_y = 0;

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;

    clhlock_node_init(&g_nodes[tid]);

    clhlock_acquire(&g_lock, &g_nodes[tid]);
    g_x++;
    g_y++;
    clhlock_release(&g_lock, &g_nodes[tid]);

    (void)args;
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    clhlock_init(&g_lock);

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
