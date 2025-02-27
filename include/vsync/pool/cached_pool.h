/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_CACHEDPOOL_H
#define VSYNC_CACHEDPOOL_H

/*******************************************************************************
 * @file cached_pool.h
 * @brief Multi-threaded, fixed size, and fixed capacity memory pool
 *
 * ### Performance:
 *  - No atomic operations in the fast path of alloc/free
 *  - Increasing number of entries may have a better performance
 *
 * @example
 * @include eg_cached_pool.c
 *
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/common/cache.h>
#include <vsync/common/compiler.h>
#include <vsync/utils/math.h>

typedef struct cached_pool_config_s {
    vuint32_t entry_space;
    vuint32_t buffer_mask;
    vuint32_t threshold_init;
    vuint32_t threshold_link;
    vuint32_t threshold_max;
} VSYNC_CACHEALIGN cached_pool_config_t;

typedef vuintptr_t cached_pool_entry_t;

typedef struct cached_pool_buffer_s {
    vatomic64_t phead VSYNC_CACHEALIGN;
    vatomic64_t ptail VSYNC_CACHEALIGN;
    vatomic64_t chead VSYNC_CACHEALIGN;
    vatomic64_t ctail VSYNC_CACHEALIGN;
    cached_pool_entry_t *nodes[] VSYNC_CACHEALIGN;
} VSYNC_CACHEALIGN cached_pool_buffer_t;

typedef struct cached_pool_vunit_s {
    vuint32_t cnt;
    cached_pool_buffer_t *buf;
    cached_pool_entry_t *top;
    cached_pool_entry_t *mid;
} VSYNC_CACHEALIGN cached_pool_vunit_t;

typedef struct cached_pool_s {
    cached_pool_config_t conf VSYNC_CACHEALIGN;
    cached_pool_buffer_t *buf VSYNC_CACHEALIGN;
    cached_pool_entry_t *entries VSYNC_CACHEALIGN;
    cached_pool_vunit_t vunits[] VSYNC_CACHEALIGN;
} VSYNC_CACHEALIGN cached_pool_t;

#define CACHED_POOL_MAX_THRESHOLD_FACTOR 2U

/******************************** entry ***************************************/
static inline cached_pool_entry_t *
_cached_pool_entry_get_next(cached_pool_entry_t *e)
{
    return (cached_pool_entry_t *)(*e);
}

static inline void
_cached_pool_entry_set_next(cached_pool_entry_t *entry,
                            cached_pool_entry_t *next)
{
    *entry = (cached_pool_entry_t)next;
}

static inline cached_pool_entry_t *
_cached_pool_entry_find(void *start, vuint32_t size, vuint32_t idx)
{
    return (cached_pool_entry_t *)(((vuintptr_t)start) + size * idx);
}

static inline void *
_cached_pool_entry_to_addr(cached_pool_entry_t *entry)
{
    return (cached_pool_entry_t *)(((vuintptr_t)entry) + sizeof(void *));
}

static inline cached_pool_entry_t *
_cached_pool_entry_from_addr(void *p)
{
    return (cached_pool_entry_t *)(((vuintptr_t)p) - sizeof(void *));
}

/******************************* buffer ***************************************/
static void
_cached_pool_buffer_init(cached_pool_buffer_t *buf)
{
    vatomic64_init(&buf->phead, 0);
    vatomic64_init(&buf->ptail, 0);
    vatomic64_init(&buf->chead, 0);
    vatomic64_init(&buf->ctail, 0);
}

static inline cached_pool_entry_t *
_cached_pool_buffer_alloc(cached_pool_t *a, cached_pool_buffer_t *buf)
{
again:;
    vuint64_t ch = vatomic64_read_rlx(&buf->chead);
    vuint64_t pt = vatomic64_read_acq(&buf->ptail);
    if (ch == pt) {
        return NULL;
    }
    if (vatomic64_cmpxchg_rlx(&buf->chead, ch, ch + 1) != ch) {
        goto again;
    }
    cached_pool_entry_t *es = buf->nodes[ch & a->conf.buffer_mask];
    await_while (vatomic64_read_rlx(&buf->ctail) != ch) {}
    vatomic64_write_rel(&buf->ctail, ch + 1);
    return es;
}

static inline void
_cached_pool_buffer_free(cached_pool_t *a, cached_pool_buffer_t *buf,
                         cached_pool_entry_t *es)
{
again:;
    vuint64_t ph = vatomic64_read_rlx(&buf->phead);
    if (vatomic64_cmpxchg_rlx(&buf->phead, ph, ph + 1) != ph) {
        goto again;
    }
    buf->nodes[ph & a->conf.buffer_mask] = es;
    await_while (vatomic64_read_rlx(&buf->ptail) != ph) {}
    vatomic64_write_rel(&buf->ptail, ph + 1);
}

/********************************* vunit **************************************/
static void
_cached_pool_vunit_init(cached_pool_t *a, cached_pool_vunit_t *u,
                        cached_pool_buffer_t *b, cached_pool_entry_t *es)
{
    u->cnt = a->conf.threshold_init;
    u->buf = b;
    u->top = es;
    u->mid = es;
}

static inline void *
_cached_pool_vunit_alloc(cached_pool_t *a, cached_pool_vunit_t *u)
{
    cached_pool_entry_t *es = u->top;
    if (unlikely(!es)) {
        es = _cached_pool_buffer_alloc(a, u->buf);
        if (unlikely(!es)) {
            return NULL;
        }
        u->cnt = a->conf.threshold_init;
    }
    u->top = _cached_pool_entry_get_next(es);
    u->cnt--;
    return _cached_pool_entry_to_addr(es);
}

static inline void
_cached_pool_vunit_free(cached_pool_t *a, cached_pool_vunit_t *u, void *p)
{
    cached_pool_entry_t *e = _cached_pool_entry_from_addr(p);
    _cached_pool_entry_set_next(e, u->top);
    u->top = e;
    u->cnt++;
    if (unlikely(u->cnt == a->conf.threshold_link)) {
        u->mid = u->top;
    }
    if (unlikely(u->cnt == a->conf.threshold_max)) {
        u->cnt                  = a->conf.threshold_init;
        cached_pool_entry_t *es = _cached_pool_entry_get_next(u->mid);
        _cached_pool_entry_set_next(u->mid, NULL);
        _cached_pool_buffer_free(a, u->buf, es);
    }
}

static inline cached_pool_vunit_t *
_cached_pool_vunit_find(cached_pool_t *a, vuint32_t id)
{
    return (cached_pool_vunit_t *)&a->vunits[id];
}

/******************************** pool ****************************************/
/**
 * Calculate the needed memory space for creating a pool
 *
 * @param thread_num    maxinum thread number
 * @param entry_num     minimal number of entires
 * @param entry_size    size of each entry
 *
 * @return needed memory space (in bytes)
 */
#define cached_pool_memsize(thread_num, entry_num, entry_size)                 \
    (sizeof(cached_pool_t) + sizeof(cached_pool_vunit_t) * (thread_num) +      \
     sizeof(cached_pool_buffer_t) + sizeof(void *) * (thread_num)*2 +          \
     (thread_num) * ((((entry_num)-1U) / (thread_num) + 1U) << 1U) *           \
         ((entry_size) + sizeof(void *)))

/**
 * Initialize the pool
 *
 * Make sure the buffer has enough size (calculated by cached_pool_memsize)
 *
 * @param buf           pointer to the buffer
 * @param thread_num    maxinum thread number
 * @param entry_num     minimal number of entires
 * @param entry_size    size of each entry
 *
 * @return a pointer of the pool structure
 */
static cached_pool_t *
cached_pool_init(void *buf, vuint32_t thread_num, vuint32_t entry_num,
                 vuint32_t entry_size)
{
    vuint32_t cached_pool_head = sizeof(cached_pool_t);
    vuint32_t buffer_head      = sizeof(cached_pool_buffer_t);
    vuint32_t buffer_size =
        buffer_head + (vuint32_t)(sizeof(void *) * thread_num * 2);
    vuint32_t node_num          = (v_pow2_round_up(thread_num * 2));
    vuint32_t threshold         = ((entry_num - 1) / thread_num + 1);
    vuint32_t vunit_size        = sizeof(cached_pool_vunit_t);
    vuint32_t vunits_size       = vunit_size * thread_num;
    vuint32_t vunit_addr_offset = cached_pool_head;
    vuint32_t buf_addr_offset   = vunit_addr_offset + vunits_size;
    vuint32_t entry_addr_offset = buf_addr_offset + buffer_size;

    cached_pool_t *a = (cached_pool_t *)buf;
    a->buf     = (cached_pool_buffer_t *)(((vuintptr_t)a) + buf_addr_offset);
    a->entries = (cached_pool_entry_t *)(((vuintptr_t)a) + entry_addr_offset);
    a->conf.entry_space    = entry_size + sizeof(void *);
    a->conf.buffer_mask    = node_num - 1;
    a->conf.threshold_init = threshold;
    a->conf.threshold_link = threshold + 1;
    a->conf.threshold_max  = threshold * CACHED_POOL_MAX_THRESHOLD_FACTOR;

    _cached_pool_buffer_init(a->buf);

    for (vuint32_t id = 0; id < thread_num; id++) {
        vuint32_t start  = id * a->conf.threshold_max;
        vuint32_t middle = start + a->conf.threshold_init;
        vuint32_t end    = middle + a->conf.threshold_init;

        _cached_pool_vunit_init(
            a, &a->vunits[id], a->buf,
            _cached_pool_entry_find(a->entries, a->conf.entry_space, start));
        for (vuint32_t idx = start; idx < middle - 1; idx++) {
            cached_pool_entry_t *curr =
                _cached_pool_entry_find(a->entries, a->conf.entry_space, idx);
            cached_pool_entry_t *next = _cached_pool_entry_find(
                a->entries, a->conf.entry_space, idx + 1);
            _cached_pool_entry_set_next(curr, next);
        }
        _cached_pool_entry_set_next(_cached_pool_entry_find(a->entries,
                                                            a->conf.entry_space,
                                                            middle - 1),
                                    NULL);

        _cached_pool_buffer_free(
            a, a->buf,
            _cached_pool_entry_find(a->entries, a->conf.entry_space, middle));
        for (vuint32_t idx = middle; idx < end - 1; idx++) {
            cached_pool_entry_t *curr =
                _cached_pool_entry_find(a->entries, a->conf.entry_space, idx);
            cached_pool_entry_t *next = _cached_pool_entry_find(
                a->entries, a->conf.entry_space, idx + 1);
            _cached_pool_entry_set_next(curr, next);
        }
        _cached_pool_entry_set_next(
            _cached_pool_entry_find(a->entries, a->conf.entry_space, end - 1),
            NULL);
    }
    return a;
}

/**
 * Allocate an entry
 *
 * Cannot be called from different threads with the same id
 *
 * @param a     pointer to the pool data structure
 * @param id    thread ID
 *
 * @return address of the allocated entry (NULL if there is no space)
 */
static inline void *
cached_pool_alloc(cached_pool_t *a, vuint32_t id)
{
    return _cached_pool_vunit_alloc(a, _cached_pool_vunit_find(a, id));
}

/**
 * Free an entry
 *
 * Cannot be called from different threads with the same id
 *
 * @param a         pointer to the pool data structure
 * @param id        thread ID
 * @param p         address of the allocated entry
 *
 */
static inline void
cached_pool_free(cached_pool_t *a, vuint32_t id, void *p)
{
    _cached_pool_vunit_free(a, _cached_pool_vunit_find(a, id), p);
}
#undef CACHED_POOL_MAX_THRESHOLD_FACTOR
#endif
