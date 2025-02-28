/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_RBTREE_H
#define VSYNC_RBTREE_H

/*******************************************************************************
 * @file rbtree.h
 * @ingroup requires_smr linearizable
 * @brief This implementation of red-black tree uses a latch sequence counter
 * to get lockless readers and non-starving writers.
 *
 * A red-black tree is a balanced binary search tree with nodes that are ordered
 * by unique keys.
 *
 * The writers are protected by a lock and update two copies of the rbtree.
 * Thus at least one copy is always available for the readers. Should a reader
 * not finish before a writer starts updating another copy, this will be
 * spotted by the sequence counter, and the reader will restart.
 *
 * @example
 * @include eg_rbtree.c
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/utils/abstract_lock.h>
#include <vsync/spinlock/ttaslock.h>

#include <vsync/map/internal/rbtree/rbtree_common.h>
#include <vsync/common/compiler.h>

DEF_ABSTRACT_LOCK(vrbtree_lock, ttaslock_t, ttaslock_init, ttaslock_acquire,
                  ttaslock_release, ttaslock_tryacquire)

typedef struct vrbtree_seq_node_s {
    vbool_t color;
    vatomicptr(struct vrbtree_seq_node_s *) parent;
    vatomicptr(struct vrbtree_seq_node_s *) child[2];
} vrbtree_seq_node_t;

typedef struct vrbtree_node_s {
    vrbtree_seq_node_t node[2];
} vrbtree_node_t;

typedef struct vrbtree_s {
    vrbtree_lock_t lock;
    vatomic32_t sequence;
    vrbtree_seq_node_t sentinel[2];
    vsize_t size[2];

    vrbtree_handle_node_t retire_fun;
    vrbtree_cmp_key_t cmp_fun;
    void *retire_fun_arg;
} vrbtree_t;

#define V_RBTREE_INIT(ptr, val)  vatomicptr_init(&ptr, val)
#define V_RBTREE_READ(ptr)       vatomicptr_read_rlx(&ptr)
#define V_RBTREE_WRITE(ptr, val) vatomicptr_write_rlx(&ptr, val)
#define V_RBTREE_WRITE_PARENT(ptr, val)                                        \
    do {                                                                       \
        vrbtree_seq_node_t *__p = vatomicptr_read_rlx(&ptr);                   \
        if (__p) {                                                             \
            vatomicptr_write_rlx(&__p->parent, val);                           \
        }                                                                      \
    } while (0)
#define V_RBTREE_READ_CHILD_COLOR(ptr, dir)                                    \
    ({                                                                         \
        vrbtree_seq_node_t *__p = vatomicptr_read_rlx(&ptr->child[dir]);       \
        __p && __p->color;                                                     \
    })

#define V_RBTREE_OPPOSITE_CHILD(dir) ((dir) ^ 1U)
#define V_RBTREE_TREE_INDEX(seq)     ((seq)&1U)

static inline void
_vrbtree_seq_init(vrbtree_t *tree, vsize_t index)
{
    ASSERT(tree);
    V_RBTREE_INIT(tree->sentinel[index].parent, NULL);
    V_RBTREE_INIT(tree->sentinel[index].child[0], NULL);
    V_RBTREE_INIT(tree->sentinel[index].child[1], NULL);
    tree->sentinel[index].color = 0;
}

static inline void
_vrbtree_seq_destroy(vrbtree_t *tree, vsize_t index)
{
    ASSERT(tree);
    V_UNUSED(index);
    V_UNUSED(tree);
}

// In the following functions we traverse the tree, keeping pointer
// to the current node (curr), as well as some neighboring nodes like
// parent (par), grandparent (gpar), great-granparent (ggpar).

// Variable dir_X means to which child we should descend from node X in
// order to get to node X, and dir_X_opp is the opposite direction
// (i.e. par->child[dir_par] points to the current node, and
// par->child[dir_par_opp] points to its sibling.

static inline void
_vrbtree_seq_add_rotations(vrbtree_seq_node_t *curr, vrbtree_seq_node_t *par,
                           vrbtree_seq_node_t *gpar, vsize_t dir_par)
{
    vsize_t dir_gpar = par == V_RBTREE_READ(gpar->child[1]);
    if (par->color) {
        vrbtree_seq_node_t *ggpar = V_RBTREE_READ(gpar->parent);
        vsize_t dir_ggpar         = gpar == V_RBTREE_READ(ggpar->child[1]);
        vsize_t dir_par_opp       = V_RBTREE_OPPOSITE_CHILD(dir_par);
        if (dir_par == dir_gpar) {
            // case #1
            V_RBTREE_WRITE(par->parent, ggpar);
            V_RBTREE_WRITE(ggpar->child[dir_ggpar], par);
            V_RBTREE_WRITE_PARENT(par->child[dir_par_opp], gpar);
            vrbtree_seq_node_t *sibling =
                V_RBTREE_READ(par->child[dir_par_opp]);
            V_RBTREE_WRITE(gpar->child[dir_gpar], sibling);
            V_RBTREE_WRITE(gpar->parent, par);
            V_RBTREE_WRITE(par->child[dir_par_opp], gpar);
            par->color  = 0;
            gpar->color = 1;
        } else {
            // case #2
            V_RBTREE_WRITE(curr->parent, ggpar);
            V_RBTREE_WRITE(ggpar->child[dir_ggpar], curr);
            V_RBTREE_WRITE_PARENT(curr->child[dir_par_opp], par);
            vrbtree_seq_node_t *child_dir = V_RBTREE_READ(curr->child[dir_par]);
            vrbtree_seq_node_t *child_dir_opp =
                V_RBTREE_READ(curr->child[dir_par_opp]);
            V_RBTREE_WRITE(par->child[dir_par], child_dir_opp);
            V_RBTREE_WRITE_PARENT(curr->child[dir_par], gpar);
            V_RBTREE_WRITE(gpar->child[dir_par_opp], child_dir);
            V_RBTREE_WRITE(par->parent, curr);
            V_RBTREE_WRITE(curr->child[dir_par_opp], par);
            V_RBTREE_WRITE(gpar->parent, curr);
            V_RBTREE_WRITE(curr->child[dir_par], gpar);
            gpar->color = 1;
            curr->color = 0;
        }
    }
}

static inline vbool_t
_vrbtree_seq_add(vrbtree_t *tree, vsize_t index, vrbtree_key_t key,
                 vrbtree_seq_node_t *node)
{
    ASSERT(tree);
    vrbtree_seq_node_t *par  = &tree->sentinel[index];
    vsize_t dir_par          = 0;
    vrbtree_seq_node_t *curr = V_RBTREE_READ(par->child[dir_par]);

    // search

    while (curr) {
        vrbtree_node_t *out_curr =
            V_CONTAINER_OF(curr, vrbtree_node_t, node[index]);
        vint32_t res = tree->cmp_fun(out_curr, key);
        if (res == 0) {
            return false;
        }
        par     = curr;
        dir_par = res < 0;
        curr    = V_RBTREE_READ(curr->child[dir_par]);
    }

    V_RBTREE_WRITE(node->child[0], NULL);
    V_RBTREE_WRITE(node->child[1], NULL);

    // special case (empty tree)

    if (par == &tree->sentinel[index]) {
        V_RBTREE_WRITE(par->child[0], node);
        node->color = 0;
        V_RBTREE_WRITE(node->parent, par);
        return true;
    }

    vrbtree_seq_node_t *old_par = par;
    vsize_t old_dir_par         = dir_par;
    node->color                 = 1;
    V_RBTREE_WRITE(node->parent, par);
    curr = node;

    // recursive recolor

    vrbtree_seq_node_t *gpar = V_RBTREE_READ(par->parent);
    while (V_RBTREE_READ_CHILD_COLOR(gpar, 0) &&
           V_RBTREE_READ_CHILD_COLOR(gpar, 1)) {
        vrbtree_seq_node_t *gpar_child_0 = V_RBTREE_READ(gpar->child[0]);
        vrbtree_seq_node_t *gpar_child_1 = V_RBTREE_READ(gpar->child[1]);
        gpar_child_0->color              = 0;
        gpar_child_1->color              = 0;
        vrbtree_seq_node_t *ggpar        = V_RBTREE_READ(gpar->parent);
        if (ggpar == &tree->sentinel[index]) {
            break;
        }
        gpar->color = 1;
        curr        = gpar;
        par         = ggpar;
        dir_par     = curr == V_RBTREE_READ(par->child[1]);
        gpar        = V_RBTREE_READ(par->parent);
    }

    // update pointers (insert)

    V_RBTREE_WRITE(old_par->child[old_dir_par], node);

    // update pointers (rotations)

    _vrbtree_seq_add_rotations(curr, par, gpar, dir_par);

    return true;
}

static inline void
_vrbtree_free_node(vrbtree_t *tree, vsize_t index, vrbtree_seq_node_t *node)
{
    if (index == 1) {
        vrbtree_node_t *out_node =
            V_CONTAINER_OF(node, vrbtree_node_t, node[index]);
        tree->retire_fun(out_node, tree->retire_fun_arg);
    }
}

static inline void
_vrbtree_seq_remove_rotations_black_sibling(vrbtree_seq_node_t *par,
                                            vrbtree_seq_node_t *sibling,
                                            vsize_t dir_par)
{
    vsize_t dir_par_opp            = V_RBTREE_OPPOSITE_CHILD(dir_par);
    vrbtree_seq_node_t *gpar       = V_RBTREE_READ(par->parent);
    vsize_t dir_gpar               = par == V_RBTREE_READ(gpar->child[1]);
    vrbtree_seq_node_t *nephew     = V_RBTREE_READ(sibling->child[dir_par]);
    vrbtree_seq_node_t *nephew_opp = V_RBTREE_READ(sibling->child[dir_par_opp]);
    if (V_RBTREE_READ_CHILD_COLOR(sibling, dir_par_opp)) {
        // case #1
        nephew_opp->color = 0;
        sibling->color    = par->color;
        par->color        = 0;

        V_RBTREE_WRITE(gpar->child[dir_gpar], sibling);

        V_RBTREE_WRITE(par->parent, sibling);
        V_RBTREE_WRITE(par->child[dir_par_opp], nephew);

        V_RBTREE_WRITE_PARENT(sibling->child[dir_par], par);

        V_RBTREE_WRITE(sibling->parent, gpar);
        V_RBTREE_WRITE(sibling->child[dir_par], par);
    } else {
        // case #2
        nephew->color = par->color;
        par->color    = 0;

        V_RBTREE_WRITE(gpar->child[dir_gpar], nephew);

        V_RBTREE_WRITE_PARENT(nephew->child[dir_par], par);
        V_RBTREE_WRITE_PARENT(nephew->child[dir_par_opp], sibling);

        vrbtree_seq_node_t *gneph = V_RBTREE_READ(nephew->child[dir_par]);
        vrbtree_seq_node_t *gneph_opp =
            V_RBTREE_READ(nephew->child[dir_par_opp]);
        V_RBTREE_WRITE(par->parent, nephew);
        V_RBTREE_WRITE(par->child[dir_par_opp], gneph);
        V_RBTREE_WRITE(sibling->parent, nephew);
        V_RBTREE_WRITE(sibling->child[dir_par], gneph_opp);

        V_RBTREE_WRITE(nephew->parent, gpar);
        V_RBTREE_WRITE(nephew->child[dir_par], par);
        V_RBTREE_WRITE(nephew->child[dir_par_opp], sibling);
    }
}

static inline void
_vrbtree_seq_remove_rotations_red_sibling(vrbtree_seq_node_t *par,
                                          vrbtree_seq_node_t *sibling,
                                          vsize_t dir_par)
{
    vsize_t dir_par_opp        = V_RBTREE_OPPOSITE_CHILD(dir_par);
    vrbtree_seq_node_t *gpar   = V_RBTREE_READ(par->parent);
    vsize_t dir_gpar           = par == V_RBTREE_READ(gpar->child[1]);
    vrbtree_seq_node_t *nephew = V_RBTREE_READ(sibling->child[dir_par]);
    if (!V_RBTREE_READ_CHILD_COLOR(nephew, 0) &&
        !V_RBTREE_READ_CHILD_COLOR(nephew, 1)) {
        // case #3
        sibling->color = 0;
        nephew->color  = 1;

        V_RBTREE_WRITE(gpar->child[dir_gpar], sibling);

        V_RBTREE_WRITE(sibling->parent, gpar);
        V_RBTREE_WRITE(sibling->child[dir_par], par);

        V_RBTREE_WRITE(par->parent, sibling);
        V_RBTREE_WRITE(par->child[dir_par_opp], nephew);

        V_RBTREE_WRITE(nephew->parent, par);
    } else if (V_RBTREE_READ_CHILD_COLOR(nephew, dir_par_opp)) {
        // case #4
        vrbtree_seq_node_t *gneph = V_RBTREE_READ(nephew->child[dir_par]);
        vrbtree_seq_node_t *gneph_opp =
            V_RBTREE_READ(nephew->child[dir_par_opp]);
        nephew->color    = 1;
        sibling->color   = 0;
        gneph_opp->color = 0;

        V_RBTREE_WRITE(gpar->child[dir_gpar], sibling);

        V_RBTREE_WRITE(sibling->parent, gpar);

        V_RBTREE_WRITE(par->parent, nephew);
        V_RBTREE_WRITE(par->child[dir_par_opp], gneph);

        V_RBTREE_WRITE_PARENT(nephew->child[dir_par], par);

        V_RBTREE_WRITE(nephew->child[dir_par], par);
    } else {
        // case #5
        sibling->color = 0;

        vrbtree_seq_node_t *gneph = V_RBTREE_READ(nephew->child[dir_par]);

        V_RBTREE_WRITE(gpar->child[dir_gpar], sibling);

        V_RBTREE_WRITE(sibling->parent, gpar);
        V_RBTREE_WRITE(sibling->child[dir_par], gneph);

        V_RBTREE_WRITE_PARENT(gneph->child[dir_par], par);
        V_RBTREE_WRITE_PARENT(gneph->child[dir_par_opp], nephew);

        vrbtree_seq_node_t *ggneph = V_RBTREE_READ(gneph->child[dir_par]);
        vrbtree_seq_node_t *ggneph_opp =
            V_RBTREE_READ(gneph->child[dir_par_opp]);
        V_RBTREE_WRITE(par->parent, gneph);
        V_RBTREE_WRITE(par->child[dir_par_opp], ggneph);
        V_RBTREE_WRITE(nephew->parent, gneph);
        V_RBTREE_WRITE(nephew->child[dir_par], ggneph_opp);

        V_RBTREE_WRITE(gneph->parent, sibling);
        V_RBTREE_WRITE(gneph->child[dir_par], par);
        V_RBTREE_WRITE(gneph->child[dir_par_opp], nephew);
    }
}

static inline void
_vrbtree_seq_remove_rotations(vrbtree_seq_node_t *curr, vrbtree_seq_node_t *par,
                              vrbtree_seq_node_t *sibling, vsize_t dir_par)
{
    if (curr->color) {
        // case #0
        curr->color = 0;
    } else if (!sibling->color) {
        _vrbtree_seq_remove_rotations_black_sibling(par, sibling, dir_par);
    } else {
        _vrbtree_seq_remove_rotations_red_sibling(par, sibling, dir_par);
    }
}

static inline void
_vrbtree_seq_remove_unlink(vrbtree_t *tree, vsize_t index,
                           vrbtree_seq_node_t *old_curr,
                           vrbtree_seq_node_t *curr,
                           vrbtree_seq_node_t *replacement)
{
    vrbtree_seq_node_t *par = V_RBTREE_READ(curr->parent);
    vsize_t dir_par         = V_RBTREE_READ(par->child[1]) == curr;
    V_RBTREE_WRITE(par->child[dir_par], replacement);

    if (old_curr != curr) {
        vrbtree_seq_node_t *old_par = V_RBTREE_READ(old_curr->parent);
        V_RBTREE_WRITE(curr->parent, old_par);
        curr->color = old_curr->color;

        vrbtree_seq_node_t *old_child_0 = V_RBTREE_READ(old_curr->child[0]);
        vrbtree_seq_node_t *old_child_1 = V_RBTREE_READ(old_curr->child[1]);
        V_RBTREE_WRITE(curr->child[0], old_child_0);
        V_RBTREE_WRITE(curr->child[1], old_child_1);

        vsize_t old_dir_par = V_RBTREE_READ(old_par->child[1]) == old_curr;
        V_RBTREE_WRITE(old_par->child[old_dir_par], curr);
        V_RBTREE_WRITE_PARENT(old_curr->child[0], curr);
        V_RBTREE_WRITE_PARENT(old_curr->child[1], curr);
    }
    _vrbtree_free_node(tree, index, old_curr);
}

static inline vbool_t
_vrbtree_seq_remove(vrbtree_t *tree, vsize_t index, vrbtree_key_t key)
{
    ASSERT(tree);
    vrbtree_seq_node_t *par  = &tree->sentinel[index];
    vsize_t dir_par          = 0;
    vrbtree_seq_node_t *curr = V_RBTREE_READ(par->child[dir_par]);

    // search

    while (curr) {
        vrbtree_node_t *out_curr =
            V_CONTAINER_OF(curr, vrbtree_node_t, node[index]);
        vint32_t res = tree->cmp_fun(out_curr, key);
        if (res == 0) {
            break;
        }
        par     = curr;
        dir_par = res < 0;
        curr    = V_RBTREE_READ(curr->child[dir_par]);
    }

    // special case (not found)

    if (!curr) {
        return false;
    }

    // special case (single node)

    vrbtree_seq_node_t *child_0 = V_RBTREE_READ(curr->child[0]);
    vrbtree_seq_node_t *child_1 = V_RBTREE_READ(curr->child[1]);

    if (par == &tree->sentinel[index] && !child_0 && !child_1) {
        V_RBTREE_WRITE(par->child[0], NULL);
        _vrbtree_free_node(tree, index, curr);
        return true;
    }

    // special case (with red child)

    vrbtree_seq_node_t *old_curr = curr;

    if (child_0) {
        if (!child_1) {
            child_0->color = 0;
            V_RBTREE_WRITE(child_0->parent, par);
            V_RBTREE_WRITE(par->child[dir_par], child_0);
            _vrbtree_free_node(tree, index, curr);
            return true;
        }

        par     = curr;
        curr    = child_1;
        dir_par = 1;
        while ((child_0 = V_RBTREE_READ(curr->child[0]))) {
            par     = curr;
            curr    = child_0;
            dir_par = 0;
        }
    }

    child_1 = V_RBTREE_READ(curr->child[1]);
    if (child_1) {
        child_1->color = 0;
        V_RBTREE_WRITE(child_1->parent, par);
        _vrbtree_seq_remove_unlink(tree, index, old_curr, curr, child_1);
        return true;
    }

    // Node old_curr is the one containing the key, and will be physically
    // removed, and node real_curr is the leaf successor that will be logically
    // removed.
    vrbtree_seq_node_t *real_curr = curr;

    // recursive recolor

    vsize_t dir_par_opp         = V_RBTREE_OPPOSITE_CHILD(dir_par);
    vrbtree_seq_node_t *sibling = V_RBTREE_READ(par->child[dir_par_opp]);
    vbool_t need_rotation       = true;
    while (!curr->color && !sibling->color &&
           !V_RBTREE_READ_CHILD_COLOR(sibling, 0) &&
           !V_RBTREE_READ_CHILD_COLOR(sibling, 1)) {
        sibling->color = 1;
        curr           = par;
        par            = V_RBTREE_READ(par->parent);
        if (par == &tree->sentinel[index]) {
            need_rotation = false;
            break;
        }
        dir_par     = curr == V_RBTREE_READ(par->child[1]);
        dir_par_opp = V_RBTREE_OPPOSITE_CHILD(dir_par);
        sibling     = V_RBTREE_READ(par->child[dir_par_opp]);
    }

    // update pointers (rotations)

    if (need_rotation) {
        _vrbtree_seq_remove_rotations(curr, par, sibling, dir_par);
    }

    // update pointers (removal)

    _vrbtree_seq_remove_unlink(tree, index, old_curr, real_curr, NULL);

    return true;
}

static inline vrbtree_node_t *
_vrbtree_seq_get(vrbtree_t *tree, vsize_t index, vrbtree_key_t key)
{
    ASSERT(tree);
    vrbtree_seq_node_t *curr = V_RBTREE_READ(tree->sentinel[index].child[0]);

    while (curr) {
        vrbtree_node_t *out_curr =
            V_CONTAINER_OF(curr, vrbtree_node_t, node[index]);
        vint32_t res = tree->cmp_fun(out_curr, key);
        if (res == 0) {
            return out_curr;
        } else {
            curr = V_RBTREE_READ(curr->child[res < 0]);
        }
    }

    return NULL;
}
static inline vrbtree_node_t *
_vrbtree_seq_extreme(vrbtree_t *tree, vsize_t index, vsize_t dir)
{
    ASSERT(tree);
    vrbtree_seq_node_t *par  = &tree->sentinel[index];
    vrbtree_seq_node_t *curr = V_RBTREE_READ(par->child[0]);

    if (!curr) {
        return NULL;
    }

    while (curr) {
        par  = curr;
        curr = V_RBTREE_READ(curr->child[dir]);
    }

    vrbtree_node_t *out_par = V_CONTAINER_OF(par, vrbtree_node_t, node[index]);
    return out_par;
}

static inline vrbtree_node_t *
_vrbtree_seq_neighbour(vrbtree_t *tree, vsize_t index, vrbtree_key_t key,
                       vsize_t dir)
{
    ASSERT(tree);
    vrbtree_seq_node_t *par  = &tree->sentinel[index];
    vsize_t dir_par          = 0;
    vrbtree_seq_node_t *curr = V_RBTREE_READ(par->child[dir_par]);
    vsize_t dir_opp          = V_RBTREE_OPPOSITE_CHILD(dir);
    vbool_t found            = 0;

    if (!curr) {
        return NULL;
    }

    while (curr) {
        vrbtree_node_t *out_curr =
            V_CONTAINER_OF(curr, vrbtree_node_t, node[index]);
        vint32_t res = tree->cmp_fun(out_curr, key);
        if (res == 0) {
            par   = curr;
            found = 1;
            break;
        } else {
            par     = curr;
            dir_par = res < 0;
            curr    = V_RBTREE_READ(curr->child[dir_par]);
        }
    }

    if (found || dir_par != dir) {
        if (V_RBTREE_READ(par->child[dir_opp])) {
            curr = V_RBTREE_READ(par->child[dir_opp]);
            while (curr) {
                par  = curr;
                curr = V_RBTREE_READ(curr->child[dir]);
            }
        } else {
            curr = par;
            par  = V_RBTREE_READ(par->parent);
            while (par != &tree->sentinel[index] &&
                   V_RBTREE_READ(par->child[dir_opp]) == curr) {
                curr = par;
                par  = V_RBTREE_READ(par->parent);
            }

            if (par == &tree->sentinel[index]) {
                return NULL;
            }
        }
    }

    vrbtree_node_t *out_par = V_CONTAINER_OF(par, vrbtree_node_t, node[index]);
    return out_par;
}

/**
 * Initializes the given vrbtree_t object `tree`.
 *
 * @note must be called before threads access the rbtree.
 *
 * @param tree address of vrbtree_t object.
 * @param retire_fun address of the function used to retire removed nodes.
 * @param retire_fun_arg optional second parameter to pass to `retire_fun`.
 * @param cmp_fun address of the function used for comparing a key in
 * `vrbtree_node_t` object with a given key.
 */
static inline void
vrbtree_init(vrbtree_t *tree, vrbtree_handle_node_t retire_fun,
             void *retire_fun_arg, vrbtree_cmp_key_t cmp_fun)
{
    ASSERT(tree);
    ASSERT(retire_fun);
    ASSERT(cmp_fun);
    tree->retire_fun     = retire_fun;
    tree->retire_fun_arg = retire_fun_arg;
    tree->cmp_fun        = cmp_fun;

    vrbtree_lock_init(&tree->lock);
    vatomic32_init(&tree->sequence, 0);

    _vrbtree_seq_init(tree, 0);
    _vrbtree_seq_init(tree, 1);
    tree->size[0] = tree->size[1] = 0L;
}

/**
 * Destroys all the remaining nodes in the rbtree.
 *
 * @note call only after thread join, or after all threads finished accessing
 * the given vrbtree_t object.
 *
 * @param tree address of vrbtree_t object.
 */
static inline void
vrbtree_destroy(vrbtree_t *tree)
{
    ASSERT(tree);
    _vrbtree_visit(tree, tree->retire_fun, tree->retire_fun_arg);

    _vrbtree_seq_destroy(tree, 0);
    _vrbtree_seq_destroy(tree, 1);
}

/**
 * Inserts the given node into the rbtree.
 *
 * The node is only inserted if there is no other node associated with the given
 * key exists in the rbtree.
 *
 * @note if the operation fails, users can safely free or reuse the given node.
 *
 * @pre the key must be set in the given `node` before calling add.
 * @param tree address of vrbtree_t object.
 * @param key the key value that is used to identify the node.
 * @param node address of vrbtree_node_t object.
 * @return true operation succeeded, the given node has been added.
 * @return false operation failed, the given node has not been added, because
 * there exists another node associated with the given key.
 *
 * @note must be called inside SMR critical section.
 */
static inline vbool_t
vrbtree_add(vrbtree_t *tree, vrbtree_key_t key, vrbtree_node_t *node)
{
    ASSERT(tree);
    vrbtree_lock_acquire(&tree->lock);

    vatomic32_inc(&tree->sequence);
    vatomic_fence_rel();
    vuint32_t result_0 = _vrbtree_seq_add(tree, 0, key, &node->node[0]);
    tree->size[0] += result_0;

    vatomic32_inc(&tree->sequence);
    vatomic_fence_rel();
    vuint32_t result_1 = _vrbtree_seq_add(tree, 1, key, &node->node[1]);
    tree->size[1] += result_1;

    vrbtree_lock_release(&tree->lock);
    ASSERT(result_0 == result_1);
    return result_0;
}

/**
 * Removes the node associated with the given key from the rbtree.
 *
 * @note the registered retire callback will be called on the removed node.
 *
 * @param tree address of vrbtree_t object.
 * @param key the key identifying the node to remove.
 * @return true the node associated with the given key was removed successfully.
 * @return false could not find a node holding the given key to remove.
 *
 * @note must be called inside SMR critical section.
 */
static inline vbool_t
vrbtree_remove(vrbtree_t *tree, vrbtree_key_t key)
{
    ASSERT(tree);
    vrbtree_lock_acquire(&tree->lock);

    vatomic32_inc(&tree->sequence);
    vatomic_fence_rel();
    vuint32_t result_0 = _vrbtree_seq_remove(tree, 0, key);
    tree->size[0] -= result_0;

    vatomic32_inc(&tree->sequence);
    vatomic_fence_rel();
    vuint32_t result_1 = _vrbtree_seq_remove(tree, 1, key);
    tree->size[1] -= result_1;

    vrbtree_lock_release(&tree->lock);
    ASSERT(result_0 == result_1);
    return result_0;
}

/**
 * Looks for the rbtree node associated with the given key.
 *
 * @param tree address of vrbtree_t object.
 * @param key key value.
 * @return vrbtree_node_t* address of the rbtree node associated with the
 * given key.
 * @return NULL if no node associated with the given key was found.
 *
 * @note must be called inside SMR critical section.
 */
static inline vrbtree_node_t *
vrbtree_get(vrbtree_t *tree, vrbtree_key_t key)
{
    ASSERT(tree);

    vuint32_t seq          = 0;
    vrbtree_node_t *result = NULL;
    do {
        seq    = vatomic32_read(&tree->sequence);
        result = _vrbtree_seq_get(tree, V_RBTREE_TREE_INDEX(seq), key);
        vatomic_fence_acq();
    } while (seq != vatomic32_read(&tree->sequence));

    return result;
}

/**
 * Returns the number of nodes in the tree.
 *
 * @param tree address of vrbtree_t object.
 *
 * @return number of nodes in the tree.
 */
static inline vsize_t
vrbtree_size(vrbtree_t *tree)
{
    ASSERT(tree);

    vuint32_t seq = vatomic32_read(&tree->sequence);
    return tree->size[V_RBTREE_TREE_INDEX(seq)];
}

static inline vrbtree_node_t *
_vrbtree_extreme(vrbtree_t *tree, vsize_t dir)
{
    ASSERT(tree);

    vuint32_t seq          = 0;
    vrbtree_node_t *result = NULL;
    do {
        seq    = vatomic32_read(&tree->sequence);
        result = _vrbtree_seq_extreme(tree, V_RBTREE_TREE_INDEX(seq), dir);
        vatomic_fence_acq();
    } while (seq != vatomic32_read(&tree->sequence));

    return result;
}

/**
 * Returns the node of the smallest key in the tree.
 *
 * @param tree address of vrbtree_t object.
 *
 * @return address of the rbtree node associated with the
 * smallest key in the tree.
 * @return NULL if the tree is empty.
 *
 * @note must be called inside SMR critical section.
 */
static inline vrbtree_node_t *
vrbtree_first(vrbtree_t *tree)
{
    return _vrbtree_extreme(tree, 0);
}

/**
 * Returns the node of the greatest key in the tree.
 *
 * @param tree address of vrbtree_t object.
 *
 * @return address of the rbtree node associated with the
 * greatest key in the tree.
 * @return NULL if the tree is empty.
 *
 * @note must be called inside SMR critical section.
 */
static inline vrbtree_node_t *
vrbtree_last(vrbtree_t *tree)
{
    return _vrbtree_extreme(tree, 1U);
}

static inline vrbtree_node_t *
_vrbtree_neighbour(vrbtree_t *tree, vrbtree_key_t key, vsize_t dir)
{
    ASSERT(tree);

    vuint32_t seq          = 0;
    vrbtree_node_t *result = NULL;
    do {
        seq = vatomic32_read(&tree->sequence);
        result =
            _vrbtree_seq_neighbour(tree, V_RBTREE_TREE_INDEX(seq), key, dir);
        vatomic_fence_acq();
    } while (seq != vatomic32_read(&tree->sequence));

    return result;
}

/**
 * Looks for the rbtree node associated with the smallest key
 * greater than the given key.
 *
 * @param tree address of vrbtree_t object.
 * @param key key value.
 *
 * @return address of the rbtree node associated with the
 * successor key.
 * @return NULL if there is no successor.
 *
 * @note must be called inside SMR critical section.
 */
static inline vrbtree_node_t *
vrbtree_successor(vrbtree_t *tree, vrbtree_key_t key)
{
    return _vrbtree_neighbour(tree, key, 0);
}

/**
 * Looks for the rbtree node associated with the largest key
 * smaller than the given key.
 *
 * @param tree address of vrbtree_t object.
 * @param key key value.
 *
 * @return address of the rbtree node associated with the
 * predecessor key.
 * @return NULL if there is no predecessor.
 *
 * @note must be called inside SMR critical section.
 */
static inline vrbtree_node_t *
vrbtree_predecessor(vrbtree_t *tree, vrbtree_key_t key)
{
    return _vrbtree_neighbour(tree, key, 1U);
}
static inline void
_vrbtree_visit_node(vrbtree_seq_node_t *node, vrbtree_handle_node_t visitor,
                    void *arg, vsize_t idx)
{
    // TODO: implement with a loop
    if (node) {
        vrbtree_seq_node_t *left  = V_RBTREE_READ(node->child[0]);
        vrbtree_seq_node_t *right = V_RBTREE_READ(node->child[1]);
        _vrbtree_visit_node(left, visitor, arg, idx);
        vrbtree_node_t *container =
            V_CONTAINER_OF(node, vrbtree_node_t, node[idx]);
        visitor(container, arg);
        _vrbtree_visit_node(right, visitor, arg, idx);
    }
}
/**
 * Visits all nodes in the rbtree and calls `visitor` on them.
 *
 * @note call only after thread join, or after all threads finished accessing
 * the given vrbtree_t object.
 *
 * @param tree address of vrbtree_t object.
 * @param visitor address of the function to call on each node.
 * @param arg second argument of visitor.
 */
static inline void
_vrbtree_visit(vrbtree_t *tree, vrbtree_handle_node_t visitor, void *arg)
{
    ASSERT(tree);
    ASSERT(visitor);
    vsize_t idx              = 0;
    vrbtree_seq_node_t *root = V_RBTREE_READ(tree->sentinel[idx].child[0]);
    _vrbtree_visit_node(root, visitor, arg, idx);
}

#undef V_RBTREE_INIT
#undef V_RBTREE_READ
#undef V_RBTREE_WRITE
#undef V_RBTREE_WRITE_PARENT
#undef V_RBTREE_READ_CHILD_COLOR

#endif
