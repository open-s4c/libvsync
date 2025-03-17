/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VQUEUE_PRIO_TREE_BASED_H
#define VQUEUE_PRIO_TREE_BASED_H
/*******************************************************************************
 * @file vqueue_prio_tree_based.h
 * @brief Tree-Based Bounded Priority Queue.
 * @ingroup requires_smr lock_free unbounded_queue
 *
 * The SimpleTree priority queue is a tree of bounded counters. Items reside
 * at the leaves. Each leaf is a stack (see  elimination_stack.h). Internal tree
 * nodes hold the number of items in the subtree rooted at the node's left
 * child.
 *
 * It has a bounded priority range and unbounded capacity.
 * It requires SMR (see gdump.h)
 *
 * @note This implementation is quiescently consistent and not linearizable.
 * @note A different type of stack can be used e.g., xbo_stack.h, or a stack
 * without a backoff mechanism, which might be better suited to low contention
 * scenarios. Contact the authors, if you need to use different type of
 * stack.
 *
 * @example
 * @include eg_queue_prio_stack_tree.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 15.3]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/

#include <vsync/vtypes.h>
#include <vsync/atomic.h>
#include <vsync/common/dbg.h>
#include <vsync/common/compiler.h>
#include <vsync/utils/math.h>
#include <vsync/stack/elimination_stack.h>

#if !defined(VQUEUE_PRIO_PRIORITY_RANGE)
    /**
     * `VQUEUE_PRIO_PRIORITY_RANGE` configures the queue to accept priorities
     * from [0:VQUEUE_PRIO_PRIORITY_RANGE-1]
     *
     * @note `VQUEUE_PRIO_PRIORITY_RANGE` must be power of two.
     */
    #define VQUEUE_PRIO_PRIORITY_RANGE 16U
#endif

/* calculate how many tree nodes are needed to support the given priority
 * range */
#define VQUEUE_PRIO_STACK_TREE_LEN ((VQUEUE_PRIO_PRIORITY_RANGE * 2U) - 1U)

typedef vstack_node_t vqueue_prio_node_t;

typedef void (*vqueue_prio_handle_node_t)(vqueue_prio_node_t *node, void *arg);

typedef struct vqueue_prio_tree_node_s {
    vatomic32_t counter;
    vstack_t *bin;
} vqueue_prio_tree_node_t;

typedef struct vqueue_prio_s {
    vqueue_prio_tree_node_t tree[VQUEUE_PRIO_STACK_TREE_LEN];
    vstack_t leaves[VQUEUE_PRIO_PRIORITY_RANGE];
    vqueue_prio_handle_node_t retire_cb;
    void *retire_cb_arg;
} vqueue_prio_t;

/* ROOT index */
#define VQUEUE_PRIO_ROOT_IDX 0U
/* maps the given priority `_i_` to a leaf index */
#define VQUEUE_PRIO_GET_LEAF_IDX(_i_)                                          \
    ((_i_) + (VQUEUE_PRIO_PRIORITY_RANGE - 1U))
/* calculates the left child index from the parent index `_i_` */
#define VQUEUE_PRIO_GET_LEFT_CHILD_IDX(_i_) (((_i_)*2U) + 1U)
/* calculates the right child index from the parent index `_i_` */
#define VQUEUE_PRIO_GET_RIGHT_CHILD_IDX(_i_) (((_i_)*2U) + 2U)
/* calculates the parent index from the child index `_i_` */
#define VQUEUE_PRIO_GET_PARENT_IDX(_i_) (((_i_)-1U) / 2U)

/**
 * Private functions
 */
static inline void _vqueue_prio_tree_init(vqueue_prio_t *pqueue,
                                          backoff_rand_fun_t rand_cb);
static inline vbool_t _vqueue_prio_is_leaf_node(vqueue_prio_tree_node_t *node);
static inline vuint32_t _vqueue_prio_bounded_get_dec(vatomic32_t *a);

#ifdef VQUEUE_PRIO_TESTING
static inline void _vqueue_prio_verify_print_tree(vqueue_prio_t *pqueue);
#endif
/**
 * Initializes the queue.
 *
 * @param pqueue address of vqueue_prio_t object.
 * @param retire_cb callback function used for retiring detached nodes to the
 * SMR.
 * @param retire_cb_arg second parameter of `retire_cb`.
 * @param rand_fun a function pointer to a function that generates a random
 * number.
 */
static inline void
vqueue_prio_init(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t retire_cb,
                 void *retire_cb_arg, backoff_rand_fun_t rand_fun)
{
    ASSERT(retire_cb);

    V_STATIC_ASSERT(V_IS_POWER_OF_TWO(VQUEUE_PRIO_PRIORITY_RANGE),
                    "VQUEUE_PRIO_PRIORITY_RANGE should be power of two");

    pqueue->retire_cb     = retire_cb;
    pqueue->retire_cb_arg = retire_cb_arg;

    _vqueue_prio_tree_init(pqueue, rand_fun);

#ifdef VQUEUE_PRIO_TESTING
    _vqueue_prio_verify_print_tree(pqueue);
#endif
}
/**
 * Calls the given callback on all remaining nodes.
 *
 * Nodes can be freed in `destroy_cb`
 *
 * @note this is not thread safe and must be called if and only if all threads
 * are done accessing the queue
 *
 * @param pqueue address of vqueue_prio_t object.
 * @param destroy_cb address of a callback function to call on each remaining
 * node.
 * @param args second argument of `destroy_cb`.
 */
static inline void
vqueue_prio_destroy(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t destroy_cb,
                    void *args)
{
    vsize_t i = 0;
    for (i = 0; i < VQUEUE_PRIO_PRIORITY_RANGE; i++) {
        vstack_destroy(&pqueue->leaves[i], destroy_cb, args);
    }
}
/**
 * Enqueues the given `node`.
 *
 * @param pqueue address of vqueue_prio_t object.
 * @param node address of vqueue_prio_node_t object.
 * @param priority the priority associated with `node`.
 */
static inline void
vqueue_prio_add(vqueue_prio_t *pqueue, vqueue_prio_node_t *pnode,
                vsize_t priority)
{
    vqueue_prio_tree_node_t *parent = NULL;
    vstack_t *bin                   = NULL;
    vsize_t idx                     = 0;
    vsize_t parent_idx              = 0;

    ASSERT(priority < VQUEUE_PRIO_PRIORITY_RANGE);

    bin = &pqueue->leaves[priority];
    vstack_push(bin, pnode);

    idx = VQUEUE_PRIO_GET_LEAF_IDX(priority);

    /* Traverse up the tree, incrementing the counters in the nodes when
     * they ascend from the left */
    while (idx != VQUEUE_PRIO_ROOT_IDX) {
        ASSERT(idx != 0);
        parent_idx = VQUEUE_PRIO_GET_PARENT_IDX(idx);

        /* check if the current node is to the left of its parents, if so
         * increase the count */
        if (idx == VQUEUE_PRIO_GET_LEFT_CHILD_IDX(parent_idx)) {
            parent = &pqueue->tree[parent_idx];
            vatomic32_inc(&parent->counter);
        }

        idx = parent_idx;
    }
}
/**
 * Dequeues a node with highest priority available.
 *
 * @param pqueue address of vqueue_prio_t object.
 * @return vqueue_prio_node_t* address of dequeued object.
 */
static inline vqueue_prio_node_t *
vqueue_prio_remove_min(vqueue_prio_t *pqueue)
{
    vsize_t idx                   = VQUEUE_PRIO_ROOT_IDX;
    vqueue_prio_tree_node_t *node = &pqueue->tree[idx];

    /* we are looking for a leaf bin that contains nodes */
    while (!_vqueue_prio_is_leaf_node(node)) {
        /* if the current counter is greater than zero, it means for sure there
         * are nodes on the left so we go left */
        if (_vqueue_prio_bounded_get_dec(&node->counter) > 0) {
            idx = VQUEUE_PRIO_GET_LEFT_CHILD_IDX(idx);
        } else {
            /* we go right if there is nothing on the left */
            idx = VQUEUE_PRIO_GET_RIGHT_CHILD_IDX(idx);
        }
        node = &pqueue->tree[idx];
    }
    ASSERT(node->bin);

    /* we pop from the current bin */
    vqueue_prio_node_t *qnode = vstack_pop(node->bin);
    if (qnode) {
        pqueue->retire_cb(qnode, pqueue->retire_cb_arg);
    }
    return qnode;
}
/**
 * Calls the given `visitor` function on each pqueue node object residing
 * the given `pqueue`
 *
 * @param pqueue
 * @param visitor the address of the function to call on every visited node
 * @param arg
 */
static inline void
_vqueue_prio_visit(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t visitor,
                   void *arg)
{
    vsize_t i       = 0;
    vstack_t *stack = NULL;

    for (i = 0; i < VQUEUE_PRIO_PRIORITY_RANGE; i++) {
        stack = &pqueue->leaves[i];
        _vstack_visit(stack, visitor, arg);
    }
}
/**
 * Decrements the given vatomic32_t iff it is not zero
 *
 * @param a
 * @return vuint32_t the value before decrementing the counter
 */
static inline vuint32_t
_vqueue_prio_bounded_get_dec(vatomic32_t *a)
{
    vuint32_t new_val = 0;
    vuint32_t old_val = 0;
    vuint32_t cur_val = 0;

    old_val = vatomic32_read(a);

    while (true) {
        if (old_val == 0) {
            return 0;
        }

        new_val = old_val - 1;
        cur_val = vatomic32_cmpxchg(a, old_val, new_val);
        if (cur_val == old_val) {
            return old_val;
        }

        old_val = cur_val;
        verification_ignore();
    }
}
/**
 * Initializes the tree of counters into an array.
 * Assume we have priority range of N=4 [0...3]
 * We need (N*2 -1)=7 nodes for the tree
 * The nodes reside into an array.
 * e.g. this tree
 *             A
 *          /     \
 *         B       C
 *       /   \   /   \
 *      D     E  F    G
 * will be represented in an array as follows:
 * A, B, C, D, E, F, G
 * with indexes 0, ..., 7
 *
 * @note only leaf nodes D, E, F, G hold references to stack bins and there
 * the actual nodes associated with the priorities reside
 * @param pqueue address of vqueue_prio_t object
 * @param rand_fun  a function pointer to a function that generates a random
 * number.
 */
static inline void
_vqueue_prio_tree_init(vqueue_prio_t *pqueue, backoff_rand_fun_t rand_fun)
{
    vsize_t i                     = 0;
    vsize_t idx                   = 0;
    vqueue_prio_tree_node_t *tree = pqueue->tree;

    for (i = 0; i < VQUEUE_PRIO_STACK_TREE_LEN; i++) {
        vatomic32_write_rlx(&tree[i].counter, 0);
        tree[i].bin = NULL;
    }

    for (i = 0; i < VQUEUE_PRIO_PRIORITY_RANGE; i++) {
        idx = VQUEUE_PRIO_GET_LEAF_IDX(i);

        DBG_BLUE("leaf %zu index in the tree is %zu", i, idx);

        ASSERT(idx >= (VQUEUE_PRIO_PRIORITY_RANGE - 1));
        ASSERT(idx < VQUEUE_PRIO_STACK_TREE_LEN);

        vstack_init(&pqueue->leaves[i], rand_fun);

        tree[idx].bin = &pqueue->leaves[i];
    }
}
/**
 * Checks if the given tree node is a leaf node
 *
 * @param node address of vqueue_prio_node_t object
 * @return true the given node is a leaf
 * @return false the given node isn't a leaf
 */
static inline vbool_t
_vqueue_prio_is_leaf_node(vqueue_prio_tree_node_t *node)
{
    return node->bin != NULL;
}
/**
 * Prints and verifies the tree structure
 * @param pqueue address of vqueue_prio_node_t object
 */
#ifdef VQUEUE_PRIO_TESTING
static inline void
_vqueue_prio_verify_print_tree(vqueue_prio_t *pqueue)
{
    vqueue_prio_tree_node_t *tree = pqueue->tree;
    vqueue_prio_tree_node_t *node = NULL;
    vsize_t idx                   = 0;
    vsize_t l                     = 0;
    vsize_t r                     = 0;
    vsize_t num_leaves            = 0;
    vsize_t num_non_leaves        = 0;

    DBG_YELLOW("# of nodes = %d", VQUEUE_PRIO_STACK_TREE_LEN);

    for (idx = 0; idx < VQUEUE_PRIO_STACK_TREE_LEN; idx++) {
        node = &tree[idx];

        l = VQUEUE_PRIO_GET_LEFT_CHILD_IDX(idx);
        r = VQUEUE_PRIO_GET_RIGHT_CHILD_IDX(idx);

        ASSERT(VQUEUE_PRIO_GET_PARENT_IDX(l) == idx);
        ASSERT(VQUEUE_PRIO_GET_PARENT_IDX(r) == idx);

        if (_vqueue_prio_is_leaf_node(node)) {
            DBG_COLOR(COL_L_YELLOW, "I am %zu a leaf node without children",
                      idx);
            ASSERT(VQUEUE_PRIO_GET_LEAF_IDX(num_leaves) == idx);
            num_leaves++;
        } else {
            DBG_COLOR(COL_L_CYAN,
                      "I am %zu my left child is %zu my right is %zu", idx, l,
                      r);
            num_non_leaves++;
        }
    }
    ASSERT(num_leaves == VQUEUE_PRIO_PRIORITY_RANGE);
    ASSERT(VQUEUE_PRIO_PRIORITY_RANGE - 1 == num_non_leaves);
}
#endif

#undef VQUEUE_PRIO_STACK_TREE_LEN
#undef VQUEUE_PRIO_ROOT_IDX
#undef VQUEUE_PRIO_GET_LEAF_IDX
#undef VQUEUE_PRIO_GET_LEFT_CHILD_IDX
#undef VQUEUE_PRIO_GET_RIGHT_CHILD_IDX
#undef VQUEUE_PRIO_GET_PARENT_IDX
#endif
