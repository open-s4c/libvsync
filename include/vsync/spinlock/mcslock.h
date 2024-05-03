/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_MCSLOCK_H
#define VSYNC_MCSLOCK_H
/*******************************************************************************
 * @file mcslock.h
 * @ingroup fair_lock
 * @brief Mellor-Crummey Scott Lock - the well-known scalable lock.
 *
 * MCS lock is a scalable lock that guarantees FIFO order. The MCS lock forms
 * a queue of waiting threads (each thread has one node in the queue), and lets
 * them spin on local variables of their nodes.
 *
 * Note that this implementation is not reentrant.
 *
 * @example
 * @include eg_mcslock.c
 *
 * @cite
 * J.Mellor-Crummey, M.L.Scott - [Algorithms for scalable synchronization
 * on shared-memory multiprocessors. ACM Trans. on Computer Systems. 1991]
 *
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/common/cache.h>
#include <vsync/vtypes.h>

/** Node of a thread/core in the MCS lock.  */
typedef struct mcs_node_s {
    vatomicptr(struct mcs_node_s *) next;
    vatomic32_t locked;
} VSYNC_CACHEALIGN mcs_node_t;

/** MCS lock data structure. */
typedef struct mcslock_s {
    vatomicptr(mcs_node_t *) tail;
} VSYNC_CACHEALIGN mcslock_t;

/** Initializer of `mcslock_t`. */
#define MCSLOCK_INIT()                                                         \
    {                                                                          \
        .tail = VATOMIC_INIT(0)                                                \
    }

/**
 * Initializes the MCS lock.
 *
 * @param l address of mcslock_t object.
 *
 * @note alternatively use `MCSLOCK_INIT`.
 */
static inline void
mcslock_init(mcslock_t *l)
{
    vatomicptr_write(&l->tail, NULL);
}
/**
 * Tries to acquire the MCS lock.
 *
 * @param l address of mcslock_t object.
 * @param node address of mcs_node_t object, associated with the calling
 * thread/core.
 * @return true, on success.
 * @return false, on fail.
 */
static inline vbool_t
mcslock_tryacquire(mcslock_t *l, mcs_node_t *node)
{
    mcs_node_t *pred;

    vatomicptr_write_rlx(&node->next, NULL);
    vatomic32_write_rlx(&node->locked, 1);

    pred = (mcs_node_t *)vatomicptr_cmpxchg(&l->tail, NULL, node);

    return pred == NULL;
}
/**
 * Acquires the MCS lock.
 *
 * @param l address of mcslock_t object.
 * @param node address of mcs_node_t object, associated with the calling
 * thread/core.
 */
static inline void
mcslock_acquire(mcslock_t *l, mcs_node_t *node)
{
    mcs_node_t *pred;

    vatomicptr_write_rlx(&node->next, NULL);
    vatomic32_write_rlx(&node->locked, 1);

    pred = (mcs_node_t *)vatomicptr_xchg(&l->tail, node);
    if (pred) {
        vatomicptr_write_rel(&pred->next, node);
        vatomic32_await_eq_acq(&node->locked, 0);
    }
}
/**
 * Releases the MCS lock.
 *
 * @param l address of mcslock_t object.
 * @param node address of mcs_node_t object, associated with the calling
 * thread/core.
 */
static inline void
mcslock_release(mcslock_t *l, mcs_node_t *node)
{
    mcs_node_t *next;

    if (vatomicptr_read_rlx(&node->next) == NULL) {
        next = (mcs_node_t *)vatomicptr_cmpxchg_rel(&l->tail, node, NULL);
        if (next == node) {
            return;
        }
        vatomicptr_await_neq_rlx(&node->next, NULL);
    }
    next = (mcs_node_t *)vatomicptr_read_acq(&node->next);
    vatomic32_write_rel(&next->locked, 0);
}
/**
 * Returns whether there is a thread waiting to acquire the MCS lock.
 *
 * This function should only be called by the current owner of the lock.
 *
 * @param l address of mcslock_t object.
 * @param node address of mcs_node_t object, associated with the calling
 * thread/core.
 * @return true, if threads are waiting to acquire the lock.
 * @return false, otherwise.
 */
static inline vbool_t
mcslock_has_waiters(mcslock_t *l, mcs_node_t *node)
{
    V_UNUSED(l);
    return (vatomicptr_read_rlx(&node->next) != NULL);
}
#endif
