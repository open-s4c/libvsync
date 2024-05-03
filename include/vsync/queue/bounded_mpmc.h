/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VQUEUE_BOUNDED_H
#define VQUEUE_BOUNDED_H
/*******************************************************************************
 * @file bounded_mpmc.h
 * @brief Lockless, multi-producer, multi-consumer bounded queue
 * @ingroup linearizable
 *
 * The queue has a bounded size and returns errors in case the queue is full,
 * empty, or if there is contention while trying to reserve an entry to enqueue
 * or dequeue.
 *
 * @example
 * @include eg_bounded_mpmc.c
 *
 * Typically, one tries to enqueue or dequeue as long the return value is
 * different from QUEUE_BOUNDED_OK.
 *
 * The retrying loop can be combined with exponential backoff.
 *
 * @cite This is a variation of the algorithm in buf_ring.h of FreeBSD 8.0.0.
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/common/assert.h>
#include <vsync/queue/internal/bounded_ret.h>

typedef struct bounded_mpmc_s {
    vatomic32_t phead;
    vatomic32_t ptail;

    vatomic32_t chead;
    vatomic32_t ctail;

    void **buf;
    vuint32_t size;
} bounded_mpmc_t;

/**
 * Initializes the given queue object.
 *
 * @param q address of bounded_mpmc_t object.
 * @param b address of an array of void* slots with size `s`.
 * @param s number of elements in array b (does not have to be power of 2).
 */
static inline void
bounded_mpmc_init(bounded_mpmc_t *q, void **b, vuint32_t s)
{
    ASSERT(b && "buffer is NULL");
    ASSERT(s != 0 && "buffer with 0 size");

    q->buf  = b;
    q->size = s;
    vatomic32_init(&q->chead, 0);
    vatomic32_init(&q->ctail, 0);
    vatomic32_init(&q->phead, 0);
    vatomic32_init(&q->ptail, 0);
}

/**
 * Tries to enqueue a value.
 *
 * @param q address of bounded_mpmc_t object.
 * @param v address of object to enqueue.
 *
 * @return QUEUE_BOUNDED_OK if successful.
 * @return QUEUE_BOUNDED_FULL if queue is full.
 * @return QUEUE_BOUNDED_AGAIN if failed to enqueue, the caller should try
 * again.
 */
static inline bounded_ret_t
bounded_mpmc_enq(bounded_mpmc_t *q, void *v)
{
    vuint32_t curr, next;

    /* try to move producer head */
    curr = vatomic32_read_acq(&q->phead);
    if (curr - vatomic32_read_rlx(&q->ctail) == q->size) {
        return QUEUE_BOUNDED_FULL;
    }
    next = curr + 1;
    if (vatomic32_cmpxchg_rel(&q->phead, curr, next) != curr) {
        return QUEUE_BOUNDED_AGAIN;
    }
    /* push value into buffer */
    q->buf[curr % q->size] = v;

    /* mode producer tail */
    vatomic32_await_eq_acq(&q->ptail, curr);
    vatomic32_write_rel(&q->ptail, next);

    return QUEUE_BOUNDED_OK;
}

/**
 * Tries to dequeue a value.
 *
 * @param q address of bounded_mpmc_t object.
 * @param v output parameter of type (void**). Contains the address of the
 * dequeued object, if the dequeue was successful.
 *
 * @return QUEUE_BOUNDED_OK if successful.
 * @return QUEUE_BOUNDED_EMPTY if queue is empty.
 * @return QUEUE_BOUNDED_AGAIN if failed to dequeue, the caller should try
 * again.
 */
static inline bounded_ret_t
bounded_mpmc_deq(bounded_mpmc_t *q, void **v)
{
    vuint32_t curr, next;

    /* try to move consumer head */
    curr = vatomic32_read_acq(&q->chead);
    next = curr + 1;
    if (curr == vatomic32_read_acq(&q->ptail)) {
        return QUEUE_BOUNDED_EMPTY;
    }
    if (vatomic32_cmpxchg_rel(&q->chead, curr, next) != curr) {
        return QUEUE_BOUNDED_AGAIN;
    }
    /* read value */
    *v = q->buf[curr % q->size];

    /* move consumer tail */
    vatomic32_await_eq_rlx(&q->ctail, curr);
    vatomic32_write_rel(&q->ctail, next);

    return QUEUE_BOUNDED_OK;
}

#endif
