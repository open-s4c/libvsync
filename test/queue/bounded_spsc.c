/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <pthread.h>
#include <vsync/common/assert.h>
#include <stdlib.h>
#include <vsync/queue/bounded_spsc.h>
#include <vsync/atomic.h>
#include <test/thread_launcher.h>

#define NTHREADS   2
#define QUEUE_SIZE 2
#define VALUES     3

typedef struct point_s {
    vuint32_t x;
    vuint32_t y;
} point_t;

void *g_buf[QUEUE_SIZE];
bounded_spsc_t g_queue;
vatomic32_t g_cs_x;

point_t *g_points[VALUES];

void *
run(void *arg)
{
    vsize_t tid = (vsize_t)(vuintptr_t)arg;

    if (tid == 0) {
        for (vsize_t i = 0; i < VALUES; i++) {
            point_t *point = g_points[i];
            point->x       = 1;
            point->y       = 1;
            await_while (bounded_spsc_enq(&g_queue, point) != QUEUE_BOUNDED_OK)
                ;
        }
    } else {
        for (int i = 0; i < VALUES; i++) {
            void *r = NULL;
            await_while (bounded_spsc_deq(&g_queue, &r) != QUEUE_BOUNDED_OK)
                ;
            point_t *point = (point_t *)r;
            if (point) {
                ASSERT(point->x == point->y);
                ASSERT(point->x == 1);
                free(point);
            } else {
                ASSERT(0 && "dequeued NULL");
            }
        }
    }
    return NULL;
}

int
main(void)
{
    bounded_spsc_init(&g_queue, g_buf, QUEUE_SIZE);
    for (vsize_t i = 0; i < VALUES; i++) {
        g_points[i] = malloc(sizeof(point_t));
        if (g_points[i] == NULL) {
            ASSERT(0 && "allocation failed");
        }
    }
    launch_threads(NTHREADS, run);
    return 0;
}
