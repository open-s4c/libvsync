/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <test/trace_manager.h>
#include <test/thread_launcher.h>
#include <test/queue/ub/queue_interface.h>
#include <vsync/utils/math.h>

#ifndef NTHREADS
    #define NTHREADS 1000
#endif

#ifndef IT
    #define IT 100
#endif

#ifndef TRACE_CAPACITY
    #define TRACE_CAPACITY 100
#endif

trace_t trace_keys[NTHREADS];

queue_t g_queue;

vatomic64_t g_key = VATOMIC_INIT(1);

void init(void);
void enq(vsize_t tid);
void deq(vsize_t tid);
void destroy(void);
void *run(void *args);

vbool_t verify_traces(void);

int
main(void)
{
#ifdef VQUEUE_UB_LF
    ASSERT(NTHREADS <= SMR_MAX_NTHREADS);
#endif
    init();
    launch_threads(NTHREADS, run);
    vbool_t is_sound = verify_traces();
    ASSERT(is_sound && "traces are not sound");
    destroy();
    V_UNUSED(is_sound);
    return 0;
}

void
init(void)
{
    queue_init(&g_queue);
}

void
destroy(void)
{
    queue_destroy(&g_queue);
}

void
enq(vsize_t tid)
{
    vuint64_t key = 0;
    vsize_t i     = 0;
    char c        = 'A';

    ASSERT(VIS_EVEN(tid));

    for (i = 0; i < IT; i++) {
        /* we need to save the used key to add it to the trace, as we cannot use
         * node->key again since it can be freed when data of that node has been
         * freed */
        key = vatomic64_get_inc_rlx(&g_key);
        ASSERT(key && "keys start from 1, how come this is zero");
        queue_enq(tid, &g_queue, key, c++);
        trace_add(&trace_keys[tid], key);
    }
}

void
deq(vsize_t tid)
{
    ASSERT(VIS_ODD(tid));

    data_t *node = NULL;
    vsize_t i    = 0;

    for (i = 0; i < IT; i++) {
        if (!queue_empty(tid, &g_queue)) {
            node = queue_deq(tid, &g_queue);
            if (node) {
                trace_add(&trace_keys[tid], node->key);
                free(node);
            }
        }
    }
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;

    queue_register(tid, &g_queue);

    trace_init(&trace_keys[tid], TRACE_CAPACITY);

    if (VIS_EVEN(tid)) {
        enq(tid);
    } else {
        deq(tid);
    }
    queue_deregister(tid, &g_queue);
    return NULL;
}

vbool_t
verify_trace_unit(trace_unit_t *unit)
{
    if (unit->count != 1) {
        DBG_YELLOW("key: %" VUINTPTR_FORMAT " count = %zu != 1 \n", unit->key,
                   unit->count);
        ASSERT(unit->key);
    }
    return unit->count == 1;
}

static void
extract_final_state(queue_node_t *node, void *arg)
{
    trace_t *trace = arg;
    data_t *mock   = node->data;
    trace_add(trace, mock->key);
}

void
verify_counts(trace_t *deq_trace, trace_t *enq_trace, trace_t *final_trace)
{
    vsize_t enqs     = trace_get_length(enq_trace);
    vsize_t deqs     = trace_get_length(deq_trace);
    vsize_t final    = trace_get_length(final_trace);
    vuint64_t allocs = vmem_get_alloc_count();
    vuint64_t frees  = vmem_get_free_count();

    DBG_YELLOW("#enq: %zu #allocs: %" VUINT64_FORMAT
               "\n#deqs: %zu #frees: %" VUINT64_FORMAT
               "\n#remaining items "
               "%zu\n",
               enqs, allocs, deqs, frees, final);

    if (enqs != allocs) {
        vbool_t passed_verify = trace_verify(enq_trace, verify_trace_unit);
        ASSERT(passed_verify);
        V_UNUSED(passed_verify);
    }
    // for each enqueue there is an alloc
    ASSERT(enqs == allocs);
    // no frees without queues
    if (deqs == 0) {
        ASSERT(frees == 0);
    } else {
        /* because the initial sentinel in the queue is never to be freed
         * (statically allocated) there is one dequeue without an associated
         * free. Note that SMR is still active in unbounded queue, it could be
         * there is still something in the retire list that has not been freed
         * yet, that is why this assertion is >= if the SMR thread had stopped,
         * then we would have used == */
        ASSERT(deqs >= frees + 1);
    }
    // # of enqueues should equal to the number of dequeued nodes + number of
    // remaining nodes in the queue (those that were enqueued but never
    // dequeued)
    ASSERT(enqs == (deqs + final));
}

vbool_t
verify_traces(void)
{
    vsize_t tid = 0;
    trace_t collect_dequeues;
    trace_t collect_enqueues;
    trace_t final_state;
    vbool_t success = false;
    vsize_t len     = 0;

    trace_init(&collect_dequeues, TRACE_CAPACITY);
    trace_init(&collect_enqueues, TRACE_CAPACITY);
    trace_init(&final_state, TRACE_CAPACITY);

    for (tid = 0; tid < NTHREADS; tid++) {
        // go through dequeue traces
        if (VIS_EVEN(tid)) {
            trace_merge_into(&collect_enqueues, &trace_keys[tid]);
            len = trace_get_length(&trace_keys[tid]);
            ASSERT(len == IT && "all enqueues must succeed");
        } else {
            trace_merge_into(&collect_dequeues, &trace_keys[tid]);
        }
        trace_destroy(&trace_keys[tid]);
    } // threads

    // extract the trace of the remaining nodes int he queue
    queue_visit_nodes(&g_queue, extract_final_state, &final_state);

    verify_counts(&collect_dequeues, &collect_enqueues, &final_state);

    // very that all dequeued nodes has been dequeued once
    success = trace_verify(&collect_dequeues, verify_trace_unit);

    trace_destroy(&collect_dequeues);
    trace_destroy(&collect_enqueues);
    trace_destroy(&final_state);

    V_UNUSED(len);
    return success;
}
