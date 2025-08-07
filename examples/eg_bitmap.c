#include <vsync/bitmap/bitmap.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define WRITER_COUNT 11U
#define N            (WRITER_COUNT + 1)
#define BIT_COUNT    (VUINT64_WIDTH * WRITER_COUNT)

vbitmap_t *g_bitmap;

void
reader(void)
{
    vsize_t idx          = 0;
    vbitmap_iter_t iter  = {0};
    vsize_t signal_count = 0;
    vsize_t expected     = BIT_COUNT - WRITER_COUNT;

    while (signal_count < expected) {
        signal_count = 0;
        vbitmap_iter_init(g_bitmap, &iter);

        while (vbitmap_iter_next(&iter, &idx)) {
            signal_count++;
        }
        printf("%zu bits were raised, expected %zu\n", signal_count, expected);
    }
}

void
writer(vsize_t tid)
{
    vbool_t set = false;

    // each writer sets 64 bits
    vsize_t from = tid * VUINT64_WIDTH;
    vsize_t to   = from + VUINT64_WIDTH - 1;

    vbitmap_set_range(g_bitmap, from, to);

    set = vbitmap_get(g_bitmap, from);
    ASSERT(set == true);

    vbitmap_clr_bit(g_bitmap, from);
    set = vbitmap_get(g_bitmap, from);
    ASSERT(set == false);

    vbitmap_set_bit(g_bitmap, from);
    set = vbitmap_get(g_bitmap, from);
    ASSERT(set == true);
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;
    if (tid == WRITER_COUNT) {
        reader();
    } else {
        writer(tid);
    }
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    vsize_t size = vbitmap_size(BIT_COUNT);
    g_bitmap     = malloc(size);
    vbitmap_init(g_bitmap, BIT_COUNT, false);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    free(g_bitmap);

    return 0;
}
