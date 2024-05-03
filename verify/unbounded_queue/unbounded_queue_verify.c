/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <vsync/common/verify.h>
#include <test/thread_launcher.h>
#ifndef NTHREADS
    #define NTHREADS 3
#endif
#include <test/queue/ub/queue_interface.h>

/**
 * LF queue verification time
 * C11
 * Number of complete executions explored: 1301796
 * Number of blocked executions seen: 2083650 (23592 mooted)
 * Total wall-clock time: 1306.25s
 * -imm -mo
 * No errors were detected.
 * Number of complete executions explored: 1301796
 * Number of blocked executions seen: 2083650 (24492 mooted)
 * Total wall-clock time: 4527.94s
 */

queue_t g_queue;

#define MAX_LEN 5
vuint64_t g_final_state[MAX_LEN];
vsize_t g_len = 0;

/* Prototypes */
void init(void);
void destroy(void);
void verify(void); // body should exist in the test case
void *run(void *args);
void enq(vsize_t, vuint64_t k, char lbl);
data_t *deq(vsize_t);
void get_final_state(void *qnode);
vbool_t empty(vsize_t);
void *run(void *args);

/* test case */
#if defined(TEST_CASE)
    #include TEST_CASE
#else
    #error "no test case defined!"
#endif

int
main(void)
{
    init();
    launch_threads(NTHREADS, run);
    /* we extract the final state of the queue exploiting print */
    queue_print(&g_queue, get_final_state);
    verify();
    destroy();
    ASSERT(vmem_no_leak());
    return 0;
}

void
init()
{
    queue_init(&g_queue);
#if defined(VQUEUE_UB_LF_RECYCLE) && defined(VSYNC_VERIFICATION)
    aligned_alloc_init(sizeof(queue_node_t));
#endif

    queue_register(0, &g_queue);
#if defined(KICKOUT_SENTINEL)
    // kick out static sentinel
    enq(0, 0, 'a');
    void *data = deq(0);
    free(data);
#endif

#if defined(INIT_WITH_N_NODES)
    char lbl = 'a';
    for (vsize_t i = 1; i <= INIT_WITH_N_NODES; i++, lbl++) {
        enq(0, i, lbl);
    }
#endif
    queue_deregister(0, &g_queue);
}

void
destroy(void)
{
    queue_destroy(&g_queue);
}

void
enq(vsize_t tid, vuint64_t k, char lbl)
{
    queue_enq(tid, &g_queue, k, lbl);
}

data_t *
deq(vsize_t tid)
{
    return queue_deq(tid, &g_queue);
}

vbool_t
empty(vsize_t tid)
{
    return queue_empty(tid, &g_queue);
}

/**
 * exploits print to extract the final state
 *
 * @param qnode
 */
void
get_final_state(void *data)
{
    ASSERT(data);
    data_t *node = data;
    ASSERT(g_len < MAX_LEN);
    g_final_state[g_len++] = node->key;
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;
    queue_register(tid, &g_queue);
    switch (tid) {
        case 0:
            t1(tid);
            break;
        case 1:
            t2(tid);
            break;
        case 2:
            t3(tid);
            break;
        default:
            ASSERT("currently only 3 threads are supported");
    }
    queue_deregister(tid, &g_queue);
    return NULL;
}
