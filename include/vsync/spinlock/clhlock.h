/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_CLHLOCK_H
#define VSYNC_CLHLOCK_H
/*******************************************************************************
 * @file clhlock.h
 * @ingroup fair_lock
 * @brief List-based lock attributed to Craig, Landin and Hagersten.
 *
 * @example
 * @include eg_clhlock.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 7.5]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/common/cache.h>

/**
 * Each thread has its own qnode (`clh_qnode_t`). When `locked` is set to 1, the
 * thread wants to enter the critical section.
 */
typedef struct clh_qnode_s {
    vatomic32_t locked;
} VSYNC_CACHEALIGN clh_qnode_t;

typedef struct clh_node_s {
    clh_qnode_t *pred;  /* points to the pred node */
    clh_qnode_t *qnode; /* points to the used qnode by the owner thread */
    clh_qnode_t _qnode; /* inital own qnode, later is used by other threads */
} clh_node_t;

/** The lock holds a pointer to the tail of the queue */
typedef struct clhlock_s {
    vatomicptr_t tail;
    clh_qnode_t initial;
} clhlock_t;

/**
 * Initializes the given node.
 *
 * @param node address of clh_node_t object.
 */
static inline void
clhlock_node_init(clh_node_t *node)
{
    vatomic32_init(&node->_qnode.locked, 0);
    node->qnode = &node->_qnode;
    node->pred  = NULL;
}
/**
 * Initializes the given lock.
 *
 * @param lock address of clhlock_t object.
 */
static inline void
clhlock_init(clhlock_t *lock)
{
    vatomic32_init(&lock->initial.locked, 0);
    vatomicptr_init(&lock->tail, &lock->initial);
}
/**
 * Acquires the lock.
 *
 * @param lock address of clhlock_t object.
 * @param node address of clh_node_t object associated with the calling thread.
 *
 * @note `node` has to continue to exist even if the thread dies.
 */
static inline void
clhlock_acquire(clhlock_t *lock, clh_node_t *node)
{
    // declare your intention of acquiring the lock
    vatomic32_write_rlx(&node->qnode->locked, 1);

    // find out, who has acquired the lock before me
    // the tail always points to the thread infront of me
    // put me inline and let the tail points to me
    node->pred = (clh_qnode_t *)vatomicptr_xchg(&lock->tail, node->qnode);

    // wait on my predecessor to free the lock
    vatomic32_await_eq_acq(&node->pred->locked, 0);

    // I have the lock by now
}
/**
 * Releases the lock.
 *
 * @param lock address of clhlock_t object.
 * @param node address of clh_node_t object associated with the calling thread.
 *
 * @note It hijacks its predecessor's queue node as its own.
 */
static inline void
clhlock_release(clhlock_t *lock, clh_node_t *node)
{
    // free the lock
    vatomic32_write_rel(&node->qnode->locked, 0);

    // node recycling: use your predecessor node as your own in future runs
    node->qnode = node->pred;

    V_UNUSED(lock);
}
/**
 * Returns whether there is a thread waiting to acquire the clhlock.
 *
 * This function should only be called by the current owner of the lock.
 *
 * @param lock address of clhlock_t object.
 * @param node address of clh_node_t object associated with the calling thread.
 *
 * @return true if there are waiters.
 * @return false if there are no waiters.
 *
 * @note this function is not part of the standard lock API.
 */
static inline vbool_t
clhlock_has_waiters(clhlock_t *lock, clh_node_t *node)
{
    clh_qnode_t *tail = (clh_qnode_t *)vatomicptr_read_rlx(&lock->tail);

    return tail != node->qnode;
}

#endif /* VSYNC_CLHLOCK_H */
