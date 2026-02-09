/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_IMPSC_H
#define VSYNC_IMPSC_H

#include <stdlib.h>
#include <vsync/vtypes.h>
#include <assert.h>
#include <test/trace_manager.h>

#define NTRACES   (NTHREADS + 1U)
#define TRACE_LEN 10U

trace_t g_deq_trace;          /* one consumer, one trace */
trace_t g_enq_trace[NTRACES]; /* traces of producers */
trace_t g_final_state;        /* final state of the queue */

static inline vbool_t
collect_cb(void *data, void *args)
{
    ASSERT(data);
    trace_add(&g_final_state, (vuintptr_t)data);
    V_UNUSED(args);
    return true;
}
static inline void
dump_traces(trace_t *total_enq)
{
    trace_print(total_enq, "total_enq");
    trace_print(&g_final_state, "g_final_state");
    trace_print(&g_deq_trace, "g_deq_trace");
}

static inline void
init_traces(void)
{
    trace_init(&g_deq_trace, TRACE_LEN);
    trace_init(&g_final_state, TRACE_LEN);
    for (vsize_t i = 0; i < NTRACES; i++) {
        trace_init(&g_enq_trace[i], TRACE_LEN);
    }
}
static inline void
destroy_traces(void)
{
    trace_destroy(&g_deq_trace);
    trace_destroy(&g_final_state);
    for (vsize_t i = 0; i < NTRACES; i++) {
        trace_destroy(&g_enq_trace[i]);
    }
}
static inline void
print_cb(vuintptr_t k)
{
    fprintf(stderr, "%lu\n", k);
}
static inline void
verify_traces(void)
{
    trace_t total_enq;
    trace_init(&total_enq, TRACE_LEN);
    /* verify */
    for (vsize_t i = 0; i < NTRACES; i++) {
        trace_merge_into(&total_enq, &g_enq_trace[i]);
    }

    vbool_t subtrace = trace_is_subtrace(&total_enq, &g_deq_trace, NULL);
    if (!subtrace) {
        dump_traces(&total_enq);
    }
    ASSERT(subtrace &&
           "Dequeued set of elements is not a subset of enqueued ones.");
    subtrace = trace_is_subtrace(&total_enq, &g_final_state, NULL);
    if (!subtrace) {
        dump_traces(&total_enq);
    }

    ASSERT(subtrace &&
           "Remaining set of elements is not a subset of enqueued ones.");
    trace_merge_into(&g_final_state, &g_deq_trace);
    vbool_t equal = trace_are_eq(&g_final_state, &total_enq, NULL);
    if (!equal) {
        dump_traces(&total_enq);
    }
    ASSERT(equal &&
           "The set of dequeued and remaining elements is not equal to "
           "enqueued ones.");
    trace_destroy(&total_enq);
}

#ifdef MPSC
    #include <vsync/queue/mpsc.h>

vmpsc_t queue;

void
init(void)
{
    vmpsc_init(&queue);
    init_traces();
}

vuintptr_t
deq(void)
{
    vmpsc_node_t *node = NULL;
    vuintptr_t v       = (vuintptr_t)vmpsc_deq(&queue, &node);
    if (v) {
        trace_add(&g_deq_trace, v);
        free(node);
    }
    return v;
}

void
enq(vsize_t tid, vuintptr_t v)
{
    vmpsc_node_t *node = (vmpsc_node_t *)malloc(sizeof(vmpsc_node_t));
    ASSERT(node);
    ASSERT(tid < NTRACES);
    trace_add(&g_enq_trace[tid], v);
    return vmpsc_enq(&queue, node, (void *)v);
}

vbool_t
is_empty(void)
{
    return vmpsc_empty(&queue);
}

vsize_t
get_len(void)
{
    return vmpsc_length(&queue);
}

void
des_cb(vmpsc_node_t *node, void *args)
{
    free(node);
    V_UNUSED(args);
}
void
fini(void)
{
    vmpsc_foreach(&queue, collect_cb, NULL);
    verify_traces();
    vmpsc_destroy(&queue, des_cb, NULL);
    destroy_traces();
}
#elif LEVEL_QUEUE

    #include <vsync/queue/level_queue.h>
    #include <test/vmem_stdlib.h>

void *
lvl_queue_alloc_cb(vsize_t sz, void *arg)
{
    V_UNUSED(arg);
    #if defined(VSYNC_VERIFICATION)
    return vmem_malloc(sz);
    #else
    return vmem_aligned_malloc(VSYNC_CACHELINE_SIZE, sz);
    #endif
}

vmem_lib_t g_mem_lib = {.free_fun   = vmem_free_cb,
                        .malloc_fun = lvl_queue_alloc_cb,
                        .arg        = NULL};
level_queue_t q;

void
init(void)
{
    level_queue_init(&q, &g_mem_lib);
    init_traces();
}

vuintptr_t
deq(void)
{
    vuintptr_t v = (vuintptr_t)level_queue_dequeue(&q);
    if (v) {
        trace_add(&g_deq_trace, v);
    }
    return v;
}

level_queue_prod_t g_prods[NTHREADS];
void
enq(vsize_t tid, vuintptr_t v)
{
    ASSERT(tid < NTHREADS);
    if (g_prods[tid] == NULL) {
        level_queue_prod_init(&q, &g_prods[tid]);
    }
    level_queue_enqueue(&q, &g_prods[tid], (void *)v);
    ASSERT(tid < NTRACES);
    trace_add(&g_enq_trace[tid], v);
}

vbool_t
is_empty(void)
{
    return level_queue_empty(&q);
}

vsize_t
get_len(void)
{
    return level_queue_length(&q);
}

void
fini(void)
{
    level_queue_foreach(&q, collect_cb, NULL);
    verify_traces();
    level_queue_destroy(&q);
    destroy_traces();
}

#endif
#endif
