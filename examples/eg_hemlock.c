#include <vsync/spinlock/hemlock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define MAX_THREADS  N
#define EXPECTED_VAL N

hemlock_t g_lock_a = HEMLOCK_INIT();
hemlock_t g_lock_b = HEMLOCK_INIT();

hem_node_t g_nodes[N];

vuint32_t g_x = 0;
vuint32_t g_y = 0;

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;
    // the same thread context object can be used with multiple instances of
    // hemlock
    hemlock_acquire(&g_lock_a, &g_nodes[tid]);
    hemlock_acquire(&g_lock_b, &g_nodes[tid]);
    g_x++;
    g_y++;
    hemlock_release(&g_lock_b, &g_nodes[tid]);
    hemlock_release(&g_lock_a, &g_nodes[tid]);
    (void)args;
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    // use this function as an alternative to HEMLOCK_INIT
    hemlock_init(&g_lock_a);
    hemlock_init(&g_lock_b);

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
