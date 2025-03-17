/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/bitmap/bitmap.h>
#include <stdlib.h>
#include <test/thread_launcher.h>

#define N         12U
#define IT        (VUINT64_WIDTH + 1U)
#define BIT_COUNT (N * IT)

vbitmap_t *g_bitmap;

void *
run(void *args)
{
    vsize_t tid     = (vsize_t)(vuintptr_t)args;
    vsize_t bit_idx = 0;
    vbool_t set     = false;

    for (vsize_t i = 0; i < IT; i++) {
        bit_idx = (tid * IT) + i;
        set     = vbitmap_get(g_bitmap, bit_idx);
        ASSERT(set == false);
        if (VIS_EVEN(bit_idx)) {
            vbitmap_set_bit(g_bitmap, bit_idx);
            set = vbitmap_get(g_bitmap, bit_idx);
            ASSERT(set == true);
        } else {
            vbitmap_clr_bit(g_bitmap, bit_idx);
            set = vbitmap_get(g_bitmap, bit_idx);
            ASSERT(set == false);
        }
    }

    return 0;
}

int
main(void)
{
    vbool_t set         = false;
    vsize_t size        = 0;
    vbitmap_iter_t iter = {0};
    vsize_t idx         = 0;
    vsize_t i           = 0;

    size     = vbitmap_size(BIT_COUNT);
    g_bitmap = malloc(size);
    vbitmap_init(g_bitmap, BIT_COUNT, false);
    launch_threads(N, run);

    for (i = 0; i < BIT_COUNT; i++) {
        set = vbitmap_get(g_bitmap, i);
        ASSERT(set == VIS_EVEN(i));
    }

    vbitmap_iter_init(g_bitmap, &iter);

    i = 0;
    while (vbitmap_iter_next(&iter, &idx)) {
        ASSERT(i == idx);
        i += 2;
    }
    ASSERT(i == BIT_COUNT);

    free(g_bitmap);
    g_bitmap = NULL;
    return 0;
}
