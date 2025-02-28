/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_QUEUE_INTERFACE
#define VSYNC_QUEUE_INTERFACE

/**
 * Based on the define chooses which queue, to include for testing
 *
 */
#include <vsync/common/compiler.h>
#if defined(VQUEUE_UB_LF)
    #define VGDUMP_ENABLE_BUFF
    #include <vsync/queue/unbounded_queue_lf.h>
    #include <test/smr/ismr.h>

    #define VQUEUE_UB_NAME "LF"
#elif defined(VQUEUE_UB_LF_RECYCLE)
    #include <vsync/queue/unbounded_queue_lf_recycle.h>
    #include <test/smr/ismr_none.h>
    #define VQUEUE_UB_NAME "RECYCLE_LF"
    #define TRACE_CAPACITY 128
    #if defined(VSYNC_VERIFICATION)
        #define ALLOC_PER_THREAD 2
        #include <test/queue/ub/aligned_alloc.h>
    #endif
    #include <test/queue/ub/queue_recycle_pool.h>
#elif defined(VQUEUE_UB_TOTAL)
    #include <vsync/queue/unbounded_queue_total.h>
    #include <test/smr/ismr_none.h>
    #define VQUEUE_UB_NAME "TOTAL"
#else
    #error "no queue implementation is defined/included"
#endif

#include <test/vmem_stdlib.h>


#if defined(VQUEUE_UB_LF)
vsize_t g_cleaner_tid = 0;
#endif

typedef vqueue_ub_t queue_t;
typedef vqueue_ub_node_t queue_node_t;
typedef void (*queue_node_free)(vqueue_ub_node_t *, void *);


typedef struct data_s {
    vuint64_t key;
    char lbl;
} data_t;

static inline void _queue_free(smr_node_t *node, void *arg);
static inline void
_queue_retire(queue_node_t *node, void *arg)
{
#if defined(VQUEUE_UB_LF)
    ismr_retire(&node->smr_node, _queue_free, true);
#elif defined(VQUEUE_UB_LF_RECYCLE)
    node_pool_put(node);
#else
    vmem_free(node);
#endif
    V_UNUSED(arg);
}

static inline void
_queue_destroy(queue_node_t *node, void *arg)
{
#if defined(VSYNC_VERIFICATION) && defined(VQUEUE_UB_LF_RECYCLE)
    aligned_alloc_free(node);
#else
    vmem_free(node);
#endif
    V_UNUSED(arg);
}

static inline void
_queue_free(smr_node_t *node, void *arg)
{
#if defined(VQUEUE_UB_LF)
    V_UNUSED(arg);
    queue_node_t *qnode = V_CONTAINER_OF(node, queue_node_t, smr_node);
    vmem_free(qnode);
#else
    ASSERT(0 && "should not be possible");
    V_UNUSED(node, arg);
#endif
}

static inline void
queue_print_callback(void *node)
{
    data_t *data = node;
    printf("node: %p with key: %" VUINT64_FORMAT ", and label %c\n", data,
           data->key, data->lbl);
}

#if defined(VQUEUE_UB_LF_RECYCLE)
locked_trace_t pool_trace;
static inline vbool_t
_pool_destroy_cb(trace_unit_t *unit)
{
    vqueue_ub_node_t *curr = (vqueue_ub_node_t *)unit->key;
    node_pool_destroy(curr, _queue_destroy);
    return true;
}
#endif

static inline void
queue_init(queue_t *q)
{
    ismr_init();
    vqueue_ub_init(q);
#if defined(VQUEUE_UB_LF) && !defined(VSYNC_VERIFICATION)
    ismr_start_cleaner();
#endif
#if defined(VQUEUE_UB_LF_RECYCLE)
    locked_trace_init(&pool_trace, TRACE_CAPACITY);
#endif
}

static inline void
queue_register(vsize_t tid, queue_t *q)
{
    ismr_reg(tid);
    V_UNUSED(q);
}
#if defined(VQUEUE_UB_LF_RECYCLE)
static inline void
_queue_save_local_pool(void)
{
    vqueue_ub_node_t *pool_head = node_pool_get_and_destroy();
    if (pool_head) {
        locked_trace_add(&pool_trace, (vuintptr_t)pool_head);
    }
}
#endif
static inline void
queue_deregister(vsize_t tid, queue_t *q)
{
#if defined(VQUEUE_UB_LF_RECYCLE)
    _queue_save_local_pool();
#endif
    ismr_dereg(tid);
    V_UNUSED(q);
}

static inline void
queue_destroy(queue_t *q)
{
    void *data = NULL;
#if defined(VQUEUE_UB_LF_RECYCLE)
    _queue_save_local_pool();
    locked_trace_destroy(&pool_trace, _pool_destroy_cb);
#endif
    while (data = vqueue_ub_deq(q, _queue_destroy, NULL), data) {
        free(data);
    }
    vqueue_ub_destroy(q, _queue_destroy, NULL);

#if defined(VQUEUE_UB_LF) && !defined(VSYNC_VERIFICATION)
    ismr_stop_cleaner();
#endif

    ismr_destroy();
    ASSERT(vmem_no_leak());
}

static inline void
queue_enq(vsize_t tid, queue_t *q, vuint64_t key, char lbl)
{
    data_t *data = malloc(sizeof(data_t));
    if (data) {
        data->key           = key;
        data->lbl           = lbl;
        queue_node_t *qnode = NULL;

#if defined(VQUEUE_UB_LF_RECYCLE)
        qnode = node_pool_get();
        if (qnode == NULL) {
                // if the pool is empty allocate normally
    #ifdef VSYNC_VERIFICATION
            qnode = aligned_alloc_allocate();
    #else
            qnode = vmem_aligned_malloc(RECYCLE_QUEUE_NODE_ALIGNMENT,
                                        sizeof(queue_node_t));
    #endif
        }
#else
        qnode = vmem_malloc(sizeof(queue_node_t));
#endif
        ismr_enter(tid);
        vqueue_ub_enq(q, qnode, data);
        ismr_exit(tid);
    } else {
        ASSERT(0 && "allocation failed");
    }
}

static inline vsize_t
queue_get_length(vsize_t tid, queue_t *q)
{
    ismr_enter(tid);
    vsize_t len = vqueue_ub_get_length(q);
    ismr_exit(tid);
    return len;
}

static inline vbool_t
queue_empty(vsize_t tid, queue_t *q)
{
    ismr_enter(tid);
    vbool_t empty = vqueue_ub_empty(q);
    ismr_exit(tid);
    return empty;
}

static inline void *
queue_deq(vsize_t tid, queue_t *q)
{
    ismr_enter(tid);
    void *data = vqueue_ub_deq(q, _queue_retire, NULL);
    ismr_exit(tid);
    return data;
}

typedef void (*print_fun_t)(void *);
static inline void
_redirect_print(vqueue_ub_node_t *qnode, void *arg)
{
    print_fun_t print = (print_fun_t)arg;
    print(qnode->data);
}
static inline void
queue_print(vqueue_ub_t *q, print_fun_t print)
{
    _vqueue_ub_visit_nodes(q, _redirect_print, (void *)print);
}

static inline void
queue_visit_nodes(vqueue_ub_t *q,
                  void (*visitor)(vqueue_ub_node_t *, void *arg), void *arg)
{
    _vqueue_ub_visit_nodes(q, visitor, arg);
}

static inline void
queue_clean(vsize_t tid)
{
    ismr_recycle(tid);
}

#endif
