
#include <vsync/spinlock/seqcount.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define IT           10
#define EXPECTED_VAL IT

seqcount_t g_seqcount = SEQCOUNT_INIT();
vuint32_t g_x         = 0;
vuint32_t g_y         = 0;

void
writer(void)
{
    for (vsize_t i = 0; i < IT; i++) {
        seqvalue_t seq = seqcount_wbegin(&g_seqcount);
        g_x++;
        g_y++;
        seqcount_wend(&g_seqcount, seq);
    }
}

void
reader(void)
{
    vuint32_t a  = 0;
    vuint32_t b  = 0;
    seqvalue_t s = 0;

    do {
        s = seqcount_rbegin(&g_seqcount);
        a = g_x;
        b = g_y;
    } while (!seqcount_rend(&g_seqcount, s));

    /* what we read must be consistent */
    ASSERT(a == b);
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;
    if (tid == 0) {
        /* one writer at most */
        writer();
    } else {
        reader();
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
