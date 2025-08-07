/*
 * The following example shows how to use the MCS lock. Each threads needs a
 * context, i.e., an instance of `mcs_node_t`. When acquiring or releasing the
 * lock, one needs to pass the lock and the context as arguments. The context
 * of a thread should not be used in multiple MCS locks at the same time.
 */
#include <vsync/spinlock/mcslock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define MAX_THREADS  N
#define EXPECTED_VAL N

mcslock_t g_lock = MCSLOCK_INIT();
mcs_node_t g_msc_nodes[MAX_THREADS];

vuint32_t g_x = 0;
vuint32_t g_y = 0;

void *
run(void *args)
{
    vsize_t tid      = (vsize_t)args;
    mcs_node_t *node = &g_msc_nodes[tid];

    mcslock_acquire(&g_lock, node);
    g_x++;
    g_y++;
    mcslock_release(&g_lock, node);

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
