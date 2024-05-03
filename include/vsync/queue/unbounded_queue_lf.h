/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VQUEUE_UB_LF_H
#define VQUEUE_UB_LF_H
/*******************************************************************************
 * @file  unbounded_queue_lf.h
 * @brief Lock-free unbounded queue.
 *
 * @ingroup linearizable unbounded_queue lock_free requires_smr
 *
 * Operating conditions
 * - This queue requires the presence of an SMR scheme vsync/smr e.g.
 * vsync/smr/gdump.h
 * - Concurrent Queue operations must be called inside the critical section of
 * SMR.
 * - It is recommended to dedicate a special thread for SMR recycling, which
 *  operates in the background and periodically attempts to recycle retired
 *  nodes.
 * - Detached/retired nodes **must be retired** to the SMR and not freed
 *  directly.
 *
 * @example
 * @include eg_unbounded_queue_lf.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 10.5]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/vtypes.h>
#include <vsync/common/assert.h>
#include <vsync/common/verify.h>
#include <vsync/smr/internal/smr_node.h>
#include <vsync/queue/internal/ub/vqueue_ub_common.h>

typedef struct vqueue_ub_node_s {
    void *data;
    vatomicptr(struct vqueue_ub_node_s *) next;
    smr_node_t smr_node;
} vqueue_ub_node_t;

typedef struct vqueue_ub_s {
    vatomicptr(vqueue_ub_node_t *) head;
    vatomicptr(vqueue_ub_node_t *) tail;
    vqueue_ub_node_t sentinel;
} vqueue_ub_t;

/**
 * Initializes the queue.
 *
 * @note must be called before threads start operating on the queue.
 * @note NOT THREAD-SAFE.
 * @param q address of vqueue_ub_t object.
 */
static inline void
vqueue_ub_init(vqueue_ub_t *q)
{
    ASSERT(q);

    q->sentinel.data = NULL;
    vatomicptr_write_rlx(&q->sentinel.next, NULL);

    /* initially both tail and head point to the same sentinel node
     * q->head == q->tail -> empty(q)
     */
    vatomicptr_write_rlx(&q->head, &q->sentinel);
    vatomicptr_write_rlx(&q->tail, &q->sentinel);
}
/**
 * Destroys all remaining nodes in the queue.
 *
 * @note call only after all threads finished operating on the queue.
 * @note NOT THREAD-SAFE.
 * @param q address of vqueue_ub_t object.
 * @param retire function pointer of the callback that handles retiring/freeing
 * the remaining nodes.
 * @param arg extra parameter passed to `retire`.
 */
static inline void
vqueue_ub_destroy(vqueue_ub_t *q, vqueue_ub_node_handler_t retire, void *arg)
{
    vqueue_ub_node_t *curr = (vqueue_ub_node_t *)vatomicptr_read(&q->head);
    vqueue_ub_node_t *next = NULL;

    while (curr) {
        next = (vqueue_ub_node_t *)vatomicptr_read(&curr->next);
        if (curr != &q->sentinel) {
            retire(curr, arg);
        }
        curr = next;
    }
}
/**
 * Enqueues the given node `qnode` in the given queue `q`.
 *
 * @note call in an SMR critical section.
 * @param q address of vqueue_ub_t object.
 * @param qnode address of vqueue_ub_node_t.
 * @param data address of data object to enqueue.
 */
static inline void
vqueue_ub_enq(vqueue_ub_t *q, vqueue_ub_node_t *qnode, void *data)
{
    vqueue_ub_node_t *last = NULL;
    vqueue_ub_node_t *next = NULL;
    vbool_t cas_succeeded  = false;

    ASSERT(q);
    ASSERT(qnode);
    ASSERT(data);

    // init the given qnode
    qnode->data = data;
    vatomicptr_write_rlx(&qnode->next, NULL);

    while (true) {
        last = (vqueue_ub_node_t *)vatomicptr_read(&q->tail);
        next = (vqueue_ub_node_t *)vatomicptr_read(&last->next);

        if (last == vatomicptr_read(&q->tail)) {
            /* next == NULL, means the tail is up to date */
            if (next == NULL) {
                /* try to append the node at the end of the queue */
                if (vatomicptr_cmpxchg(&last->next, next, qnode) == next) {
                    /* Now we try to advance the tail to point to the new node.
                     * If the cmpxchg doesn't succeed, it means we have been
                     * helped. Either way, it is a successful enqueue
                     * LP: successful cas
                     */
                    cas_succeeded =
                        vatomicptr_cmpxchg(&q->tail, last, qnode) == last;

                    verification_assume(cas_succeeded);

                    return;
                } else {
                    /* the next of the tail has changed, since we last checked,
                     * repeat */
                    verification_ignore();
                }
            } else {
                /* #helping
                 *  Some other thread is mid-enqueue, try to help and advance
                 * the tail to tail->next
                 * LP: for the thread that I am helping, if the cmpxchg succeeds
                 */
                verification_ignore();
                vatomicptr_cmpxchg(&q->tail, last, next);
            }
        } else {
            /* the tail has changed since we last checked, repeat */
            verification_ignore();
        }
    } // while
}
/**
 * Dequeues a node from the given queue `q`.
 *
 * @note call in an SMR critical section.
 * @param q address of vqueue_ub_t object.
 * @param retire function pointer of the callback that retires a detached
 * `vqueue_ub_node_t` object to the SMR.
 * @param arg extra parameter passed to `retire`.
 * @return NULL when the queue is empty.
 * @return a non NULL address of the dequeued data object.
 */
static inline void *
vqueue_ub_deq(vqueue_ub_t *q, vqueue_ub_node_handler_t retire, void *retire_arg)
{
    vqueue_ub_node_t *first = NULL;
    vqueue_ub_node_t *last  = NULL;
    vqueue_ub_node_t *next  = NULL;
    void *data              = NULL;

    ASSERT(q);
    ASSERT(retire);

    while (true) {
        first = (vqueue_ub_node_t *)vatomicptr_read(&q->head);
        last  = (vqueue_ub_node_t *)vatomicptr_read(&q->tail);
        next  = (vqueue_ub_node_t *)vatomicptr_read(&first->next);

        if (first == vatomicptr_read(&q->head)) {
            if (first == last) {
                if (next == NULL) {
                    /* LP: for empty queue */
                    return NULL;
                }
                /* #helping
                 * if the next is not NULL, it means someone is trying to
                 * enqueue we help advance the tail from last to next
                 */
                verification_ignore();
                vatomicptr_cmpxchg(&q->tail, last, next);
            } else {
                /* LP: for non-empty queue: successful cmpxchg
                 * we advance the head to next, and return the old head first
                 * note: next is not NULL here, since first != last, so
                 * first->next points to a queue node which recursively points
                 * to last.
                 */
                ASSERT(next &&
                       "next is not expected to be NULL, since the queue is "
                       "not empty");
                data = next->data;
                if (vatomicptr_cmpxchg(&q->head, first, next) == first) {
                    /* first was a sentinel, and now it can be handed to the
                     * SMR. next is the new sentinel */
                    if (first != &q->sentinel) {
                        retire(first, retire_arg);
                    }
                    return data;
                } else {
                    verification_ignore();
                }
            }
        } else { /* if head has changed since we last read it, repeat */
            verification_ignore();
        }
    } // while
}
/**
 * Returns the length of the queue.
 *
 * @note call in an SMR critical section.
 * @note returns an atomic snapshot of the length, which may be stale.
 * @param q address of vqueue_ub_t object.
 * @return vsize_t the length of the queue.
 */
static inline vsize_t
vqueue_ub_get_length(vqueue_ub_t *q)
{
    vsize_t count              = 0;
    vqueue_ub_node_t *head     = NULL;
    vqueue_ub_node_t *tail     = NULL;
    vqueue_ub_node_t *curr     = NULL;
    vbool_t is_sentinel_behind = false;

    ASSERT(q);

    head = (vqueue_ub_node_t *)vatomicptr_read(&q->head);
    tail = (vqueue_ub_node_t *)vatomicptr_read(&q->tail);

    ASSERT(head && "head should always point to a sentinel");

    curr = head;

    while (curr != tail) {
        count++;
        ASSERT(!is_sentinel_behind || curr != &q->sentinel);
        curr               = (vqueue_ub_node_t *)vatomicptr_read(&curr->next);
        is_sentinel_behind = true;
    }

    V_UNUSED(is_sentinel_behind);
    return count;
}
/**
 * Checks if the queue is empty.
 *
 * @note Use with caution this is subject to ABA problems, might return empty
 * when the queue is not empty.
 * @note can be called outside the SMR critical section.
 * @param q address of vqueue_ub_t object
 * @return true the queue is empty
 * @return false the queue is not empty
 */
static inline vbool_t
vqueue_ub_empty(vqueue_ub_t *q)
{
    vqueue_ub_node_t *first = NULL;
    vqueue_ub_node_t *last  = NULL;
    ASSERT(q);

    first = (vqueue_ub_node_t *)vatomicptr_read(&q->head);
    last  = (vqueue_ub_node_t *)vatomicptr_read(&q->tail);
    return first == last;
}

/**
 * Visits all available nodes in the queue.
 *
 * @note NOT-THREAD-SAFE.
 * @param q address of vqueue_ub_t object.
 * @param visitor function pointer of visitor function.
 * @param arg extra argument passed to visitor function if needed.
 */
static inline void
_vqueue_ub_visit_nodes(vqueue_ub_t *q, vqueue_ub_node_handler_t visitor,
                       void *arg)
{
    vqueue_ub_node_t *curr = NULL;
    vqueue_ub_node_t *next = NULL;

    curr = (vqueue_ub_node_t *)vatomicptr_read(&q->head);
    // skip the sentinel
    curr = (vqueue_ub_node_t *)vatomicptr_read(&curr->next);

    while (curr) {
        next = (vqueue_ub_node_t *)vatomicptr_read(&curr->next);
        visitor(curr, arg);
        curr = next;
    }
}
#endif
