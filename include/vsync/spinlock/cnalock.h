/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_CNA_H
#define VSYNC_CNA_H
/*******************************************************************************
 * @file cnalock.h
 * @ingroup fair_lock numa_aware
 * @brief Compact NUMA-aware Lock.
 *
 * The CNA is an efficient variant of the MCS locks, which adds NUMA-awareness
 * without a hierarchical approach.
 *
 * @example
 * @include eg_cna.c
 *
 * @cite Dave Dice and Alex Kogan -
 * [Compact NUMA-aware locks](https://dl.acm.org/doi/10.1145/3302424.3303984)
 *
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/common/cache.h>
#include <vsync/vtypes.h>

#define CNALOCK_NODE_UNSET VUINT32_MAX

typedef struct cna_node_s {
    vatomicptr_t spin;
    vatomicptr(struct cna_node_s *) next;
    vatomic32_t node;
} VSYNC_CACHEALIGN cna_node_t;

typedef struct cnalock_s {
    vatomicptr(struct cna_node_s *) tail;
} cnalock_t;

/** Initializer of `cnalock_t`. */
#define CNALOCK_INIT()                                                         \
    {                                                                          \
        .tail = VATOMIC_INIT(0)                                                \
    }

#ifdef VSYNC_VERIFICATION
vatomic32_t rand = VATOMIC_INIT(0);
#endif

#ifndef INITIAL_VAL
    #define INITIAL_VAL (128 * 128)
#endif

/**
 * Initializes the CNA lock.
 *
 * @param lock address of cnalock_t object.
 * @note alternatively use CNALOCK_INIT
 */
static inline void
cnalock_init(cnalock_t *lock)
{
    vatomicptr_init(&lock->tail, NULL);
}
/**
 * Acquires the CNA lock.
 *
 * @param lock address of cnalock_t object.
 * @param me address of cna_node_t object associated with the calling thread.
 * @param numa_node valid id of the NUMA node where the calling thread is hosted
 */
static inline void
cnalock_acquire(cnalock_t *lock, cna_node_t *me, vuint32_t numa_node)
{
    vatomicptr_write_rlx(&me->spin, NULL);
    vatomicptr_write_rlx(&me->next, 0);
    vatomic32_write_rlx(&me->node, CNALOCK_NODE_UNSET);

    // Access to tail
    cna_node_t *tail = vatomicptr_xchg(&lock->tail, me);

    if (!tail) {
        vatomicptr_write_rlx(&me->spin, (void *)1);
        return;
    }

    vatomic32_write_rlx(&me->node, numa_node);

    // Access to another threads queue element -- next
    vatomicptr_write_rel(&tail->next, me);

    vatomicptr_await_neq_acq(&me->spin, NULL);
}
/**
 * Decides if the lock should be handed to the successor of the same NUMA.
 *
 * @return vuint32_t non-zero if yes.
 * @return 0 otherwise.
 */
static inline vuint32_t
_cnalock_keep_lock_local(void)
{
#ifdef VSYNC_VERIFICATION
    return vatomic32_read_rlx(&rand);
#else
    static vuint32_t cnt = 0;
    return (cnt = (cnt + 1) % INITIAL_VAL);
#endif
}
/**
 * Returns the successor of the given node `me`.
 *
 * @param me address of cna_node_t object.
 * @param numa_node id of the NUMA node where the calling thread is hosted.
 * @return cna_node_t* address of the successor.
 */
static inline cna_node_t *
_cnalock_find_successor(cna_node_t *me, vuint32_t numa_node)
{
    cna_node_t *next  = vatomicptr_read_rlx(&me->next);
    vuint32_t my_node = vatomic32_read_rlx(&me->node);

    if (my_node == CNALOCK_NODE_UNSET) {
        my_node = numa_node;
    }

    cna_node_t *sec_head = next;
    cna_node_t *sec_tail = next;

    cna_node_t *cur;

    // Access to another threads queue element -- node, next
    for (cur      = next; cur && vatomic32_read_rlx(&cur->node) != my_node;
         sec_tail = cur, cur = vatomicptr_read_acq(&cur->next)) {}

    if (!cur) {
        return NULL; // No one is in the same node as me
    }

    if (cur == next) {
        return next;
    }

    cna_node_t *spin = vatomicptr_read_rlx(&me->spin);

    if (spin > (cna_node_t *)1) {
        // There is someone in the secondary queue

        // xchg is not necessary, read and write are enough
        // This version was choosen to improve GenMC verification
        // Access to another threads queue element -- next
        cna_node_t *origSecHead = vatomicptr_xchg_rlx(&spin->next, sec_head);
        sec_head                = origSecHead;
    }
    // Access to another threads queue element -- next
    vatomicptr_write_rlx(&sec_tail->next, sec_head);
    vatomicptr_write_rlx(&me->spin, sec_tail);

    return cur;
}
/**
 * Releases the CNA lock.
 *
 * @param lock address of cnalock_t object.
 * @param me address of cna_node_t object associated with the calling thread.
 * @param numa_node valid id of the NUMA node where the calling thread is
 * hosted.
 */
static inline void
cnalock_release(cnalock_t *lock, cna_node_t *me, vuint32_t numa_node)
{
    cna_node_t *next = vatomicptr_read_acq(&me->next);
    cna_node_t *spin = vatomicptr_read_rlx(&me->spin);

    if (!next) {
        // There is no one in the main queue
        if (spin == (void *)1) {
            // There is no one in the secondary queue
            // Access to tail
            if (vatomicptr_cmpxchg_rel(&lock->tail, me, NULL) == me) {
                return;
            }
        } else {
            cna_node_t *sec_tail = spin;
            // Access to another threads queue element -- next
            cna_node_t *sec_head = vatomicptr_xchg_rlx(&sec_tail->next, NULL);

            // Access to tail
            if (vatomicptr_cmpxchg_rel(&lock->tail, me, sec_tail) == me) {
                // Access to another thread queue element -- spin
                vatomicptr_write_rel(&sec_head->spin, (void *)1);
                return;
            }
            // Access to another threads queue element -- spin
            vatomicptr_write_rlx(&sec_tail->next, sec_head);
        }
        next = vatomicptr_await_neq_acq(&me->next, NULL);
    }
    cna_node_t *succ = NULL;
    void *value      = (void *)1;

    vuint32_t keep_lock = _cnalock_keep_lock_local();

    if (keep_lock) {
        succ = _cnalock_find_successor(me, numa_node);
        spin = vatomicptr_read_rlx(&me->spin);
        // Spin could have changed due to _cnalock_find_successor, so re-read it
    }

    if (keep_lock && succ) {
        value = spin;
    } else if (spin > (cna_node_t *)1) {
        // xchg is not necessary, read and write are enough
        // This version was choosen to improve GenMC verification
        // Access to another threads queue element -- next
        succ = vatomicptr_xchg_rlx(&spin->next, next);
    } else {
        succ = next;
    }
    // Access to another threads queue element -- spin
    vatomicptr_write_rel(&succ->spin, value);
}
#endif
