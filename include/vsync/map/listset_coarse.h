/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VLISTSET_COARSE_H
#define VLISTSET_COARSE_H
/*******************************************************************************
 * @file  listset_coarse.h
 * @ingroup requires_smr linearizable
 * @brief This implementation of listset uses one lock to serialize all
 * operations
 *
 * @example
 * @include eg_listset_coarse.c
 *
 * @note SMR usage is required only in the scenario where users access the
 * node returned by `vlistset_get`, which happens without lock protection.
 * The node might then get deleted by a parallel `vlistset_remove`
 * operation. If the user does not wish to access the node itself, and uses
 * `vlistset_get` only to check if a node exists, then SMR is not necessary as
 * long as no dereferencing of the returned address occurs.
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 9.4]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/
#include <vsync/vtypes.h>
#include <vsync/common/assert.h>
#include <vsync/common/macros.h>
#include <vsync/map/internal/listset/listset_common.h>
#include <vsync/utils/abstract_lock.h>
#include <vsync/spinlock/ttaslock.h>

/** wraps the lock in use */
DEF_ABSTRACT_LOCK(vlistset_lock, ttaslock_t, ttaslock_init, ttaslock_acquire,
                  ttaslock_release, ttaslock_tryacquire)

typedef struct vlistset_node_s {
    struct vlistset_node_s *next;
} vlistset_node_t;

typedef struct vlistset_s {
    vlistset_node_t head_sentinel;
    vlistset_node_t tail_sentinel;
    vlistset_handle_node_t retire_fun;
    void *retire_fun_arg;
    vlistset_cmp_key_t cmp_fun;
    vlistset_lock_t lock;
} vlistset_t;

/**
 * Initializes the given vlistset_t object `lst`.
 *
 * @note must be called before threads access the listset.
 * @param lst address of vlistset_t object.
 * @param retire_fun address of the function used to retire removed nodes.
 * @param retire_fun_arg optional second parameter to pass to `retire_fun`.
 * @param cmp_fun address of the function used for comparing a key in
 * `vlistset_node_t` object with a given key.
 */
static inline void
vlistset_init(vlistset_t *lst, vlistset_handle_node_t retire_fun,
              void *retire_fun_arg, vlistset_cmp_key_t cmp_fun)
{
    ASSERT(lst);
    ASSERT(cmp_fun);
    ASSERT(retire_fun);
    lst->head_sentinel.next = &lst->tail_sentinel;
    lst->retire_fun         = retire_fun;
    lst->retire_fun_arg     = retire_fun_arg;
    lst->cmp_fun            = cmp_fun;
    vlistset_lock_init(&lst->lock);
}
/**
 * Destroys all the remaining nodes in the listset.
 *
 * @note call only after thread join, or after all threads finished accessing
 * the given vlistset_t object.
 * @param lst address of vlistset_t object.
 */
static inline void
vlistset_destroy(vlistset_t *lst)
{
    ASSERT(lst);
    _vlistset_visit(lst, lst->retire_fun, lst->retire_fun_arg, true);
}
/**
 * Inserts the given node into the listset.
 *
 * The node is only inserted if there is no other node associated with the given
 * key exists in the listset.
 *
 * @note if the operation fails, users can safely free or reuse the given node.
 * @pre the key must be set in the given `node` before calling add.
 * @param lst address of vlistset_t object.
 * @param key the key value that is used to identify the node.
 * @param node address of vlistset_node_t object.
 * @return true operation succeeded, the given node has been added.
 * @return false operation failed, the given node has not been added, because
 * there exists another node associated with the given key.
 * @note must be called inside SMR critical section.
 */
static inline vbool_t
vlistset_add(vlistset_t *lst, vlistset_key_t key, vlistset_node_t *node)
{
    vlistset_node_t *pred = NULL;
    vlistset_node_t *curr = NULL;
    vlistset_node_t *tail = NULL;
    vbool_t success       = false;

    ASSERT(lst);
    ASSERT(node);

    vlistset_lock_acquire(&lst->lock);

    tail = &lst->tail_sentinel;
    pred = &lst->head_sentinel;
    curr = pred->next;

    while (curr != tail && lst->cmp_fun(curr, key) < 0) {
        pred = curr;
        curr = curr->next;
    }

    if (curr != tail && lst->cmp_fun(curr, key) == 0) {
        // there is already an item with such a key added.
        success = false;
    } else {
        node->next = curr;
        pred->next = node;
        success    = true;
    }

    vlistset_lock_release(&lst->lock);
    return success;
}
/**
 * Removes the node associated with the given key from the listset.
 *
 * @note the registered retire callback will be called on the removed node.
 * @param lst address of vlistset_t object.
 * @param key the key identifying the node to remove.
 * @return true the node associated with the given key was removed successfully.
 * @return false could not find a node holding the given key to remove.
 * @note must be called inside SMR critical section.
 */
static inline vbool_t
vlistset_remove(vlistset_t *lst, vlistset_key_t key)
{
    vlistset_node_t *pred = NULL;
    vlistset_node_t *curr = NULL;
    vlistset_node_t *tail = NULL;
    vbool_t success       = false;

    ASSERT(lst);

    vlistset_lock_acquire(&lst->lock);
    tail = &lst->tail_sentinel;
    pred = &lst->head_sentinel;
    curr = pred->next;

    while (curr != tail && lst->cmp_fun(curr, key) < 0) {
        pred = curr;
        curr = curr->next;
    }
    if (curr != tail && lst->cmp_fun(curr, key) == 0) {
        pred->next = curr->next;
        lst->retire_fun(curr, lst->retire_fun_arg);
        success = true;
    }
    vlistset_lock_release(&lst->lock);
    return success;
}
/**
 * Looks for the listset node associated with the given key.
 *
 * @param lst address of vlistset_t object.
 * @param key key value.
 * @return vlistset_node_t* address of the listset node associated with the
 * given key.
 * @return NULL if no node associated with the given key was found.
 * @note must be called inside SMR critical section.
 */
static inline vlistset_node_t *
vlistset_get(vlistset_t *lst, vlistset_key_t key)
{
    vlistset_node_t *tail = NULL;
    vlistset_node_t *curr = NULL;
    vbool_t found         = false;

    ASSERT(lst);

    vlistset_lock_acquire(&lst->lock);
    tail = &lst->tail_sentinel;
    curr = lst->head_sentinel.next;

    while (curr != tail && lst->cmp_fun(curr, key) < 0) {
        curr = curr->next;
    }

    if (curr != tail && lst->cmp_fun(curr, key) == 0) {
        found = true;
    }
    vlistset_lock_release(&lst->lock);
    return found ? curr : NULL;
}
/**
 * Visits all nodes in the list and calls `visitor` on them.
 *
 * @note call only after thread join, or after all threads finished accessing
 * the given vlistset_t object.
 * @param lst address of vlistset_t object.
 * @param visitor address of the function to call on each node.
 * @param arg second argument of visitor.
 * @param visitor_destructive
 *      true: visitor is destructive.
 *      false: visitor is not destructive.
 */
static inline void
_vlistset_visit(vlistset_t *lst, vlistset_handle_node_t visitor, void *arg,
                vbool_t visitor_destructive)
{
    vlistset_node_t *next = NULL;
    vlistset_node_t *curr = NULL;
    V_UNUSED(visitor_destructive);

    ASSERT(lst);
    ASSERT(visitor);

    curr = lst->head_sentinel.next;

    while (curr != &lst->tail_sentinel) {
        next = curr->next;
        visitor(curr, arg);
        curr = next;
    }
}
#endif
