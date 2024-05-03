/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VQUEUE_UB_TOTAL_H
#define VQUEUE_UB_TOTAL_H

/*******************************************************************************
 * @file  unbounded_queue_total.h
 * @brief Unbounded blocking total queue.
 *
 * @ingroup linearizable unbounded_queue
 *
 * Consumers can run in parallel to producers. Consumers block other consumers,
 * but not producers and vice versa.
 *
 * @example
 * @include eg_unbounded_queue_total.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 10.4]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/
#include <vsync/vtypes.h>
#include <vsync/atomic.h>
#include <vsync/utils/abstract_lock.h>
#include <vsync/spinlock/ttaslock.h>
#include <vsync/common/assert.h>
#include <vsync/queue/internal/ub/vqueue_ub_common.h>

DEF_ABSTRACT_LOCK(queue_lock, ttaslock_t, ttaslock_init, ttaslock_acquire,
                  ttaslock_release, ttaslock_tryacquire)

typedef void (*vqueue_ub_node_print)(void *);

typedef struct vqueue_ub_node_s {
    void *data;
    vatomicptr(struct vqueue_ub_node_s *) next;
} vqueue_ub_node_t;

typedef struct vqueue_ub_s {
    queue_lock_t enq_l;
    queue_lock_t deq_l;
    vqueue_ub_node_t *head;
    vqueue_ub_node_t *tail;
    vqueue_ub_node_t sentinel;
} vqueue_ub_t;

/**
 *  PUBLIC FUNCTIONS
 */
static inline void vqueue_ub_init(vqueue_ub_t *q);

static inline void vqueue_ub_enq(vqueue_ub_t *q, vqueue_ub_node_t *qnode,
                                 void *);

static inline vsize_t vqueue_ub_get_length(vqueue_ub_t *q);

static inline void *vqueue_ub_deq(vqueue_ub_t *q,
                                  vqueue_ub_node_handler_t retire,
                                  void *retire_arg);

/**
 *  INTERNAL/PRIVATE FUNCTIONS
 */
static inline void _vqueue_ub_node_init(vqueue_ub_node_t *qnode, void *data);

/**
 * Initialize queue.
 *
 * @note must be called before threads start operating on the queue
 * @param q address of vqueue_ub_t object
 *
 */
static inline void
vqueue_ub_init(vqueue_ub_t *q)
{
    q->head = &q->sentinel;
    q->tail = &q->sentinel;

    _vqueue_ub_node_init(&q->sentinel, NULL);

    queue_lock_init(&q->enq_l);
    queue_lock_init(&q->deq_l);
}

/**
 * Destroys all remaining nodes in the queue.
 *
 * @note call only after all threads finished operating on the queue.
 * @note NOT THREAD-SAFE.
 * @param q address of vqueue_ub_t object.
 * @param retire function pointer of the callback that handles freeing
 * the remaining nodes.
 * @param arg extra parameter passed to `retire`.
 */
static inline void
vqueue_ub_destroy(vqueue_ub_t *q, vqueue_ub_node_handler_t retire,
                  void *retire_arg)
{
    vqueue_ub_node_t *curr = NULL;
    vqueue_ub_node_t *next = NULL;

    curr = q->head;

    while (curr) {
        next = (vqueue_ub_node_t *)vatomicptr_read_rlx(&curr->next);
        if (curr != &q->sentinel) {
            retire(curr, retire_arg);
        }
        curr = next;
    }
}
/**
 * Enqueue the given node `qnode` in the given queue `q`.
 *
 * @param q address of vqueue_ub_t object
 * @param qnode address of vqueue_ub_node_t
 * @param data address of data object to enqueue.
 */
static inline void
vqueue_ub_enq(vqueue_ub_t *q, vqueue_ub_node_t *qnode, void *data)
{
    queue_lock_acquire(&q->enq_l);
    // #different here it differs from the original
    // algo. To prevent bugs on WMM.
    _vqueue_ub_node_init(qnode, data);
    // append at the tail
    vatomicptr_write_rel(&q->tail->next, qnode);
    // advance the tail
    q->tail = qnode;
    queue_lock_release(&q->enq_l);
}

/**
 * Check if the queue is empty.
 *
 * @param q address of vqueue_ub_t object
 * @return true the queue is empty
 * @return false the queue is not empty
 */
static inline vbool_t
vqueue_ub_empty(vqueue_ub_t *q)
{
    vqueue_ub_node_t *qnode = NULL;
    vqueue_ub_node_t *head  = NULL;

    queue_lock_acquire(&q->deq_l);
    head = q->head;
    // read the next of the head
    qnode = (vqueue_ub_node_t *)vatomicptr_read_acq(&head->next);
    queue_lock_release(&q->deq_l);
    return qnode == NULL;
}
/**
 * Dequeue a node from the given queue `q`.
 *
 * @param q address of vqueue_ub_t object.
 * @param retire function pointer of the callback that frees the detached
 * `vqueue_ub_node_t`.
 * @param arg extra parameter passed to `retire`.
 * @return NULL when the queue is empty.
 * @return a non NULL address of the dequeued data object.
 */
static inline void *
vqueue_ub_deq(vqueue_ub_t *q, vqueue_ub_node_handler_t retire, void *retire_arg)
{
    vqueue_ub_node_t *qnode = NULL;
    vqueue_ub_node_t *head  = NULL;
    void *data              = NULL;

    queue_lock_acquire(&q->deq_l);

    head = q->head;
    // read the next of the head
    qnode = (vqueue_ub_node_t *)vatomicptr_read_acq(&head->next);
    if (qnode) {
        data    = qnode->data;
        q->head = qnode;
        if (head != &q->sentinel) {
            retire(head, retire_arg);
        }
    }
    queue_lock_release(&q->deq_l);
    return data;
}
/**
 * Returns the length of the queue.
 *
 * @note this function will block both dequeues and enqueues
 * @param q address of vqueue_ub_t object
 * @return vsize_t the length of the queue
 */
static inline vsize_t
vqueue_ub_get_length(vqueue_ub_t *q)
{
    vsize_t count          = 0;
    vqueue_ub_node_t *curr = NULL;

    queue_lock_acquire(&q->deq_l);
    queue_lock_acquire(&q->enq_l);
    // don't count the sentinel
    curr = (vqueue_ub_node_t *)vatomicptr_read_rlx(&q->head->next);
    while (curr) {
        curr = (vqueue_ub_node_t *)vatomicptr_read_rlx(&curr->next);
        ASSERT(curr != &q->sentinel);
        count++;
    }
    queue_lock_release(&q->enq_l);
    queue_lock_release(&q->deq_l);
    return count;
}
/**
 * Initialize node `qnode`.
 *
 * @param qnode address of vqueue_ub_node_t
 */
static inline void
_vqueue_ub_node_init(vqueue_ub_node_t *qnode, void *data)
{
    qnode->data = data;
    vatomicptr_write_rlx(&qnode->next, NULL);
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

    curr = q->head;
    // skipping sentinel
    curr = (vqueue_ub_node_t *)vatomicptr_read_rlx(&curr->next);

    while (curr) {
        next = (vqueue_ub_node_t *)vatomicptr_read_rlx(&curr->next);
        visitor(curr, arg);
        curr = next;
    }
}
#endif
