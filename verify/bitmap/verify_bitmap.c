/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#define NTHREADS 3
#include <test/thread_launcher.h>
#include <vsync/bitmap/bitmap.h>

#define BITMAP_LEN   (VUINT64_WIDTH + 3U)
#define BITMAP_SLOTS 2U
#define BITMAP_SIZE  (sizeof(vbitmap_t) + (sizeof(vatomic64_t) * BITMAP_SLOTS))
char g_mem[BITMAP_SIZE]; // workaround
vbitmap_t *g_bitmap;

void *run(void *args);
void t0(vsize_t);
void t1(vsize_t);
void t2(vsize_t);
void pre(void);
void post(void);
void init(void);
void destroy(void);

#ifdef TEST_CASE
    #include TEST_CASE
#else
    #error "no test case was defined"
#endif

int
main(void)
{
    init();
    pre();
    launch_threads(NTHREADS, run);
    post();
    destroy();
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
destroy(void)
{
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;
    ASSERT(tid < NTHREADS);

    switch (tid) {
        case 0:
            t0(tid);
            break;
        case 1:
            t1(tid);
            break;
        case 2:
            t2(tid);
            break;
        default:
            break;
    }
    return NULL;
}
