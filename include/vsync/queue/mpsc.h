/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * Copyright (c) 2010-2011 Dmitry Vyukov. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice,
 * this list of
 *
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 * notice, this list
 *
 *       of conditions and the following disclaimer in the documentation and/or
 * other materials
 *
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY DMITRY VYUKOV "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL DMITRY VYUKOV OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Dmitry Vyukov.
 */
#ifndef VSYNC_MPSC_H
#define VSYNC_MPSC_H
/**
 * @file mpsc.h
 * @brief Multi-producer single-consumer queue.
 *
 * Enqueue operation is wait-free.
 *
 * @ingroup unbounded_queue
 *
 * @example
 * @include eg_mpsc.c
 *
 * @cite [Non-intrusive MPSC node-based queue]
 * (https://www.1024cores.net/home/lock-free-algorithms/queues/non-intrusive-mpsc-node-based-queue)
 *
 */
#include <vsync/atomic.h>
#include <vsync/common/assert.h>
#include <vsync/common/compiler.h>

typedef struct vmpsc_node_s {
    void *data; /* address of data object associated with the node */
    vatomicptr_t
        next; /* address of vmpsc_node_t object connected to this node */
} vmpsc_node_t;

typedef struct vmpsc_s {
    vatomicptr_t tail;     /* address of most recent node in the queue */
    vmpsc_node_t *head;    /* address of least recent node in the queue */
    vmpsc_node_t sentinel; /* sentinel node */
} vmpsc_t;

typedef void (*vmpsc_visit_t)(vmpsc_node_t *node, void *args);
typedef vbool_t (*vmpsc_data_visit_t)(void *data, void *args);
/**
 * Initializes the given queue object `q`.
 *
 * @param q address of vmpsc_t object.
 *
 * @note call only before threads start accessing the queue.
 */
static inline void
vmpsc_init(vmpsc_t *q)
{
    ASSERT(q);
    vmpsc_node_t *sentinel = &q->sentinel;
    sentinel->data         = NULL;
    vatomicptr_write_rlx(&sentinel->next, NULL);
    /* both head and tail point to the sentinel */
    q->head = sentinel;
    vatomicptr_write_rlx(&q->tail, sentinel);
}
/**
 * Calls the given `f` function on each enqueued node's data.
 *
 * @param q address of vmpsc_t object.
 * @param f function pointer of type vmpsc_data_visit_t.
 * @param args extra arguments of `f`.
 *
 * @note Don't call while producer threads are active.
 */
static inline void
vmpsc_foreach(const vmpsc_t *q, vmpsc_data_visit_t f, void *args)
{
    vmpsc_node_t *cur  = q->head;
    vmpsc_node_t *next = NULL;
    while (cur) {
        /* the first node of the queue is always a sentinel */
        next = vatomicptr_read_rlx(&cur->next);
        if (next) {
            if (!f(next->data, args)) {
                break;
            };
        }
        cur = next;
    }
}
/**
 * Calls the given `f` function on each enqueued node.
 *
 * @param q address of vmpsc_t object.
 * @param f function pointer of type vmpsc_visit_t.
 * @param args extra arguments of `f`.
 *
 * @note call only after all threads are done accessing the queue.
 * @note use the callback function to destruct nodes and data objects.
 */
static inline void
vmpsc_destroy(vmpsc_t *q, vmpsc_visit_t f, void *args)
{
    vmpsc_node_t *cur  = q->head;
    vmpsc_node_t *next = NULL;
    while (cur) {
        next = vatomicptr_read_rlx(&cur->next);
        if (cur != &q->sentinel) {
            f(cur, args);
        }
        cur = next;
    }
}
/**
 * Enqueues the given node into the given queue.
 *
 * @param q address of vmpsc_t object.
 * @param node address of vmpsc_node_t object to enqueue.
 * @param data address of data object to be associated with the given node.
 */
static inline void
vmpsc_enq(vmpsc_t *q, vmpsc_node_t *node, void *data)
{
    vmpsc_node_t *tail = NULL;
    ASSERT(q);
    ASSERT(node);
    ASSERT(data);
    node->data = data;
    vatomicptr_write_rlx(&node->next, NULL);
    /* move the tail to node, and record the old tail */
    tail = vatomicptr_xchg(&q->tail, node);
    ASSERT(tail);
    /* connect the old tail to node */
    vatomicptr_write_rel(&tail->next, node);
}
/**
 * Dequeues a node and data object from the queue.
 *
 * @param q address of vmpsc_t object.
 * @param discarded_node output parameter. Address of vmpsc_node_t sentinel
 * object to destroy, or NULL.
 *
 * @return void* address of dequeued data object.
 * @return NULL when the queue is empty.
 *
 * @note on each successful dequeue there will be a sentinel node object kicked
 * out of the queue.
 */
static inline void *
vmpsc_deq(vmpsc_t *q, vmpsc_node_t **discarded_node)
{
    void *entry        = NULL;
    vmpsc_node_t *head = q->head;
    vmpsc_node_t *next = vatomicptr_read_acq(&head->next);
    *discarded_node    = NULL;
    if (next == NULL) {
        if (vatomicptr_read_rlx(&q->tail) == head) {
            return NULL;
        }
        /* tail moved, a producer is mid-enq on wait for the entry to become
         * available */
        next = vatomicptr_await_neq_acq(&head->next, NULL);
    }
    entry = next->data;
    assert(entry);
    next->data = NULL;
    q->head    = next;

    if (likely(head != &q->sentinel)) {
        /*
         * NOTE: Why is SMR not needed here?
         * At this point `head->next` is not NULL.
         * and in line vatomicptr_write_rel(&tail->next, node); of deq
         * tail->next is NULL. hence tail != head.
         * and it is safe to free head.
         */
        *discarded_node = head;
    }
    return entry;
}
/**
 * Checks if the current queue is empty or not.
 *
 * @param q address of vmpsc_t object.
 *
 * @return true the queue is empty.
 * @return false the queue is not empty.
 *
 * @note to be called only by the consumer.
 */
static inline vbool_t
vmpsc_empty(vmpsc_t *q)
{
    vmpsc_node_t *head = q->head;
    vmpsc_node_t *tail = vatomicptr_read(&q->tail);
    vmpsc_node_t *next = vatomicptr_read(&tail->next);
    return (next == NULL) && (head == tail);
}
/**
 * Calculates how many nodes are enqueued in the queue.
 *
 * @param q  address of vmpsc_t object.
 * @return vsize_t number of available elements in the queue.
 *
 * @note to be called only by the consumer.
 * @note only non-sentinel nodes are counted.
 */
static inline vsize_t
vmpsc_length(vmpsc_t *q)
{
    vmpsc_node_t *curr = q->head;
    vmpsc_node_t *next = NULL;
    vsize_t len        = 0;
    while (curr) {
        // TODO: check next of tail!
        next = vatomicptr_read(&curr->next);
        if (next) {
            len++;
        }
        curr = next;
    }
    return len;
}
#endif
