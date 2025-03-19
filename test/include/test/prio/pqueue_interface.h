/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VQUEUE_PRIO_INTERFACE_H
#define VQUEUE_PRIO_INTERFACE_H

#include <test/prio/queue_prio.h>
#include <test/rand.h>
#include <test/smr/ismr.h>

#if defined(VSYNC_VERIFICATION) || defined(SMR_NONE) ||                        \
    defined(VQUEUE_PRIO_HEAP_BASED)
// on verification we don't create another thread
// when no smr is used we don't create another thread
#else

    #define CLEANER_THREAD_ACTIVE
    #define MIN_RECLAIM_COUNT 1
    #define RECLAIM_FREQUENCY 1
#endif

#include <test/trace_manager.h>
#include <vsync/common/dbg.h>
#include <test/rand.h>
#include <test/vtid.h>
#include <test/vmem_stdlib.h>

/* mock user data object */
typedef struct data_s {
    smr_node_t smr_node;           /* embed smr node */
    vpriority_t priority;          /* save priority */
    vuintptr_t key;                /* some key to identify nodes with */
    vqueue_prio_node_t queue_node; /* must be last field contains VLA */
} data_t;

#if !defined(NTHREADS)
    #error "NTHREADS must be defined"
#endif

#define NTRACES (NTHREADS + 2U)

#ifndef DS_LEN
    #define DS_LEN 1
#endif

#define PRIORITY_MIN      8
#define PRIORITY_MAX      0
#define DEFAULT_TRACE_LEN 10

#ifdef VQUEUE_PRIO_SKIPLIST_BASED
vqueue_prio_sentinel_t *g_head;
vqueue_prio_sentinel_t *g_tail;
#endif

static vqueue_prio_t g_pqueue[DS_LEN];

static trace_t g_added[DS_LEN][NTRACES];
static trace_t g_removed[DS_LEN][NTRACES];
static trace_t g_joint_adds;
static trace_t g_joint_rems;
static trace_t g_final_state;

static inline void verify(vsize_t ds_idx);
static inline void _free_cb(smr_node_t *node, void *args);
static inline void _retire_cb(vqueue_prio_node_t *node, void *arg);

#if defined(VQUEUE_PRIO_HEAP_BASED)
static inline void _destroy_cb(void *node, void *arg);
static inline void visit_node(void *node, void *arg);
#else
static inline void _destroy_cb(vqueue_prio_node_t *node, void *arg);
static inline void visit_node(vqueue_prio_node_t *node, void *arg);
#endif

/****************************************************************
 *  Test Interface
 ****************************************************************/
static inline void
pqueue_init(void)
{
    vsize_t i = 0;
    vsize_t j = 0;

    ismr_init();

    for (i = 0; i < DS_LEN; i++) {
#if defined(VQUEUE_PRIO_HEAP_BASED)
        vqueue_prio_init(&g_pqueue[i], vtid_get_thread_id);
#elif defined(VQUEUE_PRIO_SKIPLIST_BASED)
        g_head = vmem_malloc(VQUEUE_PRIO_SENTINEL_SZ);
        g_tail = vmem_malloc(VQUEUE_PRIO_SENTINEL_SZ);
        vqueue_prio_init(&g_pqueue[i], _retire_cb, NULL,
                         (vuint32_t)rand_gen_seed(), g_head, g_tail);
#else
        vqueue_prio_init(&g_pqueue[i], _retire_cb, NULL,
                         random_thread_safe_get_next);
#endif
        for (j = 0; j < NTRACES; j++) {
            trace_init(&g_added[i][j], DEFAULT_TRACE_LEN);
            trace_init(&g_removed[i][j], DEFAULT_TRACE_LEN);
        }
    }

    trace_init(&g_joint_adds, DEFAULT_TRACE_LEN);
    trace_init(&g_joint_rems, DEFAULT_TRACE_LEN);
    trace_init(&g_final_state, DEFAULT_TRACE_LEN);

#if defined(CLEANER_THREAD_ACTIVE)
    ismr_start_cleaner();
#endif
}
static inline void
pqueue_destroy(void)
{
    vsize_t i = 0;
    vsize_t j = 0;

    ASSERT(DS_LEN == 1);

    for (i = 0; i < DS_LEN; i++) {
        DBG_RED("verifying %zu pqueue", i);
        verify(i);
        vqueue_prio_destroy(&g_pqueue[i], _destroy_cb, NULL);

        for (j = 0; j < NTRACES; j++) {
            trace_destroy(&g_added[i][j]);
            trace_destroy(&g_removed[i][j]);
        }
    }

    trace_destroy(&g_joint_adds);
    trace_destroy(&g_joint_rems);
    trace_destroy(&g_final_state);

#if defined(CLEANER_THREAD_ACTIVE)
    ismr_stop_cleaner();
#endif

    ismr_destroy();

#ifdef VQUEUE_PRIO_SKIPLIST_BASED
    vmem_free(g_head);
    vmem_free(g_tail);
#endif

#if defined(SMR_NONE) || defined(VQUEUE_PRIO_HEAP_BASED)
// do nothing
#else
    ASSERT(vmem_no_leak());
#endif
}

static inline vuintptr_t
pqueue_deq(vsize_t tid, vsize_t ds, vpriority_t *priority)
{
    vuintptr_t key           = 0;
    vqueue_prio_node_t *node = NULL;
    data_t *data             = NULL;

    ismr_enter(tid);
    node = vqueue_prio_remove_min(&g_pqueue[ds]);
    if (node) {
        data = V_CONTAINER_OF(node, data_t, queue_node);
        // reading values under the protection of smr!!
        key = data->key;
        if (priority) {
            *priority = data->priority;
        }
        trace_add(&g_removed[ds][tid], key);
#if defined(VQUEUE_PRIO_HEAP_BASED)
        vmem_free(data);
#endif
    } else {
        if (priority) {
            *priority = 0;
        }
    }
    ismr_exit(tid);
    return key;
}

static inline vbool_t
pqueue_enq(vsize_t tid, vsize_t ds, vuintptr_t key, vpriority_t priority)
{
    data_t *data    = NULL;
    vbool_t success = true;

#if defined(VQUEUE_PRIO_SKIPLIST_BASED)
    vsize_t height = 0;
    vsize_t sz =
        vqueue_prio_calc_node_sz(&g_pqueue[ds], sizeof(data_t), &height);
    ASSERT(height > 0);
    ASSERT(sz > sizeof(data_t));
    data = vmem_malloc(sz);
#else
    data = vmem_malloc(sizeof(data_t));
#endif
    data->key      = key;
    data->priority = priority;

    ismr_enter(tid);
#if defined(VQUEUE_PRIO_HEAP_BASED)
    success = vqueue_prio_add(&g_pqueue[ds], &data->queue_node, priority);
#elif defined(VQUEUE_PRIO_SKIPLIST_BASED)
    vqueue_prio_add(&g_pqueue[ds], &data->queue_node, priority, height);
#else
    vqueue_prio_add(&g_pqueue[ds], &data->queue_node, priority);
#endif

    if (success) {
        trace_add(&g_added[ds][tid], key);
    } else {
#if defined(VQUEUE_PRIO_HEAP_BASED)
        vmem_free(data);
#else
        ASSERT(0 && "unreachable is reached!!");
#endif
    }
    ismr_exit(tid);
    return success;
}

/****************************************************************
 *  SMR wrappers
 ****************************************************************/
static inline void
pqueue_reg(vsize_t tid)
{
    ismr_reg(tid);
}
static inline void
pqueue_dereg(vsize_t tid)
{
    ismr_dereg(tid);
}
static inline void
pqueue_clean(vsize_t tid)
{
    ismr_recycle(tid);
}
/****************************************************************
 *  Traces verification related
 ****************************************************************/
static inline vbool_t
verify_unit_does_not_exist(trace_unit_t *unit)
{
    vsize_t idx   = 0;
    vbool_t found = trace_find_unit_idx(&g_final_state, unit->key, &idx);
    return !found;
}

static inline vbool_t
verify_unit_exists(trace_unit_t *unit)
{
    vsize_t idx = 0;
    /* skipping units that have been canceled out */
    if (unit->count == 0)
        return true;
    vbool_t found = trace_find_unit_idx(&g_final_state, unit->key, &idx);
    return found && g_final_state.units[idx].count > 0;
}

static inline void
verify(vsize_t ds_idx)
{
    vsize_t i              = 0;
    vbool_t trace_is_sound = false;

    ASSERT(ds_idx < DS_LEN);

    // extract final state
    DBG_YELLOW("Trying to verify pqueue %p %zu \n", &g_pqueue[ds_idx], ds_idx);
    _vqueue_prio_visit(&g_pqueue[ds_idx], visit_node, &g_final_state);

    for (i = 0; i < NTRACES; i++) {
        trace_merge_into(&g_joint_adds, &g_added[ds_idx][i]);
        trace_merge_into(&g_joint_rems, &g_removed[ds_idx][i]);
    }

    trace_is_sound = trace_verify(&g_joint_rems, verify_unit_does_not_exist);
    ASSERT(trace_is_sound && "remove trace is not sound");

    trace_subtract_from(&g_joint_adds, &g_joint_rems);

    trace_is_sound = trace_verify(&g_joint_adds, verify_unit_exists);
    if (!trace_is_sound) {
        trace_print(&g_joint_rems, "removed items");
        trace_print(&g_joint_adds, "remaining add");
        trace_print(&g_final_state, "final state ");
    }
    ASSERT(trace_is_sound && "remaining add trace is not sound");

    vbool_t equal = trace_is_subtrace(&g_joint_adds, &g_final_state, NULL);
    ASSERT(equal && "final state is not part of the added nodes");

    trace_reset(&g_joint_adds);
    trace_reset(&g_joint_rems);
    trace_reset(&g_final_state);
}
/****************************************************************
 *  Callbacks
 ****************************************************************/
static inline void
_retire_cb(vqueue_prio_node_t *node, void *arg)
{
    data_t *data = V_CONTAINER_OF(node, data_t, queue_node);
    ismr_retire(&data->smr_node, _free_cb, false);
    V_UNUSED(arg);
}

static inline void
_free_cb(smr_node_t *node, void *args)
{
    data_t *data = V_CONTAINER_OF(node, data_t, smr_node);
#if defined(SMR_NONE) && !defined(VQUEUE_PRIO_HEAP_BASED)
    /* if no smr is used, then don't free nodes */
    V_UNUSED(data);
#else
    vmem_free(data);
#endif
    V_UNUSED(args);
}

#if defined(VQUEUE_PRIO_HEAP_BASED)
static inline void
_destroy_cb(void *node, void *arg)
{
    data_t *data = V_CONTAINER_OF(node, data_t, queue_node);
    vmem_free(data);
    V_UNUSED(arg);
}
static inline void
visit_node(void *node, void *arg)
{
    trace_t *trace = (trace_t *)arg;
    data_t *data   = V_CONTAINER_OF(node, data_t, queue_node);
    trace_add(trace, data->key);
}

#else
static inline void
_destroy_cb(vqueue_prio_node_t *node, void *arg)
{
    data_t *data = V_CONTAINER_OF(node, data_t, queue_node);
    vmem_free(data);
    V_UNUSED(arg);
}
static inline void
visit_node(vqueue_prio_node_t *node, void *arg)
{
    trace_t *trace = (trace_t *)arg;
    data_t *data   = V_CONTAINER_OF(node, data_t, queue_node);
    trace_add(trace, data->key);
}

#endif
#endif
