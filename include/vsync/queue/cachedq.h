/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_CACHEDQ_H
#define VSYNC_CACHEDQ_H
/*******************************************************************************
 * @file cachedq.h
 * @brief Lockless, multi-producer, multi-consumer queue
 * @ingroup linearizable
 *
 * A variation of the DPDK ring buffer that uses cached variables to improve the
 * performance. It accepts vuint64_t as data type.
 *
 * @example
 * @include eg_cachedq.c
 *
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/common/cache.h>
#include <vsync/common/assert.h>
#include <vsync/vtypes.h>

typedef struct cachedq_s {
    vatomic64_t phead VSYNC_CACHEALIGN;
    vatomic64_t ptail VSYNC_CACHEALIGN;
    vatomic64_t ptail_cached VSYNC_CACHEALIGN;
    vatomic64_t chead VSYNC_CACHEALIGN;
    vatomic64_t ctail VSYNC_CACHEALIGN;
    vatomic64_t ctail_cached VSYNC_CACHEALIGN;
    vuint64_t size VSYNC_CACHEALIGN;
    vuint64_t entry[] VSYNC_CACHEALIGN;
} cachedq_t;
/**
 * Returns the minimum allocation size for a`cachedq` with the given capacity.
 *
 * @param capacity maximum number of entries that can fit in the `cachedq`.
 *
 * @return minimum number bytes the `cachedq` buffer should have.
 */
static inline vsize_t
cachedq_memsize(vsize_t capacity)
{
    return sizeof(cachedq_t) + sizeof(vuint64_t) * capacity +
           VSYNC_CACHELINE_SIZE;
}
/**
 * Initializes the `cachedq` with a maximum capacity.
 *
 * @param buf a buffer that can accommodate at least `capacity` elements.
 * @param capacity maximum number of entries that can fit in the cachedq.
 *
 * @return `buf` pointer casted to cachedq_t*.
 *
 * Use `cachedq_memsize()` to determine the size of buf.
 */
static inline cachedq_t *
cachedq_init(void *buf, vsize_t capacity)
{
    cachedq_t *q = (cachedq_t *)buf;
    vatomic64_init(&q->phead, 0);
    vatomic64_init(&q->ptail, 0);
    vatomic64_init(&q->ptail_cached, 0);
    vatomic64_init(&q->chead, 0);
    vatomic64_init(&q->ctail, 0);
    vatomic64_init(&q->ctail_cached, 0);
    q->size = (capacity - VSYNC_CACHELINE_SIZE - sizeof(cachedq_t)) /
              sizeof(vuint64_t);
    return q;
}
/**
 * Enqueues one or more entries.
 *
 * Multiple entries can be enqueued if `buf` points to an array. Use `count` to
 * indicate how many entries should be enqueueed, starting from `buf`.
 *
 * @param q address of `cachedq_t` object.
 * @param buf   pointer to first entry.
 * @param count number of entries to enqueue.
 *
 * @return number of enqueued entries.
 */
static inline vsize_t
cachedq_enqueue(cachedq_t *q, vuint64_t *buf, vsize_t count)
{
again:;
    vuint64_t phead        = vatomic64_read_rlx(&q->phead);
    vuint64_t pnext        = phead + count;
    vuint64_t ctail_cached = vatomic64_read_rlx(&q->ctail_cached);

    if (pnext <= ctail_cached + q->size) {
        if (vatomic64_cmpxchg_rlx(&q->phead, phead, pnext) != phead) {
            goto again;
        }
        for (vsize_t i = 0; i < count; i++) {
            q->entry[(phead + i) % q->size] = buf[i];
        }
        await_while (vatomic64_read_rlx(&q->ptail) != phead) {}
        vatomic64_write_rel(&q->ptail, pnext);
        return count;
    } else {
        vuint64_t ctail = vatomic64_read_acq(&q->ctail);
        if (pnext <= ctail + q->size) {
            if (vatomic64_cmpxchg_rlx(&q->phead, phead, pnext) != phead) {
                goto again;
            }
            vatomic64_write_rlx(&q->ctail_cached, ctail);
            for (vsize_t i = 0; i < count; i++) {
                q->entry[(phead + i) % q->size] = buf[i];
            }
            await_while (vatomic64_read_rlx(&q->ptail) != phead) {}
            vatomic64_write_rel(&q->ptail, pnext);
            return count;
        } else {
            return 0;
        }
    }
}
/**
 * Dequeues one or more entries.
 *
 * Multiple entries can be dequeued if `buf` points to an array. Use `count` to
 * indicate how many entries should be dequeued.
 *
 * @param q address of `cachedq_t` object.
 * @param buf   pointer to preallocated memory for the first entry.
 * @param count number of entries to dequeue.
 * @return number of dequeued entries.
 */
static inline vsize_t
cachedq_dequeue(cachedq_t *q, vuint64_t *buf, vsize_t count)
{
again:;
    vuint64_t chead        = vatomic64_read_rlx(&q->chead);
    vuint64_t cnext        = chead + count;
    vuint64_t ptail_cached = vatomic64_read_rlx(&q->ptail_cached);

    if (cnext <= ptail_cached) {
        if (vatomic64_cmpxchg_rlx(&q->chead, chead, cnext) != chead) {
            goto again;
        }
        for (vsize_t i = 0; i < count; i++) {
            buf[i] = q->entry[(chead + i) % q->size];
        }
        await_while (vatomic64_read_rlx(&q->ctail) != chead) {}
        vatomic64_write_rel(&q->ctail, cnext);
        return count;
    } else {
        vuint64_t ptail = vatomic64_read_acq(&q->ptail);
        if (cnext <= ptail) {
            if (vatomic64_cmpxchg_rlx(&q->chead, chead, cnext) != chead) {
                goto again;
            }
            vatomic64_write_rlx(&q->ptail_cached, ptail);
            for (vsize_t i = 0; i < count; i++) {
                buf[i] = q->entry[(chead + i) % q->size];
            }
            await_while (vatomic64_read_rlx(&q->ctail) != chead) {}
            vatomic64_write_rel(&q->ctail, cnext);
            return count;
        } else if (chead < ptail) {
            cnext = ptail;
            ASSERT((cnext - chead) < VSIZE_MAX);
            count = (vsize_t)(cnext - chead);
            if (vatomic64_cmpxchg_rlx(&q->chead, chead, cnext) != chead) {
                goto again;
            }
            vatomic64_write_rlx(&q->ptail_cached, ptail);
            for (vsize_t i = 0; i < count; i++) {
                buf[i] = q->entry[(chead + i) % q->size];
            }
            await_while (vatomic64_read_rlx(&q->ctail) != chead) {}
            vatomic64_write_rel(&q->ctail, cnext);
            return count;
        } else {
            return 0;
        }
    }
}
/**
 * Returns the current number of entries in the cachedq.
 *
 * @param q address of `cachedq_t` object.
 * @return number of entries in the cachedq.
 */
static inline vsize_t
cachedq_count(cachedq_t *q)
{
    vuint64_t c = vatomic64_read_rlx(&q->ctail);
    vuint64_t p = vatomic64_read_rlx(&q->ptail);
    ASSERT((p - c) < VSIZE_MAX);
    return (vsize_t)(p - c);
}

#endif /* VSYNC_CACHEDQ_H */
