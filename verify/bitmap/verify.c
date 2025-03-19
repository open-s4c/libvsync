/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#define NTHREADS 3U

#include <vsync/bitmap/bitmap.h>
#define BITMAP_LEN   (VUINT64_WIDTH + 3U)
#define BITMAP_SLOTS 2U
#define BITMAP_SIZE  (sizeof(vbitmap_t) + (sizeof(vatomic64_t) * BITMAP_SLOTS))
char g_mem[BITMAP_SIZE]; // workaround
vbitmap_t *g_bitmap;

#include <test/boilerplate/test_case.h>

int
main(void)
{
    tc();
    return 0;
}

void
init(void)
{
    vsize_t size = vbitmap_size(BITMAP_LEN);
    ASSERT(BITMAP_SIZE == size);
    g_bitmap = (vbitmap_t *)g_mem;
    ASSERT(g_bitmap);
    vbitmap_init(g_bitmap, BITMAP_LEN, false);
}

void
fini(void)
{
}

void
reg(vsize_t tid)
{
    V_UNUSED(tid);
}

void
dereg(vsize_t tid)
{
    V_UNUSED(tid);
}
