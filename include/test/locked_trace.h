/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTRACE_LOCKED_H
#define VTRACE_LOCKED_H

#include <pthread.h>
#include <test/trace_manager.h>
#include <vsync/common/assert.h>

typedef struct locked_trace_s {
    trace_t trace;
    pthread_mutex_t lock;
} locked_trace_t;

static inline void
locked_trace_init(locked_trace_t *trace, vsize_t capacity)
{
    ASSERT(trace);
    pthread_mutex_init(&trace->lock, NULL);
    trace_init(&trace->trace, capacity);
}

static inline void
locked_trace_add(locked_trace_t *trace, vuintptr_t obj)
{
    ASSERT(trace);
    pthread_mutex_lock(&trace->lock);
    trace_add(&trace->trace, obj);
    pthread_mutex_unlock(&trace->lock);
}

static inline void
locked_trace_destroy(locked_trace_t *trace, trace_verify_unit callback)
{
    trace_verify(&trace->trace, callback);
    trace_destroy(&trace->trace);
    pthread_mutex_destroy(&trace->lock);
}

#endif
