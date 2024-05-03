/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VQUEUE_BOUNDED_LOCKED_H
#define VQUEUE_BOUNDED_LOCKED_H
/*******************************************************************************
 * @file bounded_locked.h
 * @brief Multi-producer, multi-consumer bounded queue protected by a spinlock
 * @ingroup linearizable
 *
 * This is a non-blocking, multi-producer, multi-consumer queue protected by a
 * spinlock. The queue has a bounded size and returns errors in case the queue
 * is full, empty or the spinlock cannot be acquired.
 *
 * @note This queue implementation is very simple and may be inefficient
 * due to the spinlock bottleneck. Prefer using @ref bounded_mpmc.h instead.
 *
 * @example
 * @include eg_bounded_locked.c
 *
 ******************************************************************************/
#include <vsync/vtypes.h>
#include <vsync/common/assert.h>
#include <vsync/atomic.h>
#include <vsync/spinlock/caslock.h>
#include <vsync/queue/internal/bounded_ret.h>

typedef struct bounded_locked_s {
    void **buf;
    vuint32_t head;
    vuint32_t tail;
    vuint32_t size;
    caslock_t lock;
} bounded_locked_t;

/**
 * Initializes the given queue object.
 *
 * @param q address of bounded_locked_t object.
 * @param b address of an array of void* slots with size `s`.
 * @param s capacity of `b` (in number of array elements/slots).
 */
static inline void
bounded_locked_init(bounded_locked_t *q, void **b, vuint32_t s)
{
    ASSERT(b && "buffer is NULL");
    ASSERT(s != 0 && "buffer with 0 size");

    q->buf  = b;
    q->size = s;
    q->head = 0;
    q->tail = 0;
    caslock_init(&q->lock);
}
/**
 * Tries to enqueue a value.
 *
 * @param q address of bounded_locked_t object.
 * @param v address of object to enqueue.
 *
 * @return QUEUE_BOUNDED_OK if successful.
 * @return QUEUE_BOUNDED_FULL if queue is full.
 * @return QUEUE_AGAIN if queue is currently locked and the
 * caller should try again.
 */
static inline bounded_ret_t
bounded_locked_enq(bounded_locked_t *q, void *v)
{
    ASSERT(q && "q == NULL");
    ASSERT(v && "v == NULL");

    if (!caslock_tryacquire(&q->lock)) {
        return QUEUE_BOUNDED_AGAIN;
    }

    ASSERT(q->tail - q->head <= q->size && "inconsistent state");

    vuint32_t head = q->head;
    if (q->tail - head == q->size) {
        caslock_release(&q->lock);
        return QUEUE_BOUNDED_FULL;
    }

    q->buf[q->tail++ % q->size] = v;

    caslock_release(&q->lock);
    return QUEUE_BOUNDED_OK;
}
/**
 * Tries to dequeue a value.
 *
 * @param q address of bounded_locked_t object.
 * @param v output parameter of type (void**). Contains the address of the
 * dequeued object, if the dequeue was successful.
 *
 * @return QUEUE_BOUNDED_OK if successful.
 * @return QUEUE_BOUNDED_EMPTY if queue is empty.
 * @return QUEUE_BOUNDED_AGAIN if queue is currently locked and the caller
 * should try again.
 *
 * @note The output parameter `v` is only set when QUEUE_BOUNDED_OK is returned.
 */
static inline bounded_ret_t
bounded_locked_deq(bounded_locked_t *q, void **v)
{
    ASSERT(q && "q == NULL");
    ASSERT(v && "v == NULL");

    if (!caslock_tryacquire(&q->lock)) {
        return QUEUE_BOUNDED_AGAIN;
    }

    ASSERT(q->head <= q->tail && "head > tail");

    vuint32_t tail = q->tail;
    if (tail - q->head == 0) {
        caslock_release(&q->lock);
        return QUEUE_BOUNDED_EMPTY;
    }

    *v = q->buf[q->head++ % q->size];

    caslock_release(&q->lock);
    return QUEUE_BOUNDED_OK;
}
/**
 * Tests whether queue is empty without performing an action.
 *
 * @note this function is blocking.
 *
 * @param q address of bounded_locked_t object.
 *
 * @return true if empty.
 * @return false if not empty.
 */
static inline vbool_t
bounded_locked_empty(bounded_locked_t *q)
{
    ASSERT(q && "q == NULL");

    caslock_acquire(&q->lock);
    ASSERT(q->head <= q->tail && "head > tail");
    vbool_t is_empty = q->tail == q->head;
    caslock_release(&q->lock);

    return is_empty;
}
#endif
