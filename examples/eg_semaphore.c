
#include <vsync/spinlock/semaphore.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N             12
#define EXPECTED_VAL  N
#define NUM_RESOURCES 1

semaphore_t g_semaphore = SEMAPHORE_INIT(NUM_RESOURCES);
vuint32_t g_x           = 0;
vuint32_t g_y           = 0;

void *
run(void *args)
{
    semaphore_acquire(&g_semaphore, NUM_RESOURCES);
    g_x++;
    g_y++;
    semaphore_release(&g_semaphore, NUM_RESOURCES);
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
