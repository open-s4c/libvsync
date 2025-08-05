/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/queue/vqueue_prio_heap_based.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define IT 2
#define N  3

typedef struct data_s {
    vsize_t id;
} data_t;

__thread vuint32_t my_tid; /* this is just an example, thread local storage is
                              not the best option  */
vqueue_prio_t g_queue;

/* a unique number for the tid */
vuint32_t
get_tid_cb(void)
{
    return my_tid;
}

void
destroy_cb(void *node, void *args)
{
    /* the cast is not necessary it is just to emphasize that the
                 object you get here is the same as what has been inserted */
    data_t *data = (data_t *)node;
    free(data);
    (void)args;
}

void *
run(void *args)
{
    data_t *data = NULL;
    /* this cast is not recommended it is just to demonstrate that each thread
     * has a unique id */
    my_tid = (vuint32_t)(vuintptr_t)args;

    for (vsize_t i = 0; i < IT; i++) {
        data = (data_t *)vqueue_prio_remove_min(&g_queue);
        if (data == NULL) {
            data            = malloc(sizeof(data_t));
            data->id        = i;
            vbool_t success = vqueue_prio_add(&g_queue, data, i);
            if (success) {
                printf("[T%u] enq %zu\n", my_tid, data->id);
            } else {
                printf("The queue is full");
                free(data);
            }
        } else {
            printf("[T%u] deq %zu\n", my_tid, data->id);
            free(data);
        }
    }

    return NULL;
}

int
main(void)
{
    pthread_t threads[N];
    vqueue_prio_init(&g_queue, get_tid_cb);
    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }
    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    vqueue_prio_destroy(&g_queue, destroy_cb, NULL);
    return 0;
}
