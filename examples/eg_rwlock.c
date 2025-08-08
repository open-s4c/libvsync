
#include <vsync/spinlock/rwlock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            24
#define EXPECTED_VAL (N / 2)

rwlock_t g_lock = RWLOCK_INIT();
vuint32_t g_x   = 0;
vuint32_t g_y   = 0;

void
writer(void)
{
    rwlock_write_acquire(&g_lock);
    g_x++;
    g_y++;
    rwlock_write_release(&g_lock);
}

void
reader(void)
{
    vuint32_t a = 0;
    vuint32_t b = 0;

    rwlock_read_acquire(&g_lock);
    a = g_x;
    b = g_y;
    rwlock_read_release(&g_lock);

    /* what we read must be consistent */
    ASSERT(a == b);
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;
    if (tid % 2 == 0) {
        reader();
    } else {
        writer();
    }
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
