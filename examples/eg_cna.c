
#include <vsync/spinlock/cnalock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12U
#define EXPECTED_VAL N

cnalock_t g_lock      = CNALOCK_INIT();
cna_node_t g_nodes[N] = {0};

vuint32_t g_x = 0;
vuint32_t g_y = 0;

typedef struct eg_args {
    vsize_t tid;
    vuint32_t numa_node;
} eg_args_t;

void *
run(void *args)
{
    eg_args_t *t_args = (eg_args_t *)args;
    // Bind the thread to run only on the specified numa node, e.g.
    // with `sched_setaffinity`.

    // an example critical section.
    cnalock_acquire(&g_lock, &g_nodes[t_args->tid], t_args->numa_node);
    g_x++;
    g_y++;
    cnalock_release(&g_lock, &g_nodes[t_args->tid], t_args->numa_node);
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];
    eg_args_t args[N];

    for (vsize_t i = 0; i < N; i++) {
        args[i].tid = i;
        // In a real-world program you would probably use `libnuma` to detect
        // how many numa nodes there are, and have a more elaborate strategy
        // for binding threads to certain cores / nodes.
        args[i].numa_node = i % 2;
        pthread_create(&threads[i], NULL, run, (void *)&args[i]);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    ASSERT(g_x == EXPECTED_VAL);
    ASSERT(g_x == g_y);
    printf("Final value %u\n", g_x);
    return 0;
}
