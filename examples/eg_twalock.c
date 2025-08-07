#include <vsync/spinlock/twalock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

TWALOCK_ARRAY_DECL;
twalock_t g_lock = TWALOCK_INIT();

vuint32_t g_x = 0;
vuint32_t g_y = 0;

void *
run(void *args)
{
    twalock_acquire(&g_lock);
    g_x++;
    g_y++;
    twalock_release(&g_lock);

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
