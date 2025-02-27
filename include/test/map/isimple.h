/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef ISIMPLE_H
#define ISIMPLE_H

/* Macros */
#define MAIN_TID NTHREADS
#define NTRACES  (NTHREADS + 1U)

#if defined(VSYNC_VERIFICATION)
    #define TRACE_CAPACITY 8U
#else
    #define TRACE_CAPACITY 1024U
#endif

#define VSIMPLE_HT_CAPACITY (TRACE_CAPACITY / 2U)

/* includes */
#include <test/trace_manager.h>
#include <vsync/map/simpleht.h>
#include <vsync/vtypes.h>

/* types */
typedef struct data_s {
    vuintptr_t key;
    vuint64_t val;
} data_t;

/* Globals */
static trace_t g_add[NTRACES] = {0};
static trace_t g_rem[NTRACES] = {0};
static vsimpleht_t g_simpleht = {0};
static void *g_buff           = NULL;


/* callbacks */
static inline vint8_t
cb_cmp(vuintptr_t key_a, vuintptr_t key_b)
{
    if (key_a == key_b) {
        return 0;
    } else if (key_a < key_b) {
        return -1;
    } else {
        return 1;
    }
}

#if defined(VSYNC_VERIFICATION)
static inline vuint64_t
cb_hash(vuintptr_t key)
{
    return (vuint64_t)key;
}
#else
static inline vuint64_t
cb_hash(vuintptr_t key)
{
    vuint64_t h = key;
    h ^= h >> 16U;
    h *= 0x85ebca6bU;
    h ^= h >> 13U;
    h *= 0xc2b2ae35U;
    h ^= h >> 16U;
    return h;
}
#endif
static inline void
cb_destroy(void *data)
{
    free(data);
}

static inline void
_imap_verify(void)
{
    vuintptr_t key = 0;
    data_t *data   = NULL;
    vsimpleht_iter_t iter;

    trace_t add_trc;
    trace_t rem_trc;
    trace_t final_state_trc;

    trace_init(&add_trc, TRACE_CAPACITY);
    trace_init(&rem_trc, TRACE_CAPACITY);

    /* merge local traces */
    for (vsize_t i = 0; i < NTRACES; i++) {
        trace_merge_into(&add_trc, &g_add[i]);
        trace_merge_into(&rem_trc, &g_rem[i]);
    }

    /* extract final state */
    trace_init(&final_state_trc, TRACE_CAPACITY);
    vsimpleht_iter_init(&g_simpleht, &iter);
    while (vsimpleht_iter_next(&iter, &key, (void **)&data)) {
        trace_add(&final_state_trc, key);
    }

    trace_subtract_from(&add_trc, &rem_trc);
    vbool_t eq = trace_is_subtrace(&add_trc, &final_state_trc, NULL);

    trace_destroy(&add_trc);
    trace_destroy(&rem_trc);
    trace_destroy(&final_state_trc);
    ASSERT(eq && "the final state is not what is expected");
}

/* interface functions */
static inline void
imap_init(void)
{
    vsize_t sz   = vsimpleht_buff_size(VSIMPLE_HT_CAPACITY);
    void *g_buff = malloc(sz);

    vsimpleht_init(&g_simpleht, g_buff, VSIMPLE_HT_CAPACITY, cb_cmp, cb_hash,
                   cb_destroy);

    for (vsize_t i = 0; i < NTRACES; i++) {
        trace_init(&g_add[i], TRACE_CAPACITY);
        trace_init(&g_rem[i], TRACE_CAPACITY);
    }
}
static inline void
imap_destroy(void)
{
    _imap_verify();
    for (vsize_t i = 0; i < NTRACES; i++) {
        trace_destroy(&g_add[i]);
        trace_destroy(&g_rem[i]);
    }
    vsimpleht_destroy(&g_simpleht);
    free(g_buff);
}
static inline vbool_t
imap_add(vsize_t tid, vuintptr_t key, vuint64_t val)
{
    data_t *data = malloc(sizeof(data_t));
    data->key    = key;
    data->val    = val;
    vbool_t added =
        vsimpleht_add(&g_simpleht, data->key, data) == VSIMPLEHT_RET_OK;
    if (added) {
        trace_add(&g_add[tid], data->key);
    } else {
        free(data);
    }
    return added;
}

static inline vbool_t
imap_rem(vsize_t tid, vuintptr_t key)
{
    vbool_t removed = vsimpleht_remove(&g_simpleht, key) == VSIMPLEHT_RET_OK;
    if (removed) {
        trace_add(&g_rem[tid], key);
    }
    return removed;
}

static inline void *
imap_get(vsize_t tid, vuintptr_t key)
{
    V_UNUSED(tid);
    data_t *data = vsimpleht_get(&g_simpleht, key);
    if (data) {
        ASSERT(data->key == key);
    }
    return data;
}

static inline void
imap_reg(vsize_t tid)
{
    V_UNUSED(tid);
    vsimpleht_thread_register(&g_simpleht);
}

static inline void
imap_dereg(vsize_t tid)
{
    V_UNUSED(tid);
    vsimpleht_thread_deregister(&g_simpleht);
}

static inline void
imap_print(void)
{
    vuintptr_t key = 0;
    data_t *data   = NULL;
    vsimpleht_iter_t iter;
    vsimpleht_iter_init(&g_simpleht, &iter);
    while (vsimpleht_iter_next(&iter, &key, (void **)&data)) {
        printf("[%" VUINTPTR_FORMAT ":%" VUINT64_FORMAT "],", key, data->val);
    }
    printf("\n");
}

#endif
