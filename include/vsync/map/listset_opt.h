/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VLISTSET_OPT_H
#define VLISTSET_OPT_H
/*******************************************************************************
 * @file  listset_opt.h
 * @ingroup requires_smr linearizable
 * @brief This implementation is an optimized verison of listset_fine.
 *
 * It uses fine grained locking (one lock per node) to serialize all operations.
 * This implementation allows for list traversal without acquiring locks.
 * Locks are only acquired to validate the traversal results and performing
 * the operations.
 *
 * @example
 * @include eg_listset_opt.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 9.6]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/
#include <vsync/vtypes.h>
#include <vsync/common/assert.h>
#include <vsync/atomic.h>
#include <vsync/map/internal/listset/listset_common.h>
#include <vsync/common/verify.h>
#include <vsync/utils/abstract_lock.h>
#include <vsync/spinlock/ttaslock.h>

DEF_ABSTRACT_LOCK(vlistset_lock, ttaslock_t, ttaslock_init, ttaslock_acquire,
                  ttaslock_release, ttaslock_tryacquire)

typedef struct vlistset_node_s {
    vlistset_lock_t lock;
    vatomicptr(struct vlistset_node_s *) next;
} vlistset_node_t;

typedef struct vlistset_s {
    vlistset_node_t head_sentinel;
    vlistset_node_t tail_sentinel;
    vlistset_handle_node_t retire_fun;
    void *retire_fun_arg;
    vlistset_cmp_key_t cmp_fun;
} vlistset_t;

static inline vbool_t _vlistset_validate(vlistset_t *lst, vlistset_node_t *pred,
                                         vlistset_node_t *curr,
                                         vlistset_key_t key);

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
    ASSERT(retire_fun);
    ASSERT(cmp_fun);

    vlistset_lock_init(&lst->head_sentinel.lock);
    vlistset_lock_init(&lst->tail_sentinel.lock);

    vatomicptr_write_rlx(&lst->head_sentinel.next, &lst->tail_sentinel);
    vatomicptr_write_rlx(&lst->tail_sentinel.next, NULL);

    lst->retire_fun     = retire_fun;
    lst->retire_fun_arg = retire_fun_arg;
    lst->cmp_fun        = cmp_fun;
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
    vbool_t stop          = false;

    ASSERT(lst);
    ASSERT(node);
    tail = &lst->tail_sentinel;

    while (!stop) {
        pred = &lst->head_sentinel;
        curr = vatomicptr_read(&pred->next);
        /* find the node without acquiring locks */
        while (curr != tail && lst->cmp_fun(curr, key) < 0) {
            pred = curr;
            curr = vatomicptr_read(&curr->next);
        } // while <= key

        vlistset_lock_acquire(&pred->lock);
        vlistset_lock_acquire(&curr->lock);
        /* validate your findings in the critical section */
        if (_vlistset_validate(lst, pred, curr, key)) {
            if (curr != tail && lst->cmp_fun(curr, key) == 0) {
                success = false;
            } else {
                vlistset_lock_init(&node->lock);
                vatomicptr_write(&node->next, curr);
                vatomicptr_write(&pred->next, node);
                success = true;
            }
            stop = true;
        } else {
            verification_ignore();
        }

        vlistset_lock_release(&pred->lock);
        vlistset_lock_release(&curr->lock);
    } // while

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
    vbool_t stop          = false;

    ASSERT(lst);
    tail = &lst->tail_sentinel;

    while (!stop) {
        pred = &lst->head_sentinel;
        curr = vatomicptr_read(&pred->next);
        /* find the node without acquiring locks */
        while (curr != tail && lst->cmp_fun(curr, key) < 0) {
            pred = curr;
            curr = vatomicptr_read(&curr->next);
        }
        vlistset_lock_acquire(&pred->lock);
        vlistset_lock_acquire(&curr->lock);

        /* validate your findings in the critical section */
        if (_vlistset_validate(lst, pred, curr, key)) {
            if (curr != tail && lst->cmp_fun(curr, key) == 0) {
                vatomicptr_write(&pred->next, vatomicptr_read(&curr->next));
                success = true;
            } else {
                success = false;
            }
            stop = true;
        } else {
            verification_ignore();
        }
        vlistset_lock_release(&pred->lock);
        vlistset_lock_release(&curr->lock);
    }
    if (success) {
        lst->retire_fun(curr, lst->retire_fun_arg);
    }
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
    vlistset_node_t *pred = NULL;
    vlistset_node_t *curr = NULL;
    vlistset_node_t *tail = NULL;
    vbool_t found         = false;
    vbool_t stop          = false;

    ASSERT(lst);
    tail = &lst->tail_sentinel;

    while (!stop) {
        pred = &lst->head_sentinel;
        curr = vatomicptr_read(&pred->next);

        /* find the node without acquiring locks */
        while (curr != tail && lst->cmp_fun(curr, key) < 0) {
            pred = curr;
            curr = vatomicptr_read(&curr->next);
        }

        vlistset_lock_acquire(&pred->lock);
        vlistset_lock_acquire(&curr->lock);

        /* validate your findings in the critical section */
        if (_vlistset_validate(lst, pred, curr, key)) {
            found = curr != tail && lst->cmp_fun(curr, key) == 0;
            stop  = true;
        } else {
            verification_ignore();
        }

        vlistset_lock_release(&pred->lock);
        vlistset_lock_release(&curr->lock);
    } // while

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

    curr = vatomicptr_read(&lst->head_sentinel.next);

    while (curr != &lst->tail_sentinel) {
        next = vatomicptr_read(&curr->next);
        visitor(curr, arg);
        curr = next;
    }
}
/**
 * Validates that `pred` is reachable and its `pred->next == curr`.
 *
 * @param lst address of vlistset_t object.
 * @param pred address of vlistset_node_t object.
 * @param curr address of vlistset_node_t object.
 * @param key key <= curr.key && key > pred.key.
 * @return true if `pred` is reachable and  `pred->next == curr`.
 * @return false otherwise.
 */
static inline vbool_t
_vlistset_validate(vlistset_t *lst, vlistset_node_t *pred,
                   vlistset_node_t *curr, vlistset_key_t key)
{
    vlistset_node_t *node = NULL;
    vlistset_node_t *tail = NULL;

    node = &lst->head_sentinel;
    if (pred == node) {
        /* if pred == head, it will not have a key to compare to */
        return vatomicptr_read(&pred->next) == curr ? true : false;
    }

    tail = &lst->tail_sentinel;
    /* move to head->next */
    node = vatomicptr_read(&node->next);

    /* find pred and confirm pred->next = curr */
    while (node != tail && lst->cmp_fun(node, key) < 0) {
        if (node == pred) {
            return vatomicptr_read(&pred->next) == curr ? true : false;
        }
        node = vatomicptr_read(&node->next);
    }
    return false;
}
#endif
