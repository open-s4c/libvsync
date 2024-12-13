/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_HMCSLOCK_H
#define VSYNC_HMCSLOCK_H
/******************************************************************************
 * @file hmcslock.h
 * @ingroup fair_lock numa_aware
 * @brief Hierarchical MCS lock for systems with NUMA Hierarchies.
 *
 * @example
 * @include eg_hmcslock.c
 *
 * @cite
 * [High Performance Locks for Multi-level NUMA Systems]
 * (https://dl.acm.org/doi/pdf/10.1145/2858788.2688503)
 *
 * @cite
 * [Verifying and Optimizing the HMCS Lock for Arm Servers]
 * (https://link.springer.com/chapter/10.1007/978-3-030-91014-3_17)
 *****************************************************************************/
#include <vsync/vtypes.h>
#include <vsync/atomic.h>
#include <vsync/common/dbg.h>
#include <vsync/common/cache.h>

/******************************************************************************
 *	MACROS/CONSTANTS
 *****************************************************************************/
/** @cond DO_NOT_DOCUMENT */

#define HMCLOCK_UNLOCKED       0x0UL
#define HMCLOCK_LOCKED         0x1UL
#define HMCLOCK_COHORT_START   0x1UL
#define HMCLOCK_MAX            UINT64_MAX
#define HMCLOCK_ACQUIRE_PARENT (HMCLOCK_MAX - 1)
#define HMCLOCK_WAIT           HMCLOCK_MAX

#if defined(NDEBUG)
    /* on release mode make all assertions void */
    #define HMCSLOCK_ASSERT(_c_)
#else
    #define HMCS_ENABLE_DEBUG
    #include <vsync/common/assert.h>
    #define HMCSLOCK_ASSERT(_c_) ASSERT(_c_)
#endif

/** @endcond */

/******************************************************************************
 *	Data Types
 *****************************************************************************/
typedef struct hmcs_node_s {
    vatomicptr(struct hmcs_node_s *) next;
    vatomic64_t status;
} VSYNC_CACHEALIGN hmcs_node_t;

typedef struct hmcslock_s {
    vatomicptr(hmcs_node_t *) lock;
    struct hmcslock_s *parent;
    hmcs_node_t qnode;
    vuint32_t threshold;
    vbool_t is_leaf;
} VSYNC_CACHEALIGN hmcslock_t;

typedef struct hmcs_level_spec_s {
    vsize_t num_nodes_per_parent;
    vuint32_t threshold;
} hmcslock_level_spec_t;
/* *****************************************************************************
 *	Private functions
 * ****************************************************************************/
static inline void _hmcslock_release_real(hmcslock_t *lock, hmcs_node_t *qnode,
                                          vsize_t depth);

static inline void _hmcslock_acquire_real(hmcslock_t *lock, hmcs_node_t *qnode,
                                          vsize_t depth);

static inline void _hmcslock_release_helper(hmcslock_t *lock,
                                            hmcs_node_t *qnode, vuint64_t val);

static inline void _hmcslock_one_acquire_real(hmcslock_t *lock,
                                              hmcs_node_t *qnode);

static inline void _hmcslock_init_level(hmcslock_t *cur_parent,
                                        hmcslock_t *locks, vsize_t *i,
                                        vsize_t cur_level,
                                        hmcslock_level_spec_t *level_specs,
                                        vsize_t num_levels);

#if defined(HMCS_ENABLE_DEBUG)
static inline vsize_t _hmcslock_count_nodes(hmcslock_level_spec_t *level_specs,
                                            vsize_t num_levels);
#endif
/**
 * Returns the address of the lock associated with the core id.
 *
 * @param locks array of hmcslock_t objects.
 * @param locks_len `locks` array length.
 * @param level_specs array of hmcslock_level_spec_t objects.
 * @param num_levels number of the levels in hierarchy, same as the length of
 * `level_specs`.
 * @param cores_per_node number of cores per leaf lock node.
 * @param core_id core id of the calling thread.
 *
 * @return hmcslock_t* address of hmcslock_t object, which the calling
 * thread should use to acquire the lock.
 */
static inline hmcslock_t *
hmcslock_which_lock(hmcslock_t *locks, vsize_t locks_len,
                    hmcslock_level_spec_t *level_specs, vsize_t num_levels,
                    vuint32_t cores_per_node, vuint32_t core_id)
{
    vsize_t l               = 0;
    vsize_t i               = 0;
    vsize_t j               = 0;
    vsize_t leaf_node_count = 1;
    vsize_t node_index      = 0;

    for (l = 0; l < num_levels; l++) {
        leaf_node_count *= level_specs[l].num_nodes_per_parent;
    }

#ifdef __cplusplus
    hmcslock_t *leaf_nodes[HMCS_MAX_THREADS];
#else
    hmcslock_t *leaf_nodes[leaf_node_count];
#endif

    for (i = 0, j = 0; i < locks_len; i++) {
        HMCSLOCK_ASSERT(j < leaf_node_count &&
                        "you got over the boundary of leaf nodes");
        // to map the thread to its leaf parent
        if (locks[i].is_leaf) {
            leaf_nodes[j++] = &locks[i];
        }
    } // for
    node_index = (core_id / cores_per_node) % leaf_node_count;
    return leaf_nodes[node_index];
}
/**
 * Initializes the locks array.
 *
 * @param locks array of hmcslock_t objects.
 * @param locks_len `locks` array length.
 * @param level_specs array of hmcslock_level_spec_t objects.
 * @param num_levels number of the levels in heirarchy, same as the length of
 * `level_specs`.
 *
 * @note
 *	e.g., say you have three levels, the machine, two NUMA nodes, and two caches
 *  per NUMA. Then `locks_len = 1(machine) + 2(NUMA) + 4 (2*2 caches) = 7`.
 *	`num_levels = 3` (including the machine level)
 *  define levels_spec as follows:
 * ```c
 *	 level_specs[num_levels] = {
 *	 	{1, LEVEL_THRESHOLD}, // 1 machine
 *	 	{2, LEVEL_THRESHOLD}, // 2 NUMAs per machine
 *	 	{2, LEVEL_THRESHOLD}, // 2 caches per NUMA
 *	 }
 * ```
 */
static inline void
hmcslock_init(hmcslock_t *locks, vsize_t locks_len,
              hmcslock_level_spec_t *level_specs, vsize_t num_levels)
{
    vsize_t level = 0;

    HMCSLOCK_ASSERT(locks);
    HMCSLOCK_ASSERT(locks_len > 0);
    HMCSLOCK_ASSERT(num_levels > 0);

#if defined(HMCS_ENABLE_DEBUG)
    vsize_t total_nodes = 0;

    HMCSLOCK_ASSERT(level_specs[0].num_nodes_per_parent == 1 &&
                    "top level should have one node");

    total_nodes = _hmcslock_count_nodes(level_specs, num_levels);

    HMCSLOCK_ASSERT(locks_len >= total_nodes &&
                    "insufficient nodes, or too many nodes");
#else
    V_UNUSED(locks_len);
#endif

    _hmcslock_init_level(NULL, locks, &level, 0, level_specs, num_levels);

    HMCSLOCK_ASSERT(level == locks_len &&
                    "recursion did not reach the expected depth");
}
/**
 * Acquires the given lock.
 *
 * @param lock address of hmcslock_t object.
 * @param qnode address of hmcs_node_t object.
 * @param num_levels number of levels including machine level.
 *
 * @note `lock` should be what `hmcslock_which_lock` returned.
 */
static inline void
hmcslock_acquire(hmcslock_t *lock, hmcs_node_t *qnode, vsize_t num_levels)
{
    HMCSLOCK_ASSERT(lock);
    HMCSLOCK_ASSERT(qnode);
    HMCSLOCK_ASSERT(num_levels > 0);
    _hmcslock_acquire_real(lock, qnode, num_levels - 1);
}
/**
 * Releases the given lock.
 *
 * @param lock address of hmcslock_t object.
 * @param qnode address of hmcs_node_t object.
 * @param num_levels number of levels including machine level.
 *
 * @note `lock` should be what `hmcslock_which_lock` returned.
 */
static inline void
hmcslock_release(hmcslock_t *lock, hmcs_node_t *qnode, vsize_t num_levels)
{
    HMCSLOCK_ASSERT(lock);
    HMCSLOCK_ASSERT(qnode);
    HMCSLOCK_ASSERT(num_levels > 0);
    _hmcslock_release_real(lock, qnode, num_levels - 1);
}
/**
 * Helps releasing the lock to the successor.
 *
 * @param lock address of hmcslock_t object.
 * @param qnode address of hmcs_node_t object.
 * @param val the new status of the successor.
 */
static void
_hmcslock_release_helper(hmcslock_t *lock, hmcs_node_t *qnode, vuint64_t val)
{
    hmcs_node_t *succ = (hmcs_node_t *)vatomicptr_read_acq(&qnode->next);

    if (succ == NULL) {
        if (vatomicptr_cmpxchg(&lock->lock, qnode, NULL) == qnode) {
            return;
        }
        // await till there is a successor
        succ = (hmcs_node_t *)vatomicptr_await_neq_rlx(&qnode->next, NULL);
    }
    // update successor status
    vatomic64_write_rel(&succ->status, val);
}
/**
 * Releases the lock on depth zero.
 *
 * @param lock address of hmcslock_t object.
 * @param qnode address of hmcs_node_t object.
 */
static inline void
_hmcslock_one_acquire_real(hmcslock_t *lock, hmcs_node_t *qnode)
{
    hmcs_node_t *pred = NULL;

    // Prepare the node for use.
    vatomic64_write_rlx(&qnode->status, HMCLOCK_LOCKED);
    vatomicptr_write_rlx(&qnode->next, NULL);
    pred = (hmcs_node_t *)vatomicptr_xchg(&lock->lock, qnode);

    if (pred) {
        // spin as long as it's locked
        vatomicptr_write_rel(&pred->next, qnode);
        vatomic64_await_neq_acq(&qnode->status, HMCLOCK_LOCKED);
    } else {
        vatomic64_write_rlx(&qnode->status, HMCLOCK_UNLOCKED);
    }
}
/**
 * Acquires the lock on `depth > 1`.
 *
 * Recursive function.
 *
 * @param lock address of hmcslock_t object.
 * @param qnode address of hmcs_node_t object.
 * @param depth current recursion depth.
 */
static inline void
_hmcslock_acquire_real(hmcslock_t *lock, hmcs_node_t *qnode, vsize_t depth)
{
    hmcs_node_t *pred    = NULL;
    vuint64_t cur_status = 0;

    // Prepare the node for use
    vatomic64_write_rlx(&qnode->status, HMCLOCK_WAIT);
    vatomicptr_write_rlx(&qnode->next, NULL);
    pred = (hmcs_node_t *)vatomicptr_xchg(&lock->lock, qnode);

    if (pred) {
        vatomicptr_write_rel(&pred->next, qnode);
        // spin as long as the status is wait
        cur_status = vatomic64_await_neq_acq(&qnode->status, HMCLOCK_WAIT);

        // acquired, enter the CS
        if (cur_status < HMCLOCK_ACQUIRE_PARENT) {
            return;
        }
    }

    vatomic64_write_rlx(&qnode->status, HMCLOCK_COHORT_START);

    HMCSLOCK_ASSERT(lock->parent && "parent is null");

    if (depth == 1) {
        _hmcslock_one_acquire_real(lock->parent, &lock->qnode);
    } else {
        _hmcslock_acquire_real(lock->parent, &lock->qnode, depth - 1);
    }
}
/**
 * Releases the lock on `depth > 1`.
 *
 * Recursive function.
 *
 * @param lock address of hmcslock_t object.
 * @param qnode address of hmcs_node_t object.
 * @param depth current recursion depth.
 */
static inline void
_hmcslock_release_real(hmcslock_t *lock, hmcs_node_t *qnode, vsize_t depth)
{
    vuint64_t cur_count = 0;
    hmcs_node_t *succ   = NULL;

    cur_count = vatomic64_read_rlx(&qnode->status);
    HMCSLOCK_ASSERT(cur_count <= lock->threshold);

    // Lower level releases
    if (cur_count < lock->threshold) { // Not reached threshold

        succ = (hmcs_node_t *)vatomicptr_read_acq(&qnode->next);
        if (succ) { // pass within cohorts
            vatomic64_write_rel(&succ->status, cur_count + 1);
            return;
        } // if succ
    }     // if threshold not reached

    // else: No known successor or reached threshold, release to parent
    if (depth == 1) {
        _hmcslock_release_helper(lock->parent, &lock->qnode, HMCLOCK_UNLOCKED);
    } else {
        _hmcslock_release_real(lock->parent, &lock->qnode, depth - 1);
    }

    // Ask successor to acquire next level lock
    _hmcslock_release_helper(lock, qnode, HMCLOCK_ACQUIRE_PARENT);
}
/**
 * Builds and initializes the locks array/tree.
 *
 * @param cur_parent address of hmcslock_t object. The parent lock of
 * `cur_level`.
 * @param locks array of hmcslock_t objects.
 * @param idx in/out parameters, index of the current lock.
 * @param cur_level index of the current level.
 * @param level_specs array of hmcslock_level_spec_t objects.
 * @param num_levels number of levels including machine level.
 *
 * @note recursive function.
 */
static void
_hmcslock_init_level(hmcslock_t *cur_parent, hmcslock_t *locks, vsize_t *idx,
                     vsize_t cur_level, hmcslock_level_spec_t *level_specs,
                     vsize_t num_levels)
{
    vsize_t p             = 0;
    vsize_t cur_index     = 0;
    vbool_t is_leaf       = false;
    hmcslock_t *cur_child = NULL;

    if (num_levels == cur_level) {
        return;
    }

    if (cur_level + 1 == num_levels) {
        is_leaf = true;
    }

    for (p = 0; p < level_specs[cur_level].num_nodes_per_parent; p++) {
        cur_index            = (*idx)++;
        cur_child            = &locks[cur_index];
        cur_child->parent    = cur_parent;
        cur_child->threshold = level_specs[cur_level].threshold;
        cur_child->is_leaf   = is_leaf;
        _hmcslock_init_level(cur_child, locks, idx, cur_level + 1, level_specs,
                             num_levels);
        vatomicptr_write(&cur_child->lock, NULL);
    }
}
#if defined(HMCS_ENABLE_DEBUG)
/**
 * Returns the number of locks.
 *
 * @param level_specs array of hmcslock_level_spec_t objects.
 * @param num_levels number of levels including machine level.
 * @return vuint32_t
 *
 * @note recursive function
 */
static inline vsize_t
_hmcslock_count_nodes(hmcslock_level_spec_t *level_specs, vsize_t num_levels)
{
    vsize_t i     = 0;
    vsize_t nodes = 1;

    if (num_levels == 0) {
        return 0;
    }

    for (i = 0; i < num_levels; i++) {
        nodes *= level_specs[i].num_nodes_per_parent;
    }

    return nodes + _hmcslock_count_nodes(level_specs, num_levels - 1);
}
#endif

#undef HMCLOCK_UNLOCKED
#undef HMCLOCK_LOCKED
#undef HMCLOCK_COHORT_START
#undef HMCLOCK_MAX
#undef HMCLOCK_ACQUIRE_PARENT
#undef HMCLOCK_WAIT
#undef HMCSLOCK_ASSERT
#if defined(HMCS_ENABLE_DEBUG)
    #undef HMCS_ENABLE_DEBUG
#endif
#endif
