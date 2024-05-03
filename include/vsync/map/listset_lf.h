/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VLISTSET_LF_H
#define VLISTSET_LF_H
/*******************************************************************************
 * @file listset_lf.h
 * @ingroup lock_free requires_smr linearizable
 * @brief Lock-free implementation of listset.
 *
 * This is a map implementation based on lock-free linked list with unique keys.
 *
 * @example
 * @include eg_listset_lf.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 9.8]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/

#include <vsync/vtypes.h>
#include <vsync/common/assert.h>
#include <vsync/map/internal/listset/listset_common.h>
#include <vsync/atomic/atomicptr_markable.h>
#include <vsync/common/verify.h>

#ifdef VLISTSET_NO_FUN_POINTER
/* to give the user the option to not use function pointers during comparison */
extern inline int vlistset_cmp_key(struct vlistset_node_s *node,
                                   vlistset_key_t k);
    #define VLISTSET_CMP(_l_, _n_, _k_) vlistset_cmp_key((_n_), _k_)
#else
    #define VLISTSET_CMP(_l_, _n_, _k_) ((_l_)->cmp_fun((_n_), _k_))
#endif

typedef struct vlistset_node_s {
    vatomicptr_markable(struct vlistset_node_s *) next;
} vlistset_node_t;

typedef struct vlistset_s {
    vlistset_node_t head_sentinel;
    vlistset_node_t tail_sentinel;
    vlistset_handle_node_t retire_fun;
    void *retire_fun_arg;
    vlistset_cmp_key_t cmp_fun;
} vlistset_t;

static inline vbool_t _vlistset_find(vlistset_t *lst, vlistset_key_t key,
                                     vlistset_node_t **out_pred,
                                     vlistset_node_t **out_curr);

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

    vatomicptr_markable_set_rlx(&lst->head_sentinel.next, &lst->tail_sentinel,
                                false);
    vatomicptr_markable_set_rlx(&lst->tail_sentinel.next, NULL, false);

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
    vlistset_node_t *pred     = NULL;
    vlistset_node_t *curr     = NULL;
    vlistset_node_t *tail     = NULL;
    vbool_t find_modified_mem = false;

    ASSERT(lst);
    ASSERT(node);
    tail = &lst->tail_sentinel;
    while (true) {
        find_modified_mem = _vlistset_find(lst, key, &pred, &curr);

        if (tail != curr && VLISTSET_CMP(lst, curr, key) == 0) {
            return false;
        } else {
            // node->next = curr
            vatomicptr_write(&node->next, curr);

            // we try to attach the new node
            // pred->next = node
            if (vatomicptr_markable_cmpxchg(&pred->next, curr, false, node,
                                            false)) {
                return true;
            } else {
                /* ignore failed iterations without side-effects
                 */
                verification_assume(find_modified_mem);
            }
        } // else
    }     // while
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
    vlistset_node_t *pred     = NULL;
    vlistset_node_t *curr     = NULL;
    vlistset_node_t *succ     = NULL;
    vlistset_node_t *tail     = NULL;
    vbool_t snip              = false;
    vbool_t find_modified_mem = false;
    vbool_t detached          = false;

    ASSERT(lst);
    tail = &lst->tail_sentinel;
    while (true) {
        find_modified_mem = _vlistset_find(lst, key, &pred, &curr);
        if (tail == curr || VLISTSET_CMP(lst, curr, key) != 0) {
            // key does not exit, fail.
            return false;
        } else {
            // find the succ
            succ = vatomicptr_markable_get_pointer(&curr->next);

            /**
             * The book uses attemp_mark here, this is a bug that later they
             * corrected
             * @see https://nanopdf.com/download/errata-5aecfce05a886_pdf
             * If one uses attempt_mark here, two threads removing the same key
             * can succeed. We need to succeed iff the mark has changed from
             * false->true. Thus a cas is the right thing to do.
             */
            snip = vatomicptr_markable_cmpxchg(&curr->next, succ, false, succ,
                                               true);
            // if marking failed, retry
            if (!snip) {
                /* ignore failed iterations */
                verification_assume(find_modified_mem);
                continue;
            }
            // if marking succeeded remove it physically
            // pred->next = succ
            detached = vatomicptr_markable_cmpxchg(&pred->next, curr, false,
                                                   succ, false);
            if (detached) {
                lst->retire_fun(curr, lst->retire_fun_arg);
            }

            return true;
        }
    } // while
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
    vbool_t marked        = false;
    vbool_t found         = false;

    ASSERT(lst);
    tail = &lst->tail_sentinel;
    curr = vatomicptr_markable_get_pointer(&lst->head_sentinel.next);

    while (tail != curr && VLISTSET_CMP(lst, curr, key) < 0) {
        curr = vatomicptr_markable_get_pointer(&curr->next);
    } // while

    if (tail != curr && VLISTSET_CMP(lst, curr, key) == 0) {
        marked = vatomicptr_markable_get_mark(&curr->next);
        found  = !marked;
    }
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
    vlistset_node_t *curr = NULL;
    vlistset_node_t *succ = NULL;
    vbool_t marked        = false;

    ASSERT(lst);
    ASSERT(visitor);

    /* note that head_sentinel can never be logically removed */
    curr = vatomicptr_markable_get_pointer(&lst->head_sentinel.next);

    while (curr && curr != &lst->tail_sentinel) {
        succ = vatomicptr_markable_get(&curr->next, &marked);

        if (!marked || visitor_destructive) {
            visitor(curr, arg);
        }
        curr = succ;
    }
}
/**
 * Locates the position of the node associated with the given key.
 *
 * Traverses the given `lst` looking for a node with key >= to the given
 * key `key`. All marked nodes on the search path will be physically detached.
 *
 * @param lst address of vlistset_t object.
 * @param key key value.
 * @param out_pred output parameter (`*out_pred->next = *out_curr`).
 * @param out_curr output parameter with (`*out_curr->key >= key`).
 * @param out_snipped output parameter. true snip happened, false snip did not
 * happen.
 * @return true repeat is required.
 * @return false repeat is not required.
 *
 * @note the function returns true for repeating if the state of the listset has
 * changed.
 */
static inline vbool_t
_vlistset_find_loop(vlistset_t *lst, vlistset_key_t key,
                    vlistset_node_t **out_pred, vlistset_node_t **out_curr,
                    vbool_t *out_snipped)
{
    vlistset_node_t *pred = NULL;
    vlistset_node_t *curr = NULL;
    vlistset_node_t *succ = NULL;
    vlistset_node_t *tail = NULL;
    vbool_t marked        = false;
    vbool_t snip          = false;

    tail = &lst->tail_sentinel;
    pred = &lst->head_sentinel;
    curr = vatomicptr_markable_get_pointer(&pred->next);

    while (curr != tail) {
        succ = vatomicptr_markable_get(&curr->next, &marked);

        // remove marked nodes
        while (marked) {
            // remove curr
            // unmark succ
            snip = vatomicptr_markable_cmpxchg(&pred->next, curr, false, succ,
                                               false);

#ifdef VSYNC_VERIFICATION
            *out_snipped = *out_snipped | snip;
            verification_assume(*out_snipped);
#else
            V_UNUSED(out_snipped);
#endif
            if (!snip) {
                return true;
            }

            lst->retire_fun(curr, lst->retire_fun_arg);

            if (succ == tail) {
                break;
            }
            curr = succ;
            succ = vatomicptr_markable_get(&curr->next, &marked);
        } // while marked

        if (curr == tail || VLISTSET_CMP(lst, curr, key) >= 0) {
            break;
        }
        pred = curr;
        curr = succ;
    } // while

    *out_pred = pred;
    *out_curr = curr;

    return false;
}
/**
 * Locates the position of the node associated with the given key.
 *
 * Traverses the given `lst` looking for a node with key >= to the given
 * key `key`. All marked nodes on the search path will be physically detached.
 *
 * @param lst address of vlistset_t object.
 * @param key key value.
 * @param out_pred output parameter (`*out_pred->next = *out_curr`).
 * @param out_curr output parameter with (`*out_curr->key >= key`).
 * @return true modified memory.
 * @return false did not modify anything.
 */
static inline vbool_t
_vlistset_find(vlistset_t *lst, vlistset_key_t key, vlistset_node_t **out_pred,
               vlistset_node_t **out_curr)
{
    vbool_t ever_snipped = false;
    while (_vlistset_find_loop(lst, key, out_pred, out_curr, &ever_snipped)) {}
    return ever_snipped;
}

#undef VLISTSET_CMP
#endif
