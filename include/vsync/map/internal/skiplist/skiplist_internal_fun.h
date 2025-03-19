/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSKIPLIST_INTERNAL_FUNCTIONS_H
#define VSKIPLIST_INTERNAL_FUNCTIONS_H

#include <vsync/map/internal/skiplist/skiplist_types.h>

/**
 * initializes the given skiplist node
 *
 * @param node address of a skiplist node to initialize
 * @param height the height of the given node, i.e. to how many skiplist levels
 * it is going to connect to
 */
static inline void
_vskiplist_init_node(vskiplist_node_t *node, vsize_t height)
{
    ASSERT(height <= VSKIPLIST_MAX_HEIGHT);

    node->height = height;
    // the node is going to be connected to `height` levels, we set the
    // non_snipped_level to that
    vatomicsz_write_rlx(&node->non_snipped_level, height);
}

/**
 * Looks for a given key and returns its associated node along with
 * all of its successors in `succs` and predecessors in `preds` on all levels
 * connected to node.
 * This function is used by add and remove operations to locate the node. In
 * addition to locating the sought node, it also physically detaches all
 * logically removed (marked) nodes that preced the sought node and retires them
 *
 * @param lst address of vskiplist_t object
 * @param key the key to look for
 * @param preds output parameter array of found node's predecessors
 * @param succs output parameter array of found node's successors
 * @return vskiplist_node_t*
 *      address of the node associated with the given key
 *      NULL: no node associated with the given key was found
 */
static inline vskiplist_node_t *
_vskiplist_find(vskiplist_t *lst, vskiplist_key_t key, vskiplist_node_t **preds,
                vskiplist_node_t **succs, vbool_t find_specific,
                vskiplist_node_t *node)
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

    /* starting from the head ending at the tail, we search from top to
     * bottom.
     * the loop shall visit nodes on each level \in [top_level, 0]. We write the
     * condition of the loop as `<= top_level` instead of `>= 0`, because
     * vsize_t is unsigned and overflow can occur on level--. */
    for (level = top_level; level <= top_level; level--) {
        // this cast should be safe because the height cannot be that large
        ASSERT(level < pred->height);

        curr = (vskiplist_node_t *)vatomicptr_markable_get_pointer(
            &pred->next[level]);
        ASSERT(curr);

        while (true) {
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
                    verification_ignore();
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
            if (curr != tail && lst->fun_cmp(curr, key) < 0) {
                if (unlikely(find_specific)) {
                    ASSERT(node);
                    if (curr == node) {
                        /* relevant to the priority queue API */
                        break;
                    }
                }
                pred = curr;
                curr = succ;
            } else {
                // we either reached the end of this level, or encountered a
                // node with key greater or equal to the key we are looking
                // for. we break the while and go forward with the for
                // moving down a level
                //  |
                // \ /
                break;
            }
        } // while(true)

        /* update succ and pred for this level */
        preds[level] = pred;
        succs[level] = curr;
    } // for levels

    ASSERT(curr);
    ASSERT(curr != head);

    // at this point we reached the lowest level
    if (curr != tail && lst->fun_cmp(curr, key) == 0) {
        // we found the node associated with the given key
        return curr;
    } else {
        return NULL;
    }
}
/**
 * Returns the skiplist node associated with the given key if found. If it was
 * not found and `accept_next_gt_key` was set to `true` the node associated with
 * next greater key than the given key is returned instead. If no such node
 * exists NULL is returned
 *
 * @param lst address of vskiplist_t object
 * @param key the key to look for
 * @param accept_next_gt_key
 *      true: if no node associated with the given key is found, return the node
 *      with the next greater key instead
 *      false: if no node associated with the given key is found, return NULL
 * @return vskiplist_node_t* depending on the value of `accept_next_gt_key` can
 *  be:
 *     - NULL: `accept_next_gt_key == false?` no node associated with the given
 *      key was found
 *     - NULL: `accept_next_gt_key == true?` neither a node associated with the
 *      given key nor a node associated with a greater key was found
 *     - address of the skiplist node associated with the given key, if such
 *      node is found
 *     - address of the skiplist node associated with the next greater key
 *      than the given key, if the associated node is not found and
 *      `accept_next_gt_key = true`
 */
static inline vskiplist_node_t *
_vskiplist_lookup(vskiplist_t *lst, vskiplist_key_t key,
                  vbool_t accept_next_gt_key)
{
    vsize_t top_level      = lst->seed.max_height - 1;
    vsize_t level          = 0;
    vbool_t marked         = false;
    vskiplist_node_t *pred = NULL;
    vskiplist_node_t *curr = NULL;
    vskiplist_node_t *succ = NULL;
    vskiplist_node_t *head = _vskiplist_head(lst);
    vskiplist_node_t *tail = _vskiplist_tail(lst);

    /* start searching from the head */
    pred = head;

    /* from the top level to the bottom
     * the loop shall visit nodes on each level \in [top_level, 0]. We write the
     * condition of the loop as `<= top_level` instead of `>= 0`, because
     * vsize_t is unsigned and overflow can occur on level--. */
    for (level = top_level; level <= top_level; level--) {
        curr = (vskiplist_node_t *)vatomicptr_markable_get_pointer(
            &pred->next[level]);
        ASSERT(curr);

        while (true) {
            succ = (vskiplist_node_t *)vatomicptr_markable_get(
                &curr->next[level], &marked);

            /* we skip all logically removed (marked) nodes in this level
             */
            while (marked) {
                curr = (vskiplist_node_t *)vatomicptr_markable_get_pointer(
                    &curr->next[level]);
                succ = (vskiplist_node_t *)vatomicptr_markable_get(
                    &curr->next[level], &marked);
            } // while(marked)

            ASSERT(curr != head);
            // if we reach here, it means curr is not marked
            if (curr != tail && lst->fun_cmp(curr, key) < 0) {
                /* we continue moving forward on this level -> */
                pred = curr;
                curr = succ;
            } else {
                /* we encountered a node with key k >= key or reached the end
                 * we break the while loop, and continue with the for loop
                 * checking for the existence on the lower levels
                 *  |
                 * \ /
                 */
                break;
            }
        } // while(true)
    }     // for levels

    /* if the key exists, then we found the node with the associated key.
     * However, if the key does not exist, it means curr->key > key, and that
     * there is no node s.t. key > node->key > curr->key.
     * This allows us to return curr if the next larger key is requested
     */
    ASSERT(curr != head);
    if (curr != tail && (lst->fun_cmp(curr, key) == 0 || accept_next_gt_key)) {
        return curr;
    }
    return NULL;
}
/**
 * Marks the given node as logically removed on all levels
 *
 * @param node address of vskiplist_node_t node object
 * @return true the calling thread is the one who marked the bottom level
 * @return false some other thread already marked the bottom level
 */
static inline vbool_t
_vskiplist_logically_remove_node(vskiplist_node_t *node)
{
    vsize_t level          = 0;
    vskiplist_node_t *succ = NULL;
    vbool_t node_marked    = false;
    vbool_t i_marked_it    = false;

    ASSERT(node);
    ASSERT(node->height > 0);
    vsize_t top_level = node->height - 1U;

    /* we mark from top to bottom
     * the loop shall visit nodes on each level \in [top_level, 0]. We write the
     * condition of the loop as `<= top_level` instead of `>= 0`, because
     * vsize_t is unsigned and overflow can occur on level--. */
    for (level = top_level; level <= top_level; level--) {
        /* TODO: once vatomicptr_or is supported we can use that to mark the top
         * levels */
        i_marked_it = false;

        do {
            /* find out the current state of node and what succ it has */
            succ = (vskiplist_node_t *)vatomicptr_markable_get(
                &node->next[level], &node_marked);

            if (node_marked) {
                /* already marked, someone else marked it on this level */
                verification_ignore();
                break;
            }
            /* if this cmpxchg succeeds it means we are the ones who marked the
             * node on this level */
            i_marked_it = vatomicptr_markable_cmpxchg(&node->next[level], succ,
                                                      false, succ, true);
        } while (!i_marked_it);
    } /* for all levels */

    /* we return the result of marking the bottom level */
    return i_marked_it;
}
/**
 * physically detaches the given `node` from the skiplist
 *
 * @param lst address of vskiplist_t object
 * @param key the key value associated withe node
 * @param node address of vskiplist_node_t node object
 * @return true node is removed successfully
 * @return false node is not found
 */
static inline vbool_t
_vskiplist_detach_node(vskiplist_t *lst, vskiplist_key_t key,
                       vskiplist_node_t *node)
{
    vskiplist_node_t *succs[VSKIPLIST_MAX_HEIGHT];
    vskiplist_node_t *preds[VSKIPLIST_MAX_HEIGHT];
    vbool_t marked = false;

    marked = _vskiplist_logically_remove_node(node);
    if (marked) {
        _vskiplist_find(lst, key, preds, succs, true, node);
    }
    return marked;
}
/**
 * A visitor function, iterates over all nodes connected to the given level of
 * the skiplist and calls `visit` on them
 *
 * @param lst address of vskiplist_t object
 * @param level level must be less than the skiplist height, and it indicates
 * the level to visit
 * @param visit function pointer, takes a skiplist node address and a vbool_t as
 * parameters. The vbool_t parameter indicates if the node is logically removed
 * @param args the third parameter of the callback function `visit`
 */
static inline void
_vskiplist_visit_level(vskiplist_t *lst, vsize_t level,
                       void (*visit)(vskiplist_node_t *, vbool_t, void *),
                       void *args)
{
    vskiplist_node_t *head = _vskiplist_head(lst);
    vskiplist_node_t *tail = _vskiplist_tail(lst);
    vskiplist_node_t *curr = NULL;
    vskiplist_node_t *succ = NULL;
    vbool_t marked         = false;

    ASSERT(lst);
    ASSERT(level < head->height);

    /* the next of head is never marked, because the head is a sentinel and
     * never removed */
    curr =
        (vskiplist_node_t *)vatomicptr_markable_get_pointer(&head->next[level]);

    ASSERT(curr);

    if (curr == tail) {
        return;
    }

    do {
        // if the succ (curr->next) is marked, then
        // the curr is logically removed
        succ = (vskiplist_node_t *)vatomicptr_markable_get(&curr->next[level],
                                                           &marked);

        visit(curr, marked, args);

        curr = succ;
    } while (curr != tail);
}

/**
 * Decrements the `non_snipped_level` count.
 *
 * @param node address of vskiplist_node_t object.
 * @return true `non_snipped_level = 0`
 * @return false `non_snipped_level != 0`
 */
static inline vbool_t
_vskiplist_disconnect_node(vskiplist_node_t *node)
{
    vsize_t non_snipped = vatomicsz_dec_get_rlx(&node->non_snipped_level);
    ASSERT(non_snipped <= VSKIPLIST_MAX_HEIGHT);
    return non_snipped == 0;
}
/**
 * Retires the given node.
 *
 * @param node address of vskiplist_node_t object.
 * @param marked flag when set indicates that node is logically removed.
 * @param args address of vskiplist_t object.
 */
static inline void
_vskiplist_destroy_node(vskiplist_node_t *node, vbool_t marked, void *args)
{
    vskiplist_t *lst           = (vskiplist_t *)args;
    vbool_t fully_disconnected = _vskiplist_disconnect_node(node);
    if (fully_disconnected) {
        lst->fun_retire(node, lst->fun_retire_arg);
    }
    V_UNUSED(marked);
}
#endif
