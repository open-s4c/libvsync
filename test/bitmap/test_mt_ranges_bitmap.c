/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/bitmap/bitmap.h>
#include <stdlib.h>
#include <test/thread_launcher.h>

#define N          12U
#define BIT_COUNT  (92274688UL)
#define PER_THREAD (BIT_COUNT / N)
#define REMAINDER  (BIT_COUNT % N)

#define RANGES_LEN 3
vsize_t g_clr_ranges[RANGES_LEN][2] = {{5, 8},
                                       {64, 67},
                                       {BIT_COUNT - 1, BIT_COUNT - 1}};

vbitmap_t *g_bitmap;

vbool_t
_in_clr_range(vsize_t idx)
{
    for (vsize_t i = 0; i < RANGES_LEN; i++) {
        if (idx >= g_clr_ranges[i][0] && idx <= g_clr_ranges[i][1]) {
            return true;
        }
    }
    return false;
}

void *
run(void *args)
{
    vsize_t tid      = (vsize_t)(vuintptr_t)args;
    vsize_t num_bits = PER_THREAD;

    vsize_t from = tid * num_bits;
    vsize_t to   = from + num_bits + (tid == (N - 1) ? REMAINDER - 1 : 0);

    if (tid != N - 1) {
        to += 1; // let ranges intersect
    }

    DBG_BLUE("T%zu: handles  [%zu, %zu]", tid, from, to);

    if (to >= BIT_COUNT) {
        DBG_RED("T%zu, %zu %lu", tid, to, BIT_COUNT);
        ASSERT(to < BIT_COUNT);
    }

    vbitmap_set_range(g_bitmap, from, to);
    for (vsize_t i = from; i <= to; i++) {
        vbool_t set = vbitmap_get(g_bitmap, i);
        if (!_in_clr_range(i)) {
            ASSERT(set);
        }
    }

    for (vsize_t i = 0; i < RANGES_LEN; i++) {
        vbitmap_clr_range(g_bitmap, g_clr_ranges[i][0], g_clr_ranges[i][1]);
    }

    return 0;
}

int
main(void)
{
    vsize_t size = 0;
    size         = vbitmap_size(BIT_COUNT);
    g_bitmap     = malloc(size);
    vbitmap_init(g_bitmap, BIT_COUNT, false);

    launch_threads(N, run);

    for (vsize_t i = 0; i < BIT_COUNT; i++) {
        vbool_t set = vbitmap_get(g_bitmap, i);
        if (_in_clr_range(i)) {
            ASSERT(!set);
        } else {
            ASSERT(set);
        }
    }

    free(g_bitmap);
    g_bitmap = NULL;
    return 0;
}
