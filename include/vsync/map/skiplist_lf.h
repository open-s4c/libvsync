/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSKIPLIST_H
#define VSKIPLIST_H
/*******************************************************************************
 * @file  skiplist_lf.h
 * @brief Lock-free concurrent skiplist
 * @ingroup requires_smr linearizable lock_free
 *
 * @note users can configure the maximum levels the skiplist can use by defining
 *`-DVSKIPLIST_MAX_HEIGHT=H`. `H=32` by default.
 *
 * @example
 * @include eg_skiplist_lf.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 14.4]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/
#include <vsync/vtypes.h>
#include <vsync/common/verify.h>
#include <vsync/common/dbg.h>
#include <vsync/map/internal/skiplist/skiplist_helper.h>
#include <vsync/map/internal/skiplist/skiplist_internal_fun.h>

/**
 * Initializes the skiplist.
 *
 * @note call it before threads start accessing the skiplist.
 *
 * @param lst address of vskiplist_t object.
 * @param fun_cmp  function pointer. This function is used to compare a given
 *  skiplist node against a given key. It must return the following:
 *  - container(node)->key == key -> 0
 *  - container(node)->key < key -> -1
 *  - container(node)->key > key -> 1
 * @param fun_retire function pointer to callback retire function.
 * @param fun_retire_arg the second parameter of `fun_retire`.
 * @param rand_seed a random seed used in random node height generation.
 * @param head the address of the dynamically allocated head sentinel object of
 * size `VSKIPLIST_SENTINEL_SZ`.
 * @param tail the address of the dynamically allocated tail sentinel object of
 * size `VSKIPLIST_SENTINEL_SZ`.
 *
 * @note both `head` and `tail` must have size `VSKIPLIST_SENTINEL_SZ`.
 * @note `head` and `tail` must never be modified directly by the user, and they
 * must be allocated before `vskiplist_init`. They can be freed only after
 * `vskiplist_destroy`. i.e. they must have a life span larger or equal to
 * `lst`.
 */
static inline void
vskiplist_init(vskiplist_t *lst, vskiplist_cmp_node_t fun_cmp,
               vskiplist_handle_node_t fun_retire, void *fun_retire_arg,
               vuint32_t rand_seed, vskiplist_node_t *head,
               vskiplist_node_t *tail)
{
    vsize_t i = 0;

    ASSERT(lst);
    ASSERT(head);
    ASSERT(tail);
    ASSERT(head != tail);

    _vskiplist_seed_init(&lst->seed, rand_seed, VSKIPLIST_MAX_HEIGHT);

    ASSERT(fun_cmp);
    ASSERT(fun_retire);

    lst->fun_cmp        = fun_cmp;
    lst->fun_retire     = fun_retire;
    lst->fun_retire_arg = fun_retire_arg;
    lst->head           = head;
    lst->tail           = tail;

    _vskiplist_init_node(head, VSKIPLIST_MAX_HEIGHT);
    _vskiplist_init_node(tail, VSKIPLIST_MAX_HEIGHT);

    /* forall available levels do
     * head->next[i] = tail
     * tail->next[i] = NULL
     */
    for (i = 0; i < VSKIPLIST_MAX_HEIGHT; i++) {
        vatomicptr_markable_set(&head->next[i], tail, false);
        vatomicptr_markable_set(&tail->next[i], NULL, false);
    }
}
/**
 * Destroys all existing nodes in the skiplist.
 *
 * @note the destruction is done by calling retire function on all remaining
 * nodes.
 * @note call after threads are done accessing the skiplist.
 * @param lst address of vskiplist_t object.
 */
static inline void
vskiplist_destroy(vskiplist_t *lst)
{
    vsize_t top_level = VSKIPLIST_MAX_HEIGHT - 1;
    /**
     * We need to scan upper levels, because there maybe nodes connected to
     * upper levels but not to the bottom.
     * _vskiplist_destroy_node will retire those with non_snipped_levels = 0
     * the loop shall visit nodes on each level \in [top_level, 0]. We write the
     * condition of the loop as `i <= top_level` instead of `i >= 0` because
     * vsize_t is unsigned and overflow can occur on i = 0, i--. This way we
     * handle such case.
     */
    for (vsize_t i = top_level; i <= top_level; i--) {
        _vskiplist_visit_level(lst, i, _vskiplist_destroy_node, lst);
    }
}
/**
 * Searches for the skiplist node associated with the given key.
 *
 * @param lst address of vskiplist_t object.
 * @param key the key you are looking for.
 * @return vskiplist_node_t* address of the skiplist node associated with the
 * given key if exists.
 * @return NULL if no node associated with the given key was found.
 * @note call within SMR critical section.
 */
static inline vskiplist_node_t *
vskiplist_lookup(vskiplist_t *lst, vskiplist_key_t key)
{
    return _vskiplist_lookup(lst, key, false);
}
/**
 * Calculates the required size and height of `vskiplist_node_t` container.
 *
 * @param lst address of vskiplist_t object.
 * @param container_sz the size of the container object returned by sizeof.
 * @param height output parameter. Indicates to how many levels the node will be
 * connected to in the skiplist.
 *
 * @return vsize_t the minimum required size of the container object of
 * `vskiplist_node_t`.
 *
 * @note users must call this function everytime they wish to add a new node to
 * the skiplist. see `vskiplist_add`.
 *
 * @note if the node is not embedded in a container struct. Pass `container_sz`
 * as `sizeof(vskiplist_node_t)`.
 */
static inline vsize_t
vskiplist_calc_node_sz(vskiplist_t *lst, vsize_t container_sz, vsize_t *height)
{
    ASSERT(lst);
    ASSERT(height);
    *height = _vskiplist_get_rand_height(&lst->seed);
    ASSERT(*height < VSKIPLIST_MAX_HEIGHT);
    return container_sz + (sizeof(vatomicptr_markable_t) * (*height));
}
/**
 * Inserts `node` into the skiplist.
 *
 * @param lst address of vskiplist_t object.
 * @param key the key associated with `node`.
 * @param node address of vskiplist_node_t object.
 * @param out_node output parameter contains the address of the existing node
 * associated with the given key, if the key already exists and insertion
 * failed, otherwise it contains NULL.
 * @param height the value of the output param `height` updated by
 * `vskiplist_calc_node_sz`.
 *
 * @return true the key did not exist, and `node` is added successfully.
 * @return false the key already exists, `node` is not added.
 *
 * @note call within SMR critical section.
 * @note `out_node` can be NULL.
 * @note see `vskiplist_calc_node_sz` for container size calculation.
 */
static inline vbool_t
vskiplist_add(vskiplist_t *lst, vskiplist_key_t key, vskiplist_node_t *node,
              vskiplist_node_t **out_node, vsize_t height)

{
    vskiplist_node_t *preds[VSKIPLIST_MAX_HEIGHT];
    vskiplist_node_t *succs[VSKIPLIST_MAX_HEIGHT];
    vskiplist_node_t *succ       = NULL;
    vskiplist_node_t *pred       = NULL;
    vskiplist_node_t *found_node = NULL;
    vskiplist_node_t *cur_succ   = NULL;
    vbool_t mark                 = false;
    vsize_t level                = 0;

    ASSERT(node && "node must be allocated");

    /**
     * The following assertion is turned off, because for priority queue this
     * will not hold, and it is intended to be like that.
     * However, in a context unrelated to priority queue, this must hold.
     *
     * ASSERT(lst->fun_cmp(node, key) == 0 &&
     *          "the association between the node and the key is a "
     *         "prerequisite for insertion");
     */

    _vskiplist_init_node(node, height);

    while (true) {
        found_node = _vskiplist_find(lst, key, preds, succs, false, NULL);
        if (found_node) {
            if (out_node) {
                *out_node = found_node;
            }
            return false;
        }

        /* we connect the node's next on all levels to the successors found by
         * find */
        for (level = 0; level < height; level++) {
            succ = succs[level];
            vatomicptr_markable_set(&node->next[level], succ, false);
        }

        pred = preds[0];
        succ = succs[0];

        /* we connect level zero first, iff this succeeds we proceed to connect
         * the rest of the levels */
        if (!vatomicptr_markable_cmpxchg(&pred->next[0], succ, false, node,
                                         false)) {
            continue;
        }

        /* we proceed to connect the top-levels  */
        for (level = 1; level < height; level++) {
            while (true) {
                pred = preds[level];
                succ = succs[level];

                do {
                    /* we don't want to lose shortcuts in the skip-list, and
                     * avoid a situation where n is connected to old succ and
                     * not to new nodes that came into the picture later
                     *
                     * Scenario #1
                     * 1: n -> s, p -> s
                     * 2: n-> s,  p-> y ->s (y has been added)
                     * 3: p-> n -> s, y-> s (y is unreachable at that level)
                     *
                     * Scenario #2
                     *  1: n -> s, p -> s
                     *  2: n-> s, p-> y-> X (s is now detached and recycled)
                     *  3: p-> n -> s, y-> X (y is unreachable at that level),
                     * (s becomes visible again and access to it is unsafe)
                     *
                     * We avoid the above scenarios by making sure we connect
                     * to the right successor
                     * */
                    cur_succ = (vskiplist_node_t *)vatomicptr_markable_get(
                        &node->next[level], &mark);
                    if (cur_succ == succ) {
                        // nothing changed
                        break;
                    }
                    // we update the successor
                } while (!vatomicptr_markable_cmpxchg(
                    &node->next[level], cur_succ, mark, succ, mark));

                // we connect the node to this level
                if (vatomicptr_markable_cmpxchg(&pred->next[level], succ, false,
                                                node, false)) {
                    // we succeeded, we move on to connect the remaining levels
                    break;
                }
                // we failed, we reissue the find to detect the change of preds
                // and succs on the upper levels
                _vskiplist_find(lst, key, preds, succs, false, NULL);
            } // while(true)
        }     // for
        return true;
    } // while(true)
}
/**
 * Removes the skiplist node associated with the given key.
 *
 * @param lst address of vskiplist_t object.
 * @param key the key associated with the node you want to remove.
 * @return true a node associated with the given key has been found and removed.
 * @return false no node associated with the given key was found.
 * @note call within SMR critical section.
 */
static inline vbool_t
vskiplist_remove(vskiplist_t *lst, vskiplist_key_t key)
{
    vskiplist_node_t *preds[VSKIPLIST_MAX_HEIGHT];
    vskiplist_node_t *succs[VSKIPLIST_MAX_HEIGHT];
    vskiplist_node_t *node = NULL;

    /* find out if a node with the given key exists */
    node = _vskiplist_find(lst, key, preds, succs, false, NULL);
    if (node) {
        ASSERT(lst->fun_cmp(node, key) == 0);
        /* a node has been found */
        return _vskiplist_detach_node(lst, key, node);
    } else {
        /* no node associated with the given key was found */
        return false;
    }
}
/**
 * Searches for the skiplist node associated with the given key.
 *
 * Similar to `vskiplist_lookup` looks for a node associated with the given
 * `key`, however if no such node is found it returns the address of the node
 * associated with key `k` such that `k > key` and there is no key `k'` such
 * that `key < k' < k`.
 * @note extension API
 * @param lst address of vskiplist_t object.
 * @param key the key you are looking for.
 * @return vskiplist_node_t* the address of the node associated with key if
 * found, otherwise it returns the address of the node associated with `k >
 * key`. If there is no such `k` it returns NULL.
 * @note call within SMR critical section.
 */
static inline vskiplist_node_t *
vskiplist_lookup_next(vskiplist_t *lst, vskiplist_key_t key)
{
    return _vskiplist_lookup(lst, key, true);
}

/**
 * Returns the successor of the given node `curr` if a successor exists.
 *
 * @note a successor satisfies `container(succ)->key > container(curr)->key` and
 * there exists no node n such that `container(succ)->key > container(n)->key
 * > container(curr)->key`, otherwise `n` is the successor
 * @note extension API, the user is responsible for making sure that curr points
 * to a valid non-reclaimed node
 * @param lst address of vskiplist_t object.
 * @param curr address of a skiplist node.
 * @return vskiplist_node_t*
 *      address of the successor: if successor exists
 *      NULL: if successor does not exist or curr has been removed.
 * @note call within the same SMR critical section where `curr` was retrieved.
 */
static inline vskiplist_node_t *
vskiplist_get_succ(vskiplist_t *lst, vskiplist_node_t *curr)
{
    const vsize_t level         = 0;
    vskiplist_node_t *succ      = NULL;
    vskiplist_node_t *succ_next = NULL;
    vbool_t succ_removed        = false;

    ASSERT(lst && "lst is NULL!");
    ASSERT(curr && "curr is NULL!");

    succ_next =
        (vskiplist_node_t *)vatomicptr_markable_get_pointer(&curr->next[level]);

    do {
        /* we should never get NULL here, because if we reached the tail, the
         * tail cannot be marked */
        ASSERT(succ_next);
        succ = succ_next;
        /* we need to make sure that the succ is not removed, for this we
         * check if the next pointer of succ is marked, we keep reading
         * till we encounter a succ that is not logically removed */
        succ_next = (vskiplist_node_t *)vatomicptr_markable_get(
            &succ->next[level], &succ_removed);
    } while (succ_removed);

    /* we double check that we did not reach the tail! */
    if (succ == _vskiplist_tail(lst)) {
        succ = NULL;
    }
    return succ;
}
/**
 * Returns the first node in the given `lst`.
 *
 * i.e. the element that is mapped to the smallest key.
 * @note nodes that are logically removed (i.e. marked) are treated as
 * nonexistent.
 * @param lst address of vskiplist_t object.
 * @return vskiplist_node_t* address of the first node.
 * @note call within  SMR critical section.
 */
static inline vskiplist_node_t *
vskiplist_get_first(vskiplist_t *lst)
{
    vbool_t marked         = false;
    vskiplist_node_t *node = NULL;
    vskiplist_node_t *head = _vskiplist_head(lst);
    node = (vskiplist_node_t *)vatomicptr_markable_get_pointer(&head->next[0]);
    if (node == _vskiplist_tail(lst)) {
        /* skiplist is empty */
        return NULL;
    }

    marked = vatomicptr_markable_get_mark(&node->next[0]);
    if (marked) {
        /* the first item is marked, we try to get its not marked successor  */
        return vskiplist_get_succ(lst, node);
    } else {
        /* the first item is not marked we can return it */
        return node;
    }
}
/**
 * Removes the given node associated with the given key if it exists.
 *
 * It fails if no node associated with the given key was found, or if the found
 * node is not the same as the given node.
 * @note extension API, the user is responsible for making sure that the
 * expected_node `node` is valid and points to a non-reclaimed memory.
 * @param lst address of vskiplist_t object.
 * @param key the key associated with the node to remove.
 * @param node address of the node to remove.
 * @return true the node associated with the given key has been removed.
 * @return false there is no node associated with the given key, or the existing
 * node is different than the given node.
 * @note call within the same SMR critical section where `node` was retrieved.
 */
static inline vbool_t
vskiplist_remove_node(vskiplist_t *lst, vskiplist_key_t key,
                      vskiplist_node_t *node)
{
    vskiplist_node_t *succs[VSKIPLIST_MAX_HEIGHT];
    vskiplist_node_t *preds[VSKIPLIST_MAX_HEIGHT];
    vskiplist_node_t *existing_node = NULL;

    ASSERT(lst);
    ASSERT(node);

    existing_node = _vskiplist_find(lst, key, preds, succs, false, NULL);
    if (existing_node != node) {
        return false;
    }
    return _vskiplist_detach_node(lst, key, node);
}
/**
 * Triggers the physical removal of all logically removed nodes.
 *
 * It is a modified copy _vskiplist_find.
 * @note this function is experimental and subject to removal.
 * @param lst address of vskiplist_t object.
 * @note call within  SMR critical section.
 */
static inline void
vskiplist_cleanup(vskiplist_t *lst)
{
    vsize_t top_level      = lst->seed.max_height - 1;
    vsize_t level          = 0;
    vbool_t marked         = false;
    vbool_t snip           = false;
    vskiplist_node_t *pred = NULL;
    vskiplist_node_t *curr = NULL;
    vskiplist_node_t *succ = NULL;
    vskiplist_node_t *tail = _vskiplist_tail(lst);
    vskiplist_node_t *head = _vskiplist_head(lst);
    vsize_t o_non_snip_lvl = 0;

RETRY:
    pred = head;

    /* makes sure that it was initialized */
    ASSERT(pred->height != 0);

    /* the loop shall visit nodes on each level \in [top_level, 0]. We write the
     * condition of the loop as `<= top_level` instead of `>= 0`, because
     * vsize_t is unsigned and overflow can occur on level--. */
    for (level = top_level; level <= top_level; level--) {
        // this cast should be safe because the height cannot be that large
        ASSERT(level < pred->height);

        curr = (vskiplist_node_t *)vatomicptr_markable_get_pointer(
            &pred->next[level]);
        ASSERT(curr);

        while (curr != tail) {
            succ = (vskiplist_node_t *)vatomicptr_markable_get(
                &curr->next[level], &marked);

            /* curr is marked, we are going to detach all the marked nodes
             * we encounter on our way */
            while (marked) {
                /* attempt to detach the node pred->next[level] = succ */
                snip = vatomicptr_markable_cmpxchg(&pred->next[level], curr,
                                                   false, succ, false);
                if (!snip) {
                    /* failed to detach retry */
                    goto RETRY;
                }

                /* we decrease the number of non snipped level.
                 * Relaxing the barrier causes a WMM #bug. Meaning, the
                 * effect of this decrement can be visible before the cas
                 * above, which will result into handing the node to the SMR
                 * before it has been detached. This can cause access to
                 * freed memory!! DO NOT RELAX THE BARRIER */
                o_non_snip_lvl = vatomicsz_get_dec(&curr->non_snipped_level);

                ASSERT(o_non_snip_lvl != 0);
                ASSERT(o_non_snip_lvl > 0);

                /* if the previous value is 1, it means we just snipped the
                 * last remaining link */
                if (o_non_snip_lvl == 1) {
                    /* we know at this point that the insertion of this node
                     * has completed at all levels and so is the snipping.
                     * The node is now completely detached from the
                     * skip-list and can be handed in to the SMR */
                    ASSERT(curr != head);
                    ASSERT(curr != tail);
                    /* we retire the node */
                    lst->fun_retire(curr, lst->fun_retire_arg);
                }

                // move forward on this level ->
                curr = (vskiplist_node_t *)vatomicptr_markable_get_pointer(
                    &pred->next[level]);
                succ = (vskiplist_node_t *)vatomicptr_markable_get(
                    &curr->next[level], &marked);
            } // while(marked)

            ASSERT(curr != head);
            if (curr == tail) {
                break;
            }
            pred = curr;
            curr = succ;
        } // while(curr != tail)
    }     // for levels
}
#endif
