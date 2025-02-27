/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#if !defined(_GNU_SOURCE)
    #define _GNU_SOURCE
#endif
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <vsync/vtypes.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <vsync/common/assert.h>
#include <stdlib.h>
#include <vsync/common/dbg.h>
#include <vsync/atomic.h>
#include <vsync/queue/bounded_spsc.h>
#include <vsync/pool/cached_pool.h>

#define CACHEDP_NUM_ENTRIES 4096U
#define CACHEDP_MAX_THREAD  32U
#define CACHEDP_ENTRY_SIZE  8U

#define RUNNING_TIME_SECOND 5
#define MAX_THREADS         8

#define CLSZ 128
#define QSZ  128

struct thread_data {
    unsigned int tid;
    pthread_barrier_t *barrier;
    long unsigned int iters;
} __attribute__((aligned(CLSZ)));
struct thread_data td[MAX_THREADS];

bounded_spsc_t q[MAX_THREADS][MAX_THREADS];
struct queue_data_s {
    void *buf[QSZ];
};
struct queue_data_s queue_data[MAX_THREADS][MAX_THREADS];

uint8_t buf[cached_pool_memsize(CACHEDP_MAX_THREAD, CACHEDP_NUM_ENTRIES,
                                CACHEDP_ENTRY_SIZE)];

vatomic64_t finished[MAX_THREADS];
vatomic64_t stop;

static inline void
set_affinity(int id)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(id, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        DBG_RED("Error: failed to set affinity\n");
    }
}

double
now_ms()
{
    struct timeval now;
    int ret;

    ret = gettimeofday(&now, NULL);
    if (ret != 0) {
        DBG_RED("Error: could not get time of day\n");
        exit(-1);
    }
    return (double)now.tv_sec * 1000.0 + (double)now.tv_usec / 1000.0;
}

void
init()
{
    cached_pool_init(buf, CACHEDP_MAX_THREAD, CACHEDP_NUM_ENTRIES,
                     CACHEDP_ENTRY_SIZE);
    vatomic64_write(&stop, 0);
    for (int i = 0; i < MAX_THREADS; i++) {
        for (int j = 0; j < MAX_THREADS; j++) {
            bounded_spsc_init(&q[i][j], (void **)&queue_data[i][j], QSZ);
        }
    }
}

void *
start_thread(void *arg)
{
    struct thread_data *td = (struct thread_data *)arg;
    vuint32_t id           = td->tid;
    set_affinity(id);
    cached_pool_t *t = (cached_pool_t *)buf;

    vatomic64_init(&finished[id], false);
    pthread_barrier_wait(td->barrier);

    td->iters                 = 0;
    vuint64_t current_enq_idx = id;
    vuint64_t current_deq_idx = ((id + MAX_THREADS) / 2) % MAX_THREADS;
    void *addr;
    while (!vatomic64_read_rlx(&stop)) {
        /* malloc */
        for (int i = 0; i < 100; i++) {
            addr = cached_pool_alloc(t, id);
            if (addr) {
                vbool_t success = false;
                for (vuint64_t i = 0; i < MAX_THREADS; i++) {
                    if (current_enq_idx != id &&
                        bounded_spsc_enq(&q[id][current_enq_idx], addr) ==
                            QUEUE_BOUNDED_OK) {
                        success = true;
                        break;
                    }
                    current_enq_idx = (current_enq_idx + 1) % MAX_THREADS;
                }
                if (success) {
                    td->iters++;
                } else {
                    cached_pool_free(t, id, addr);
                }
            } else {
                break;
            }
        }

        /* free */
        for (int i = 0; i < 100; i++) {
            vbool_t success = false;
            for (vuint64_t i = 0; i < MAX_THREADS; i++) {
                if (current_deq_idx != id &&
                    bounded_spsc_deq(&q[current_deq_idx][id], &addr) ==
                        QUEUE_BOUNDED_OK) {
                    success = true;
                    cached_pool_free(t, id, addr);
                    break;
                }
                current_deq_idx = (current_deq_idx + 1) % MAX_THREADS;
            }

            if (success) {
                td->iters++;
            } else {
                break;
            }
        }
    }

    /* free the rest */
    vatomic64_write_rlx(&finished[id], true);
    for (vuint64_t current_deq_idx = 0; current_deq_idx < MAX_THREADS;
         current_deq_idx++) {
        while (!vatomic64_read_rlx(&finished[current_deq_idx]))
            ;
        while (current_deq_idx != id &&
               bounded_spsc_deq(&q[current_deq_idx][id], &addr) ==
                   QUEUE_BOUNDED_OK) {
            cached_pool_free(t, id, addr);
            td->iters++;
        }
    }

    // TODO: check it is empty
    return NULL;
}

int
main(void)
{
    init();

    pthread_barrier_t barrier;
    if (pthread_barrier_init(&barrier, NULL, MAX_THREADS + 1)) {
        DBG_RED("Error: could not initialize barrier");
        exit(-1);
    }

    pthread_t threads[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++) {
        td[i].tid     = i;
        td[i].barrier = &barrier;
        td[i].iters   = 0;
        if (pthread_create(&threads[i], NULL, start_thread, (void *)&td[i]) !=
            0) {
            DBG_RED("Error: could not create thread %d", i);
            exit(-1);
        }
    }

    double start_ms = now_ms();
    pthread_barrier_wait(&barrier);

    if (RUNNING_TIME_SECOND) {
        sleep(RUNNING_TIME_SECOND);
        vatomic64_write(&stop, 1);
    }

    long unsigned total_iters = 0;
    for (int i = 0; i < MAX_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            DBG_RED("Error: could not join thread %d\n", i);
            exit(-1);
        }
        total_iters += td[i].iters;
    }

    double elapsed_ms = now_ms() - start_ms;

    printf("\n-------------------\n");
    printf("performance summary\n");
    printf("-------------------\n");
    for (int i = 0; i < MAX_THREADS; i++) {
        printf("t%d: %lu iters\n", i, td[i].iters);
    }

    printf("time: %.3lf ms\n", elapsed_ms);
    printf("iter: %lu iters\n", total_iters);
    printf("single thread throughput: %.3lf iter/s\n",
           1000.0 * total_iters / elapsed_ms / MAX_THREADS);
    printf("throughput: %.3lf iter/s\n", 1000.0 * total_iters / elapsed_ms);
}
