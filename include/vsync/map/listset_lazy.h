/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VLISTSET_LAZY_H
#define VLISTSET_LAZY_H
/*******************************************************************************
 * @file  listset_lazy.h
 * @ingroup requires_smr linearizable
 * @brief This is a partially parallel implementation with lock-free get.
 *
 * `vlistset_get` method is lock-free, while `vlistset_add` and
 * `vlistset_remove` are serialized.
 *
 * @example
 * @include eg_listset_lazy.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 9.7]
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
    vatomic32_t marked;
    vatomicptr(struct vlistset_node_s *) next;
} vlistset_node_t;

typedef struct vlistset_s {
    vlistset_node_t head_sentinel;
    vlistset_node_t tail_sentinel;
    vlistset_handle_node_t retire_fun;
    vlistset_cmp_key_t cmp_fun;
    void *retire_fun_arg;
} vlistset_t;

static inline vbool_t _vlistset_validate(vlistset_node_t *pred,
                                         vlistset_node_t *curr);

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

    vatomic32_write_rlx(&lst->head_sentinel.marked, 0);
    vatomic32_write_rlx(&lst->tail_sentinel.marked, 0);

    // attach the tail
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

        while (curr != tail && lst->cmp_fun(curr, key) < 0) {
            pred = curr;
            curr = vatomicptr_read(&curr->next);
        }

        vlistset_lock_acquire(&pred->lock);
        vlistset_lock_acquire(&curr->lock);

        if (_vlistset_validate(pred, curr)) {
            if (curr != tail && lst->cmp_fun(curr, key) == 0) {
                success = false;
            } else {
                // prepare node
                ASSERT(node);

                vlistset_lock_init(&node->lock);
                vatomic32_write(&node->marked, 0);

                /* insert the new node between pred and curr */
                vatomicptr_write(&node->next, curr);
                vatomicptr_write(&pred->next, node);
                success = true;
            } // else key does not exit

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
    vlistset_node_t *succ = NULL;
    vlistset_node_t *tail = NULL;
    vbool_t success       = false;
    vbool_t stop          = false;

    ASSERT(lst);
    tail = &lst->tail_sentinel;

    while (!stop) {
        pred = &lst->head_sentinel;
        curr = vatomicptr_read(&pred->next);

        /* find a node with a key >= to key */
        while (curr != tail && lst->cmp_fun(curr, key) < 0) {
            pred = curr;
            curr = vatomicptr_read(&curr->next);
        }

        vlistset_lock_acquire(&pred->lock);
        vlistset_lock_acquire(&curr->lock);

        /* make sure the data is consistent */
        if (_vlistset_validate(pred, curr)) {
            if (curr != tail && lst->cmp_fun(curr, key) == 0) {
                /* logically remove curr */
                vatomic32_write(&curr->marked, 1);
                succ = vatomicptr_read(&curr->next);
                /* detach curr */
                vatomicptr_write(&pred->next, succ);
                /* retire curr */
                lst->retire_fun(curr, lst->retire_fun_arg);
                /* remove succeeded */
                success = true;
            } else {
                /* curr->key > key */
                success = false;
            }
            /* we reached a conclusion */
            stop = true;
        } else {
            verification_ignore();
        }

        vlistset_lock_release(&pred->lock);
        vlistset_lock_release(&curr->lock);
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
    vlistset_node_t *curr = NULL;
    vlistset_node_t *tail = NULL;
    vbool_t found         = false;

    tail = &lst->tail_sentinel;
    curr = vatomicptr_read(&lst->head_sentinel.next);

    while (curr != tail && lst->cmp_fun(curr, key) < 0) {
        curr = vatomicptr_read(&curr->next);
    }

    found = curr != tail && lst->cmp_fun(curr, key) == 0 &&
            !vatomic32_read(&curr->marked);

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

    ASSERT(lst);
    ASSERT(visitor);

    curr = vatomicptr_read(&lst->head_sentinel.next);

    while (curr != &lst->tail_sentinel) {
        next = vatomicptr_read(&curr->next);

        if (visitor_destructive || !vatomic32_read(&curr->marked)) {
            visitor(curr, arg);
        }

        curr = next;
    }
}
/**
 * Validates that neither pred nor curr has been deleted and
 * pred->next == curr
 *
 * @param pred
 * @param curr
 * @return true assumptions hold
 * @return false assumptions are violated
 */
static inline vbool_t
_vlistset_validate(vlistset_node_t *pred, vlistset_node_t *curr)
{
    return !vatomic32_read(&pred->marked) && !vatomic32_read(&curr->marked) &&
           (vatomicptr_read(&pred->next) == curr);
}

#endif
