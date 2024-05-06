/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_HCLHLOCK_H
#define VSYNC_HCLHLOCK_H

#include <vsync/atomic.h>
#include <vsync/vtypes.h>
#include <vsync/common/assert.h>

/*******************************************************************************
 * @file hclhlock.h
 * @ingroup fair_lock numa_aware
 * @brief Hierarchical CLH Queue Lock.
 *
 * @example
 * @include eg_hclh.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 7.8.2]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/

/**
 * @def HCLH_MAX_CLUSTERS
 * @brief configures the number of available clusters in HCLH lock.
 *
 * Each cluster has a local queue. Each thread must be
 * associated with a cluster value in `[0: HCLH_MAX_CLUSTERS -1]`
 *
 * A cluster can be thought of as a NUMA node or cache group. Threads
 * running on cores that belong to the same cache-group or NUMA node are
 * expected to be associated with the same cluster.
 *
 * @note by default 32 clusters are defined. Users can redefine it using:
 * `-DHCLH_MAX_CLUSTERS=N`, where `N` is the number of clusters the user wishes
 * to set.
 *
 */
#ifndef HCLH_MAX_CLUSTERS
    #define HCLH_MAX_CLUSTERS 32
#endif

/*
 * TWS: Tail When Spliced
 * MSB  bits[31:31] = 1
 */
#define HCLH_TWS_MASK           0x80000000U
#define HCLH_SET_TWS_TRUE(_s_)  ((_s_) |= HCLH_TWS_MASK)
#define HCLH_SET_TWS_FALSE(_s_) ((_s_) &= (~HCLH_TWS_MASK))
#define HCLH_IS_TWS(_s_)        ((((_s_)&HCLH_TWS_MASK) >> 31U) == 1U)

/*
 * SMW: Successor Must Wait
 * bits[30:30] = 1
 */
#define HCLH_SMW_MASK           0x40000000U
#define HCLH_SET_SMW_TRUE(_s_)  ((_s_) |= HCLH_SMW_MASK)
#define HCLH_SET_SMW_FALSE(_s_) ((_s_) &= (~HCLH_SMW_MASK))
#define HCLH_IS_SMW(_s_)        ((((_s_)&HCLH_SMW_MASK) >> 30U) == 1U)

/*
 * Cluster value in
 * bits [29:00]
 */
#define HCLH_CLUSTER_MASK 0x3FFFFFFFU
#define HCLH_SET_CLUSTER(_s_, _c_)                                             \
    ((_s_) = ((_s_) & (~HCLH_CLUSTER_MASK)) | (_c_))
#define HCLH_GET_CLUSTER(_s_) ((_s_)&HCLH_CLUSTER_MASK)

typedef struct hclh_qnode_s {
    vatomic32_t
        state; /* State encodes the following information
                * - state[31:31]: TWS (Tail When Spliced) indicates
                * whether the node is the last node in the sequence
                * being spliced onto the global queue.
                * - state[30:30]: SMW (Successor Must Wait) indicates
                * whether the successor thread must wait.
                * - state[29:00]: indicates to which cluster the thread belongs
                */
} hclh_qnode_t;

typedef struct hclh_lock_s {
    vatomicptr_t local_queues[HCLH_MAX_CLUSTERS]; /* tails of local queues, one
                                                     queue per cluster needed */
    vatomicptr_t global_queue; /* the tail of the global queue */
    hclh_qnode_t head;         /* inital node in the global queue */
} hclh_lock_t;

typedef struct hclh_tnode_s {
    hclh_qnode_t *pred;  /* predecessor's qnode */
    hclh_qnode_t *qnode; /* thread's qnode */
} hclh_tnode_t;

/**
 * Internal functions
 */
static inline void _hclhlock_qnode_hijack(hclh_tnode_t *, vuint32_t);

static inline vuint32_t _hclh_lock_get_default_state(vuint32_t);

static inline vbool_t _hclh_await_grant_or_cluster_master(vatomic32_t *,
                                                          vuint32_t);

/**
 * Initializes the lock.
 *
 * @param hclh_lock address of hclh_lock object to be initialized.
 */
static inline void
hclhlock_init(hclh_lock_t *hclh_lock)
{
    vsize_t i = 0;

    vatomic32_write_rlx(&hclh_lock->head.state, 0);
    vatomicptr_write_rlx(&hclh_lock->global_queue, &hclh_lock->head);
    for (i = 0; i < HCLH_MAX_CLUSTERS; i++) {
        vatomicptr_write_rlx(&hclh_lock->local_queues[i], NULL);
    }
}
/**
 * Initializes the given `tnode`.
 *
 * @param tnode address of the hclh_tnode_t object associated with the calling
 * thread.
 * @param qnode address of the hclh_qnode_t object associated with the calling
 * thread.
 * @param cluster the cluster to which the  calling thread belongs.
 *
 * @note Must be called once before the first call of `hclhlock_acquire`.
 * @note `qnode` must live as long as the system is running and not be freed.
 * The reason is that on release qnodes get passed to successor threads.
 * @note `tnode` should live as long as the associated thread lives.
 */
static inline void
hclhlock_init_tnode(hclh_tnode_t *tnode, hclh_qnode_t *qnode, vuint32_t cluster)
{
    vuint32_t state = 0;

    ASSERT(tnode);
    ASSERT(qnode);
    ASSERT(cluster < HCLH_MAX_CLUSTERS);

    state = _hclh_lock_get_default_state(cluster);
    vatomic32_write_rlx(&qnode->state, state);
    // thread's qnode  must point to the given node
    tnode->qnode = qnode;
    tnode->pred  = NULL;
}
/**
 * Acquires the lock.
 *
 * @param lock address of the lock to be used.
 * @param tnode address of the hclh_tnode_t object associated with the calling
 * thread.
 *
 * @note `hclhlock_init_tnode` must be called on the given `tnode`, before
 * `hclhlock_acquire` is called for the first time.
 */
static inline void
hclhlock_acquire(hclh_lock_t *lock, hclh_tnode_t *tnode)
{
    vatomicptr_t *local_queue = NULL;
    hclh_qnode_t *pred        = NULL;
    hclh_qnode_t *local_tail  = NULL;
    hclh_qnode_t *exp_tail    = NULL;
    vuint32_t state           = 0;
    vuint32_t cluster         = 0;
    vuint32_t succ_state      = 0;
    vbool_t granted           = false;

    ASSERT(lock);
    ASSERT(tnode);

    state       = vatomic32_read_rlx(&tnode->qnode->state);
    cluster     = HCLH_GET_CLUSTER(state);
    local_queue = &lock->local_queues[cluster];

    // splice my qnode into my cluster's local queue
    do {
        exp_tail = pred;
        pred     = vatomicptr_cmpxchg(local_queue, exp_tail, tnode->qnode);
    } while (pred != exp_tail);

    // if I have a predecessor I will wait till its done or I am the cluster
    // master
    if (pred != NULL) {
        granted = _hclh_await_grant_or_cluster_master(&pred->state, cluster);
        if (granted) {
            tnode->pred = pred;
            /* I have been granted the permission to enter the CS */
            return;
        }
    }

    /*
     * I am the head of the queue, I am the cluster master and I am responsible
     * for splicing the local queue onto the global queue
     * splice my qnode into the global queue
     */
    pred = NULL;
    do {
        local_tail = vatomicptr_read_acq(local_queue);
        exp_tail   = pred;
        pred = vatomicptr_cmpxchg(&lock->global_queue, exp_tail, local_tail);
    } while (pred != exp_tail);

    // inform successor it is the new master (the head of the local queue)
    succ_state = vatomic32_read_rlx(&local_tail->state);
    // set tail when spliced to true
    HCLH_SET_TWS_TRUE(succ_state);
    vatomic32_write_rlx(&local_tail->state, succ_state);

    /* as long as successor must wait in our pred state is set, we wait */
    state = vatomic32_read_acq(&pred->state);
    while (HCLH_IS_SMW(state)) {
        /* wait for the state to change */
        state = vatomic32_await_neq_acq(&pred->state, state);
    }
    tnode->pred = pred;
}
/**
 * Releases the lock.
 *
 * @param tnode address of the hclh_tnode_t object associated with the calling
 * thread.
 */
static inline void
hclhlock_release(hclh_tnode_t *tnode)
{
    vatomic32_t *state  = NULL;
    vuint32_t cur_state = 0;
    vuint32_t cluster   = 0;

    ASSERT(tnode);
    ASSERT(tnode->qnode);

    state = &tnode->qnode->state;

    cur_state = vatomic32_read_rlx(state);

    // extract cluster
    cluster = HCLH_GET_CLUSTER(cur_state);

    // set successor must wait to false
    HCLH_SET_SMW_FALSE(cur_state);

    vatomic32_write_rel(state, cur_state);

    // Rest the state and take the qnode from pred
    _hclhlock_qnode_hijack(tnode, cluster);
}

/**
 * Reinitializes the given state.
 *
 * Sets the cluster id and resets the flags
 * SMW and TWS.
 *
 * @param state address of the state to reinitialize
 * @param cluster cluster of the calling thread.
 */
static inline void
_hclhlock_qnode_hijack(hclh_tnode_t *tnode, vuint32_t cluster)
{
    vuint32_t cur_state = 0; /* current state */
    vuint32_t exp_state = 0; /* expected state */
    vuint32_t new_state = 0; /* new state */
    vatomic32_t *state  = NULL;

    ASSERT(tnode);
    ASSERT(tnode->pred);
    ASSERT(cluster < HCLH_CLUSTER_MASK);

    state = &tnode->pred->state;

    new_state = _hclh_lock_get_default_state(cluster);

    cur_state = vatomic32_read_rlx(state);
    do {
        exp_state = cur_state;
        cur_state = vatomic32_cmpxchg_rlx(state, exp_state, new_state);
    } while (cur_state != exp_state);

    /*
     * NOTE: GenMC 0.9 has a bug that does not consider the write even of the
     * successful cas on hclhlock.c client when REACQUIRE is enabled A
     * workaround would be to trick it with adding:
     *          vatomic32_write_rlx(state, new_state);
     * Right now the REACQUIRE is disabled, and should be re-enabled
     * after GenMC is fixed.
     */
    tnode->qnode = tnode->pred;
}

/**
 * Waits on the given state.
 *
 *
 * A blocking function that waits till either of the following is met:
 *	- the predecessor node is from the same cluster and TWS=SMW=0,
 *	- the predecessor node is from a different cluster and TWS=1.
 *
 * @param state address of the predecessor's state.
 * @param state state of the calling thread.
 * @return true pred is from same cluster, SMW=false & TWS=false.
 * @return false pred is not from the same cluster, or TWS=true.
 */
static inline vbool_t
_hclh_await_grant_or_cluster_master(vatomic32_t *state, vuint32_t cluster)
{
    vuint32_t cur_state      = 0; /* curr state */
    vuint32_t required_state = 0;

    ASSERT(state);
    ASSERT(cluster < HCLH_CLUSTER_MASK);
    /**
     * Predecessor state
     * Predecessor is in the same cluster and
     * It Successor must wait is set to false
     * and tail when spliced is set to false
     */
    required_state = _hclh_lock_get_default_state(cluster);
    HCLH_SET_SMW_FALSE(required_state);

    cur_state = vatomic32_read_acq(state);

    while (true) {
        // check same cluster, SMW=TWS=0
        if (cur_state == required_state) {
            return true;
        }
        // check diff cluster or TWS=1
        if (HCLH_GET_CLUSTER(cur_state) != cluster || HCLH_IS_TWS(cur_state)) {
            return false;
        }
        // wait for a new update to occur.
        cur_state = vatomic32_await_neq_acq(state, cur_state);
    }
}
/**
 * Returns default state.
 *
 * @param cluster cluster of calling thread.
 * @return vuint32_t default state value.
 */
static inline vuint32_t
_hclh_lock_get_default_state(vuint32_t cluster)
{
    vuint32_t state = 0;

    ASSERT(cluster < HCLH_CLUSTER_MASK);
    /* set cluster */
    HCLH_SET_CLUSTER(state, cluster);
    /* set successor must wait to one */
    HCLH_SET_SMW_TRUE(state);
    /* set tail when spliced to false */
    HCLH_SET_TWS_FALSE(state);
    return state;
}

#undef HCLH_TWS_MASK
#undef HCLH_SET_TWS_TRUE
#undef HCLH_SET_TWS_FALSE
#undef HCLH_IS_TWS
#undef HCLH_SMW_MASK
#undef HCLH_SET_SMW_TRUE
#undef HCLH_SET_SMW_FALSE
#undef HCLH_IS_SMW
#undef HCLH_CLUSTER_MASK
#undef HCLH_SET_CLUSTER
#undef HCLH_GET_CLUSTER
#endif
