/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#if defined(STACK_ELIMINATION_BACKOFF)
    #include <vsync/stack/elimination_stack.h>
    #define VSTACK_NAME "elimination"
#elif defined(STACK_XBO_BACKOFF)
    #include <vsync/stack/xbo_stack.h>
    #define VSTACK_NAME "backoff"
    #include <unistd.h>
    #ifndef VSTACK_MIN_BACKOFF_MS
        #define VSTACK_MIN_BACKOFF_MS 1
    #endif

    #ifndef VSTACK_MAX_BACKOFF_MS
        #define VSTACK_MAX_BACKOFF_MS 1000
    #endif
static inline int
_usleep_callback(unsigned int milliseconds)
{
    #if defined(VSYNC_VERIFICATION)
    V_UNUSED(milliseconds);
    #else
    usleep(milliseconds);
    #endif
    return 0;
}
#elif defined(STACK_BASELINE)
    #include <test/stack/stack_baseline.h>
    #define VSTACK_NAME "baseline"
/* does not need SMR */
    #define SMR_NONE
#else
    #include <test/stack/stack_helper_dispatcher.h>
#endif

#include <test/trace_manager.h>
#include <vsync/common/dbg.h>
#include <test/vmem_stdlib.h>
#include <test/smr/ismr.h>
#include <time.h>
#include <test/rand.h>
#include <vsync/common/compiler.h>

#ifndef NTHREADS
    #error "undefined number of threads"
#endif

#define NTRACES (SMR_MAX_NTHREADS + 1)

#ifndef N_DS
    #define N_DS 1
#endif


#define DEFAULT_TRACE_LEN 10

static vstack_t g_stack[N_DS];
static trace_t g_added[N_DS][NTRACES];
static trace_t g_removed[N_DS][NTRACES];

trace_t g_joint_adds;
trace_t g_joint_rems;
trace_t g_final_state;

typedef struct data_node_s {
    vstack_node_t stack_node; /* keep as first field DO NOT move it */
    smr_node_t smr_node;
    vuint64_t key;
    char lbl;
} data_node_t;


static inline void
_free_callback(smr_node_t *snode, void *args)
{
    data_node_t *data = V_CONTAINER_OF(snode, data_node_t, smr_node);
    vmem_free(data);
    V_UNUSED(args);
}

static inline void
push(vsize_t tid, vsize_t ds, vuintptr_t id)
{
    data_node_t *in_data = NULL;
    in_data              = vmem_malloc(sizeof(data_node_t));
    in_data->key         = id;
    DBG_YELLOW("[T%zu] pushing %lx", tid, id);
    vstack_push(&g_stack[ds], &in_data->stack_node);
    trace_add(&g_added[ds][tid], id);
}

static inline vuintptr_t
pop(vsize_t tid, vsize_t ds)
{
#ifdef STACK_HELPER_INTERFACE
    vbool_t kernel_thread = tid < (1 << VSTACK_CORE_ID_WIDTH);
    vuint32_t core_id = kernel_thread ? tid : tid - (1 << VSTACK_CORE_ID_WIDTH);

    vstack_node_t *node = vstack_pop(&g_stack[ds], core_id, kernel_thread);
#else
    vstack_node_t *node = vstack_pop(&g_stack[ds]);
#endif
    if (node) {
        data_node_t *out_data = V_CONTAINER_OF(node, data_node_t, stack_node);
        vuint64_t key         = out_data->key;
        ismr_retire(&out_data->smr_node, _free_callback, false);
        trace_add(&g_removed[ds][tid], key);
        DBG_GREEN("[T%zu] popped %lx", key);
        return key;
    }
    /* zero is mapped to no item */
    DBG_GREEN("[T%zu] popped EMPTY ", tid);
    return 0;
}


static inline void
init(void)
{
    vsize_t i = 0;
    vsize_t j = 0;

    ismr_init();

    for (i = 0; i < N_DS; i++) {
#if defined(STACK_XBO_BACKOFF)
        vstack_init(&g_stack[i], _usleep_callback, random_thread_safe_get_next,
                    VSTACK_MIN_BACKOFF_MS, VSTACK_MAX_BACKOFF_MS);
#elif defined(STACK_ELIMINATION_BACKOFF)
        vstack_init(&g_stack[i], random_thread_safe_get_next);
#else
        vstack_init(&g_stack[i]);
#endif

        for (j = 0; j < NTRACES; j++) {
            trace_init(&g_added[i][j], DEFAULT_TRACE_LEN);
            trace_init(&g_removed[i][j], DEFAULT_TRACE_LEN);
        }
    }

    trace_init(&g_joint_adds, DEFAULT_TRACE_LEN);
    trace_init(&g_joint_rems, DEFAULT_TRACE_LEN);
    trace_init(&g_final_state, DEFAULT_TRACE_LEN);


#if !defined(VSYNC_VERIFICATION)
    ismr_start_cleaner();
#endif
}

static inline void
destroy_data(vstack_node_t *node, void *arg)
{
    data_node_t *data = V_CONTAINER_OF(node, data_node_t, stack_node);
    vmem_free(data);
    V_UNUSED(arg);
}

static inline void
destroy(void)
{
    vsize_t i = 0;
    vsize_t j = 0;

    for (i = 0; i < N_DS; i++) {
        vstack_destroy(&g_stack[i], destroy_data, NULL);

        for (j = 0; j < NTRACES; j++) {
            trace_destroy(&g_added[i][j]);
            trace_destroy(&g_removed[i][j]);
        }
    }

    trace_destroy(&g_joint_adds);
    trace_destroy(&g_joint_rems);
    trace_destroy(&g_final_state);

#if !defined(VSYNC_VERIFICATION)
    ismr_stop_cleaner();
#endif

    ismr_destroy();

#if !defined(SMR_NONE)
    ASSERT(vmem_no_leak() && "#of allocs != #of frees");
#endif
}

static inline void
visit_node(vstack_node_t *snode, void *arg)
{
    trace_t *trace = arg;

    ASSERT(snode);
    data_node_t *data = V_CONTAINER_OF(snode, data_node_t, stack_node);

    ASSERT(arg);
    trace_add(trace, data->key);
}


static inline vbool_t
verify_unit(trace_unit_t *unit, vbool_t expect_existence)
{
    vsize_t idx = 0;
    if (unit->count) {
        if (unit->count > 1) {
            DBG_RED("%lu was added %zu times ", unit->key, unit->count);
        }
        ASSERT(unit->count == 1);
        vbool_t exists = trace_find_unit_idx(&g_final_state, unit->key, &idx);
        return exists == expect_existence;
    }
    // ignore removed units
    return true;
}

static inline vbool_t
verify_unit_does_not_exist(trace_unit_t *unit)
{
    return verify_unit(unit, false);
}

static inline vbool_t
verify_unit_exists(trace_unit_t *unit)
{
    return verify_unit(unit, true);
}


static inline void
verify(vsize_t ds_idx)
{
    vsize_t i              = 0;
    vbool_t trace_is_sound = false;

    ASSERT(ds_idx < N_DS);

    // extract final state
    _vstack_visit(&g_stack[ds_idx], visit_node, &g_final_state);


    for (i = 0; i < NTRACES; i++) {
        trace_merge_into(&g_joint_adds, &g_added[ds_idx][i]);
        trace_merge_into(&g_joint_rems, &g_removed[ds_idx][i]);
    }

    trace_is_sound = trace_verify(&g_joint_rems, verify_unit_does_not_exist);
    ASSERT(trace_is_sound && "remove trace is not sound");


    trace_subtract_from(&g_joint_adds, &g_joint_rems);

    trace_is_sound = trace_verify(&g_joint_adds, verify_unit_exists);
    if (!trace_is_sound) {
        trace_print(&g_joint_adds, "remaining add ");
        trace_print(&g_final_state, "final state ");
    }
    ASSERT(trace_is_sound && "remaining add trace is not sound");


    vbool_t equal = trace_is_subtrace(&g_joint_adds, &g_final_state, NULL);
    ASSERT(equal && "final state is not part of the added nodes");

    trace_reset(&g_joint_adds);
    trace_reset(&g_joint_rems);
    trace_reset(&g_final_state);
}

void
reg(vsize_t tid)
{
    ismr_reg(tid);
}
void
dereg(vsize_t tid)
{
    ismr_dereg(tid);
}
void
stack_enter(vsize_t tid)
{
    ismr_enter(tid);
}
void
stack_exit(vsize_t tid)
{
    ismr_exit(tid);
}


static inline void
stack_clean(vsize_t tid)
{
    ismr_recycle(tid);
}
