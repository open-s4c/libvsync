
#include <vsync/spinlock/rec_seqlock.h>
#include <pthread.h>
#include <stdio.h>

#define N            10
#define EXPECTED_VAL (N / 2)

rec_seqlock_t g_lock = REC_SEQLOCK_INIT();
vuint32_t g_x        = 0;
vuint32_t g_y        = 0;

void
writer(vuint32_t tid)
{
    rec_seqlock_acquire(&g_lock, tid);
    // writer acquisition of the lock can be nested
    rec_seqlock_acquire(&g_lock, tid);
    g_x++;
    g_y++;
    rec_seqlock_release(&g_lock);
    rec_seqlock_release(&g_lock);
}

void
reader(void)
{
    vuint32_t a  = 0;
    vuint32_t b  = 0;
    seqvalue_t s = 0;

    do {
        s = rec_seqlock_rbegin(&g_lock);
        a = g_x;
        b = g_y;
    } while (!rec_seqlock_rend(&g_lock, s));

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
        writer((vuint32_t)tid);
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
