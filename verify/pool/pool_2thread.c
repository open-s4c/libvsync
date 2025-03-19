/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <vsync/vtypes.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <vsync/common/assert.h>
#include <vsync/common/dbg.h>
#include <stdlib.h>

#include <vsync/atomic.h>
#include <vsync/queue/bounded_spsc.h>
#include <test/thread_barrier.h>

#define RUNNING_NUM         5
#define CACHEDP_MAX_THREAD  2
#define CACHEDP_NUM_ENTRIES 2
#define CACHEDP_ENTRY_SIZE  8U

#include <vsync/pool/cached_pool.h>

#define CLSZ 128
#define QSZ  128

struct thread_data {
    unsigned int tid;
    vthread_barrier_t *barrier;
    long unsigned int iters;
} __attribute__((aligned(CLSZ)));
struct thread_data td[CACHEDP_MAX_THREAD];

bounded_spsc_t q[CACHEDP_MAX_THREAD][CACHEDP_MAX_THREAD];
struct queue_data_s {
    void *buf[QSZ];
};
struct queue_data_s queue_data[CACHEDP_MAX_THREAD][CACHEDP_MAX_THREAD];

uint8_t buf[cached_pool_memsize(CACHEDP_MAX_THREAD, CACHEDP_NUM_ENTRIES,
                                CACHEDP_ENTRY_SIZE)];

vatomic64_t finished[CACHEDP_MAX_THREAD];
vatomic64_t stop;

void
init()
{
    cached_pool_t *t = cached_pool_init(
        buf, CACHEDP_MAX_THREAD, CACHEDP_NUM_ENTRIES, CACHEDP_ENTRY_SIZE);
    ASSERT(t->conf.threshold_init == 1);
    vatomic64_write(&stop, 0);
    for (int i = 0; i < CACHEDP_MAX_THREAD; i++) {
        for (int j = 0; j < CACHEDP_MAX_THREAD; j++) {
            bounded_spsc_init(&q[i][j], (void **)&queue_data[i][j], QSZ);
        }
    }
}

void *
start_thread(void *arg)
{
    struct thread_data *td = (struct thread_data *)arg;
    vuint32_t id           = td->tid;
    cached_pool_t *t       = (cached_pool_t *)buf;

    vatomic64_init(&finished[id], false);
    vthread_barrier_wait(td->barrier);

    td->iters                 = 0;
    vuint64_t current_enq_idx = id;
    vuint64_t current_deq_idx =
        ((id + CACHEDP_MAX_THREAD) / 2) % CACHEDP_MAX_THREAD;
    void *addr;
    for (int i = 0; i < RUNNING_NUM; i++) {
        /* malloc */
        if (id) {
            addr = cached_pool_alloc(t, id);
            if (addr) {
                vbool_t success = false;
                for (vuint64_t i = 0; i < CACHEDP_MAX_THREAD; i++) {
                    if (current_enq_idx != id &&
                        bounded_spsc_enq(&q[id][current_enq_idx], addr) ==
                            QUEUE_BOUNDED_OK) {
                        success = true;
                        break;
                    }
                    current_enq_idx =
                        (current_enq_idx + 1) % CACHEDP_MAX_THREAD;
                }
                if (success) {
                    td->iters++;
                } else {
                    cached_pool_free(t, id, addr);
                }
            }
        }
        /* free */
        else {
            vbool_t success = false;
            for (vuint64_t i = 0; i < CACHEDP_MAX_THREAD; i++) {
                if (current_deq_idx != id &&
                    bounded_spsc_deq(&q[current_deq_idx][id], &addr) ==
                        QUEUE_BOUNDED_OK) {
                    success = true;
                    cached_pool_free(t, id, addr);
                    break;
                }
                current_deq_idx = (current_deq_idx + 1) % CACHEDP_MAX_THREAD;
            }

            if (success) {
                td->iters++;
            }
        }
    }
    return NULL;
}

int
main(void)
{
    init();

    vthread_barrier_t barrier;
    if (vthread_barrier_init(&barrier, NULL, CACHEDP_MAX_THREAD + 1)) {
        DBG_RED("Error: could not initialize barrier");
        exit(-1);
    }

    pthread_t threads[CACHEDP_MAX_THREAD];
    for (int i = 0; i < CACHEDP_MAX_THREAD; i++) {
        td[i].tid     = i;
        td[i].barrier = &barrier;
        td[i].iters   = 0;
        if (pthread_create(&threads[i], NULL, start_thread, (void *)&td[i]) !=
            0) {
            DBG_RED("Error: could not create thread %d", i);
            exit(-1);
        }
    }

    vthread_barrier_wait(&barrier);

    long unsigned total_iters = 0;
    for (int i = 0; i < CACHEDP_MAX_THREAD; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            DBG_RED("Error: could not join thread %d\n", i);
            exit(-1);
        }
        total_iters += td[i].iters;
    }
    DBG_GREEN("Total iterations: %lu\n", total_iters);
}
