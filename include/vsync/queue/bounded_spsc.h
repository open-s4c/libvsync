/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VQUEUE_BOUNDED_SPSC_H
#define VQUEUE_BOUNDED_SPSC_H
/*******************************************************************************
 * @file bounded_spsc.h
 * @brief Single-producer, single-consumer, wait-free bounded queue
 * @ingroup lock_free linearizable
 *
 * Implementation of the classic single-producer, single-consumer, wait-free
 * bounded queue algorithm.
 *
 * @example
 * @include eg_bounded_spsc.c
 *
 * @cite
 * L. Lamport. - [Specifying concurrent program modules]
 * (ACM Transactions on Programming Languages and Systems, 5(2):190-222, 1983.)
 *
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/common/assert.h>
#include <vsync/queue/internal/bounded_ret.h>

typedef struct {
    void **buf;
    vatomic32_t head;
    vatomic32_t tail;
    vuint32_t size;
} bounded_spsc_t;

/**
 * Initializes the given queue object.
 *
 * @param q address of bounded_spsc_t object.
 * @param b address of an array of void* slots with size `s`.
 * @param s number of elements in array b (does not have to be power of 2).
 */
static inline void
bounded_spsc_init(bounded_spsc_t *q, void **b, vuint32_t s)
{
    ASSERT(b && "buffer is NULL");
    ASSERT(s != 0 && "buffer with 0 size");

    q->buf  = b;
    q->size = s;

    vatomic32_init(&q->head, 0);
    vatomic32_init(&q->tail, 0);
}
/**
 * Tries to enqueue a value.
 *
 * @param q address of bounded_spsc_t object.
 * @param v address of object to enqueue.
 *
 * @return QUEUE_BOUNDED_OK if successful.
 * @return QUEUE_BOUNDED_FULL if queue is full.
 */
static inline bounded_ret_t
bounded_spsc_enq(bounded_spsc_t *q, void *v)
{
    ASSERT(q && "q == NULL");
    ASSERT(v && "v == NULL");

    vuint32_t tail = vatomic32_read_rlx(&q->tail);
    vuint32_t head = vatomic32_read_rlx(&q->head);
    // check if it is full
    if (tail - head == q->size) {
        return QUEUE_BOUNDED_FULL;
    }

    q->buf[tail % q->size] = v;
    vatomic32_write_rel(&q->tail, tail + 1);

    return QUEUE_BOUNDED_OK;
}
/**
 * Tries to dequeue a value.
 *
 * @param q address of bounded_spsc_t object.
 * @param v output parameter of type (void**). Contains the address of the
 * dequeued object, if the queue is not empty.
 *
 * @return QUEUE_BOUNDED_OK if successful.
 * @return QUEUE_BOUNDED_EMPTY if queue is empty.
 */
static inline bounded_ret_t
bounded_spsc_deq(bounded_spsc_t *q, void **v)
{
    ASSERT(q && "q == NULL");
    ASSERT(v && "v == NULL");

    vuint32_t head = vatomic32_read_rlx(&q->head);
    vuint32_t tail = vatomic32_read_acq(&q->tail);
    // check if it is empty
    if (tail - head == 0) {
        return QUEUE_BOUNDED_EMPTY;
    }

    *v = q->buf[head % q->size];
    vatomic32_write_rel(&q->head, head + 1);

    return QUEUE_BOUNDED_OK;
}
#endif
