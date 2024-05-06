/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_HEMLOCK_H
#define VSYNC_HEMLOCK_H
/*******************************************************************************
 * @file hemlock.h
 * @ingroup fair_lock
 * @brief Hemlock by Dice and Kogan.
 *
 * @example
 * @include eg_hemlock.c
 *
 * @cite
 *  Dice and Kogan - [Hemlock : Compact and Scalable Mutual Exclusion]
 * (https://arxiv.org/abs/2102.03863)
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/common/cache.h>

/** Node of a thread/core for all Hemlock instances. */
typedef struct hem_node_s {
    vatomicptr(struct hemlock_s *) grant;
} VSYNC_CACHEALIGN hem_node_t;

/** Hemlock data structure. */
typedef struct hemlock_s {
    vatomicptr(hem_node_t *) tail;
} VSYNC_CACHEALIGN hemlock_t;

/** Initializer of `hemlock_t`. */
#define HEMLOCK_INIT()                                                         \
    {                                                                          \
        .tail = VATOMIC_INIT(0)                                                \
    }

/**
 * Initializes the given lock object `l`.
 *
 * @param l address of hemlock_t object.
 */
static inline void
hemlock_init(hemlock_t *l)
{
    vatomicptr_write(&l->tail, NULL);
}
/**
 * Tries to acquire the Hemlock.
 *
 * @param l address of hemlock_t object.
 * @param node address of hem_node_t object. Associated with the calling
 * thread/core.
 * @return 1 on success, 0 on failure
 */
static inline int
hemlock_tryacquire(hemlock_t *l, hem_node_t *node)
{
    hem_node_t *pred = NULL;

    vatomicptr_write_rlx(&node->grant, NULL);
    pred = (hem_node_t *)vatomicptr_cmpxchg_acq(&l->tail, NULL, node);
    return pred == NULL;
}
/**
 * Acquires the Hemlock.
 *
 * @param l address of hemlock_t object.
 * @param node address of hem_node_t object. Associated with the calling
 * thread/core.
 */
static inline void
hemlock_acquire(hemlock_t *l, hem_node_t *node)
{
    hem_node_t *pred = NULL;

    vatomicptr_write_rlx(&node->grant, NULL);
    pred = (hem_node_t *)vatomicptr_xchg(&l->tail, node);
    if (pred == NULL) {
        return;
    }

#if !defined(HEMLOCK_USE_CTR)
    vatomicptr_await_eq_acq(&pred->grant, l);
    vatomicptr_write_rlx(&pred->grant, NULL);
#else
    await_while (vatomicptr_cmpxchg_acq(&pred->grant, l, NULL) != l)
        vatomic_cpu_pause();
#endif
}
/**
 * Releases the Hemlock.
 *
 * @param l address of hemlock_t object.
 * @param node address of hem_node_t object. Associated with the calling
 * thread/core.
 */
static inline void
hemlock_release(hemlock_t *l, hem_node_t *node)
{
    if (vatomicptr_read_rlx(&l->tail) == node &&
        vatomicptr_cmpxchg_rel(&l->tail, node, NULL) == node) {
        return;
    }
    vatomicptr_write_rel(&node->grant, l);

#if !defined(HEMLOCK_USE_CTR)
    vatomicptr_await_eq_rlx(&node->grant, NULL);
#elif __SIZEOF_POINTER__ == 8
    vatomic64_t *grant = (vatomic64_t *)&node->grant;
    await_while (vatomic64_get_add_rlx(grant, 0) != (vuint64_t)NULL)
        vatomic_cpu_pause();
#else
    vatomic32_t *grant = (vatomic32_t *)&node->grant;
    await_while (vatomic32_get_add_rlx(&node->grant, 0) != (vuint32_t)NULL)
        vatomic_cpu_pause();
#endif
}
/**
 * Returns whether there is a thread waiting to acquire the Hemlock.
 *
 * This function should only be called by the current owner of the lock.
 *
 * @param l address of hemlock_t object.
 * @param node address of hem_node_t object. Associated with the calling
 * thread/core.
 */
static inline int
hemlock_has_waiters(hemlock_t *l, hem_node_t *node)
{
    return vatomicptr_read_rlx(&l->tail) != node;
}

#endif
