
#include <vsync/spinlock/rec_mcslock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

rec_mcslock_t g_lock = REC_MCSLOCK_INIT();
mcs_node_t g_nodes[N];

vuint32_t g_x = 0;
vuint32_t g_y = 0;

void *
run(void *args)
{
    vuint32_t tid    = (vuint32_t)(vsize_t)args;
    mcs_node_t *node = &g_nodes[tid];
    // can be acquired multiple times without causing dead-lock
    rec_mcslock_acquire(&g_lock, tid, node);
    rec_mcslock_acquire(&g_lock, tid, node);
    g_x++;
    g_y++;
    rec_mcslock_release(&g_lock, node);
    rec_mcslock_release(&g_lock, node);

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
