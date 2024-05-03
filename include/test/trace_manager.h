/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TRACE_MANAGER_H
#define VSYNC_TRACE_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <vsync/common/assert.h>
#include <vsync/vtypes.h>
#include <vsync/common/dbg.h>
#include <vsync/utils/string.h>

#define VTRACE_EXTEND_FACTOR 2

typedef struct trace_unit_s {
    vuintptr_t key;
    vsize_t count;
} trace_unit_t;

typedef struct trace_s {
    trace_unit_t *units;
    vsize_t len;
    vsize_t capacity;
    vbool_t initialized;
} trace_t;

typedef vbool_t (*trace_verify_unit)(trace_unit_t *unit);

static inline void
trace_init(trace_t *trace, vsize_t capacity)
{
    ASSERT(trace);
    trace->units = malloc(capacity * sizeof(trace_unit_t));
    if (trace->units) {
        trace->len         = 0;
        trace->capacity    = capacity;
        trace->initialized = true;
    } else {
        trace->len         = 0;
        trace->capacity    = 0;
        trace->initialized = false;
        ASSERT(0 && "allocation failed");
    }
}

/**
 * Allows to reuse trace memory without destroying it
 *
 * @param trace
 */
static inline void
trace_reset(trace_t *trace)
{
    ASSERT(trace);
    ASSERT(trace->units);
    ASSERT(trace->initialized);
    trace->len = 0;
}

static inline vsize_t
trace_get_length(trace_t *trace)
{
    ASSERT(trace);
    ASSERT(trace->initialized);
    return trace->len;
}

static inline void
trace_extend(trace_t *trace)
{
    ASSERT(trace);

    vsize_t src_size = trace->capacity * sizeof(trace_unit_t);
    vsize_t des_max  = src_size * VTRACE_EXTEND_FACTOR;

    trace_unit_t *src = trace->units;
    trace_unit_t *des = malloc(des_max);

    if (des) {
        int ret = memcpy_s(des, des_max, src, src_size);
        if (ret == 0) {
            trace->units = des;
            trace->capacity *= VTRACE_EXTEND_FACTOR;
        } else {
            ASSERT(0 && "copying failed");
        }
        free(src);
    } else {
        ASSERT(0 && "allocation failed");
    }
}

static inline void
trace_destroy(trace_t *trace)
{
    ASSERT(trace);
    ASSERT(trace->initialized);
    free(trace->units);
    trace->initialized = false;
}

static inline vbool_t
trace_find_unit_idx(trace_t *trace, vuintptr_t key, vsize_t *out_idx)
{
    vsize_t i = 0;
    ASSERT(trace);
    ASSERT(trace->initialized);
    for (i = 0; i < trace->len; i++) {
        if (trace->units[i].key == key) {
            *out_idx = i;
            return true;
        }
    }
    return false;
}

static inline void
_trace_add_or_rem_occurrences(trace_t *trace, vuintptr_t key, vsize_t count,
                              vbool_t subtract)
{
    vsize_t idx   = 0;
    vbool_t found = false;

    ASSERT(trace);
    ASSERT(trace->initialized);

    found = trace_find_unit_idx(trace, key, &idx);

    if (subtract) {
        ASSERT(found);
        ASSERT(trace->units[idx].count >= count);
        trace->units[idx].count -= count;
        return;
    }

    if (!found) {
        idx = trace->len++;
        if (idx >= trace->capacity) {
            trace_extend(trace);
        }
        trace->units[idx].key   = key;
        trace->units[idx].count = count;
    } else {
        trace->units[idx].count += count;
    }
}

static inline void
trace_add(trace_t *trace, vuintptr_t key)
{
    ASSERT(trace);
    ASSERT(trace->initialized);
    _trace_add_or_rem_occurrences(trace, key, 1, false);
}

static inline void
_trace_merge_or_subtract(trace_t *trace_container, trace_t *trace,
                         vbool_t subtract)
{
    vsize_t i = 0;
    ASSERT(trace_container);
    ASSERT(trace_container->initialized);

    ASSERT(trace);
    ASSERT(trace->initialized);

    for (i = 0; i < trace->len; i++) {
        _trace_add_or_rem_occurrences(trace_container, trace->units[i].key,
                                      trace->units[i].count, subtract);
    }
}
static inline void
trace_merge_into(trace_t *trace_container, trace_t *trace)
{
    _trace_merge_or_subtract(trace_container, trace, false);
}

static inline void
trace_subtract_from(trace_t *trace_container, trace_t *trace)
{
    _trace_merge_or_subtract(trace_container, trace, true);
}

static inline void
trace_print(trace_t *trace, char *trace_name)
{
    vsize_t i             = 0;
    const vsize_t newline = 5;

    ASSERT(trace);
    ASSERT(trace->initialized);

    printf("NAME:%s, LEN:%zu : { ", trace_name, trace->len);

    for (i = 0; i < trace->len; i++) {
        printf("[Key: %" VUINTPTR_FORMAT ", count: %zu], ", trace->units[i].key,
               trace->units[i].count);
        if (i % newline == 0) {
            printf("\n");
        }
    }

    printf(" }\n");
}

static inline vbool_t
trace_verify(trace_t *trace, trace_verify_unit verify_fun)
{
    vsize_t i = 0;

    ASSERT(trace);
    ASSERT(trace->initialized);
    ASSERT(verify_fun);

    for (i = 0; i < trace->len; i++) {
        if (verify_fun(&trace->units[i]) == false) {
            return false;
        }
    }
    return true;
}

/**
 * Compares if the given trace_a is equal to the given trace_b.
 * Two traces are considered equal if they have the same length, and
 * for each key k in trace_a, there exists a key k' such that
 * k == k' and the #occurrences of k = #occurrences of k'.
 *
 * @note if print parameter is provided, the key on which the traces differ is
 * printed
 *
 * @param trace_a address of trace_t
 * @param trace_b address of trace_t
 * @param print function pointer, for printing the first key where traces
 * differ, can be NULL
 * @return true the given traces are equal
 * @return false the given traces differ
 */
static inline vbool_t
trace_are_eq(trace_t *trace_a, trace_t *trace_b, void (*print)(vuintptr_t k))
{
    vsize_t i   = 0;
    vsize_t idx = 0;

    trace_unit_t *unit_a;
    trace_unit_t *unit_b;

    if (trace_a->len != trace_b->len) {
        if (print) {
            printf("different trace length %zu %zu\n", trace_a->len,
                   trace_b->len);
        }
        return false;
    }

    /* verify that for each key in trace_a there exists a key in trace_b */
    for (i = 0; i < trace_a->len; i++) {
        unit_a = &trace_a->units[i];

        if (trace_find_unit_idx(trace_b, unit_a->key, &idx)) {
            unit_b = &trace_b->units[idx];

            ASSERT(unit_a->key == unit_b->key);

            if (unit_a->count != unit_b->count) {
                if (print) {
                    printf("key[%" VUINTPTR_FORMAT
                           "] count is different %zu != %zu\n",
                           unit_a->key, unit_a->count, unit_b->count);
                    print(unit_a->key);
                }
                return false;
            }
        } else {
            if (print) {
                printf("key[%" VUINTPTR_FORMAT "] not found\n", unit_a->key);
                print(unit_a->key);
            }
            return false;
        }
    }

    return true;
}

// to do make it part of the above function
static inline vbool_t
trace_is_subtrace(trace_t *super_trace, trace_t *sub_trace,
                  void (*print)(vuintptr_t k))
{
    vsize_t i   = 0;
    vsize_t idx = 0;

    trace_unit_t *unit_a;
    trace_unit_t *unit_b;

    /* verify that for each key in trace_a there exists a key in trace_b */
    for (i = 0; i < sub_trace->len; i++) {
        unit_a = &sub_trace->units[i];

        if (trace_find_unit_idx(super_trace, unit_a->key, &idx)) {
            unit_b = &super_trace->units[idx];

            ASSERT(unit_a->key == unit_b->key);

            if (unit_a->count != unit_b->count) {
                if (print) {
                    printf("key[%" VUINTPTR_FORMAT
                           "] count is different %zu != %zu\n",
                           unit_a->key, unit_a->count, unit_b->count);
                    print(unit_a->key);
                }
                return false;
            }
        } else {
            if (print) {
                printf("key[%" VUINTPTR_FORMAT "] not found\n", unit_a->key);
                print(unit_a->key);
            }
            return false;
        }
    }

    return true;
}
#endif
