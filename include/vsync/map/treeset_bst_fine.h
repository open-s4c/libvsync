/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TREESET_BST_FINE
#define VSYNC_TREESET_BST_FINE

/*******************************************************************************
 * @file treeset_bst_fine.h
 * @ingroup linearizable
 * @brief This implementation of treeset uses unbalanced binary search tree
 * (BST) and fine-grained locking.
 *
 * Refer to treeset_bst_coarse.h for more general information about treeset.
 *
 * @example
 * @include eg_treeset_bst_fine.c
 ******************************************************************************/

#ifndef TREESET_REPLACE_HEADER

    #include <vsync/vtypes.h>
    #include <vsync/common/assert.h>
    #include <vsync/utils/alloc.h>
    #include <vsync/map/internal/treeset/treeset_common.h>
    #include <vsync/map/internal/treeset/treeset_lock.h>

typedef struct treeset_node_s {
    lock_t lock;
    treeset_key_t key;
    vbool_t external;
    struct treeset_node_s *child[2];
} treeset_node_t;

typedef struct treeset_s {
    treeset_node_t head_sentinel;
    vmem_lib_t mem_lib;
} treeset_t;

    #include <vsync/map/internal/treeset/treeset_alloc.h>

#endif

static inline void _treeset_putall(treeset_t *tree);

/**
 * Initializes the treeset.
 *
 * @note must be called before threads access the treeset.
 * @param tree address of the treeset_t object.
 * @param mem_lib object of type `vmem_lib_t` containing malloc/free functions
 * to allocate/free internal nodes.
 */
static inline void
treeset_init(treeset_t *tree, vmem_lib_t mem_lib)
{
    ASSERT(tree);
    ASSERT(vmem_lib_not_null(&mem_lib));
    tree->head_sentinel.key      = 0; // unused
    tree->head_sentinel.external = false;
    tree->head_sentinel.child[0] = NULL;
    tree->head_sentinel.child[1] = NULL;
    tree->mem_lib                = mem_lib;
    l_init(&tree->head_sentinel.lock);
}

/**
 * Destroys all the remaining nodes in the treeset.
 *
 * @note call only after thread join, or after all threads finished accessing
 * the treeset.
 * @param tree address of the treeset_t object.
 */
static inline void
treeset_destroy(treeset_t *tree)
{
    ASSERT(tree);
    _treeset_putall(tree);
    l_destroy(&tree->head_sentinel.lock);
}

/**
 * Attempts to insert an element with a given key and value into the treeset.
 *
 * @param tree address of the treeset_t object.
 * @param key the key to be inserted.
 * @param value value to be associated with inserted key.
 * @param out_value out parameter for the previous value associated with the
 * key.
 * @return true operation succeeded.
 * @return false operation failed, since the given key was already in the
 * treeset, in the `out_value` the value of this element is returned.
 */
static inline vbool_t
treeset_add(treeset_t *tree, treeset_key_t key, void *value, void **out_value)
{
    ASSERT(tree);

    // x  = currently processed node
    // f  = its father
    // cX = index of X's child that lies on the path

    treeset_node_t *f = &tree->head_sentinel;
    l_acquire(&f->lock);
    treeset_node_t *x = f->child[0];

    if (unlikely(!x)) {
        treeset_node_t *ext = _treeset_get_node(tree);
        ext->key            = key;
        ext->external       = true;
        ext->child[0]       = value;
        ext->child[1]       = NULL;
        l_init(&ext->lock);

        f->child[0] = ext;
        l_release(&f->lock);
        return true;
    }

    l_acquire(&x->lock);

    while (!x->external) {
        l_release(&f->lock);
        const vsize_t cx = x->key <= key;
        f                = x;
        x                = x->child[cx];
        l_acquire(&x->lock);
    }

    if (x->key == key) {
        l_release(&f->lock);
        if (out_value) {
            *out_value = x->child[0];
        }
        l_release(&x->lock);
        return false;
    }

    /* new */
    treeset_node_t *ext = _treeset_get_node(tree);
    ext->key            = key;
    ext->external       = true;
    ext->child[0]       = value;
    ext->child[1]       = NULL;
    l_init(&ext->lock);

    treeset_node_t *mid = _treeset_get_node(tree);
    mid->external       = false;
    l_init(&mid->lock);
    l_acquire(&mid->lock);

    if (x->key < key) {
        mid->key      = key;
        mid->child[0] = x;
        mid->child[1] = ext;
    } else {
        mid->key      = x->key;
        mid->child[0] = ext;
        mid->child[1] = x;
    }

    const vsize_t cf = (f != &tree->head_sentinel) && (f->key <= key);
    f->child[cf]     = mid;
    l_release(&f->lock);
    l_release(&mid->lock);
    l_release(&x->lock);
    return true;
}

/**
 * Attempts to remove an element with a given key from the treeset.
 *
 * @param tree address of the treeset_t object.
 * @param key the key to be removed.
 * @param out_value out parameter for the value associated with the key.
 * @return true operation succeeded, in the `out_value` the value of the removed
 * element is returned.
 * @return false operation failed, there is no element with the given key.
 */
static inline vbool_t
treeset_remove(treeset_t *tree, treeset_key_t key, void **out_value)
{
    ASSERT(tree);

    // x  = currently processed node (black)
    // f  = its father
    // g  = its grandfather
    // y  = its child
    // cX = index of X's child that lies on the path

    treeset_node_t *g = NULL;
    treeset_node_t *f = &tree->head_sentinel;
    l_acquire(&f->lock);
    treeset_node_t *x = f->child[0];

    /* 0 elements */
    if (unlikely(!x)) {
        l_release(&f->lock);
        return false;
    }
    l_acquire(&x->lock);

    /* 1 element */
    if (unlikely(x->external)) {
        if (x->key != key) {
            l_release(&f->lock);
            l_release(&x->lock);
            return false;
        }
        f->child[0] = NULL;
        l_release(&f->lock);
        if (out_value) {
            *out_value = x->child[0];
        }
        l_release(&x->lock);
        l_destroy(&x->lock);
        _treeset_put_node(tree, x);
        return true;
    }

    while (!x->external) {
        if (g) {
            l_release(&g->lock);
        }
        const vsize_t cx = x->key <= key;
        g                = f;
        f                = x;
        x                = x->child[cx];
        l_acquire(&x->lock);
    }

    /* x is external */
    if (x->key != key) {
        l_release(&g->lock);
        l_release(&f->lock);
        l_release(&x->lock);
        return false;
    }

    const vsize_t cg = (g != &tree->head_sentinel) && (g->key <= key);
    const vsize_t cf = f->key <= key;

    treeset_node_t *y = f->child[!cf];
    l_acquire(&y->lock);
    g->child[cg] = y;

    l_release(&g->lock);
    l_release(&y->lock);

    l_release(&f->lock);
    l_destroy(&f->lock);
    if (out_value) {
        *out_value = x->child[0];
    }
    l_release(&x->lock);
    l_destroy(&x->lock);

    _treeset_put_node(tree, f);
    _treeset_put_node(tree, x);

    return true;
}

/**
 * Searches the treeset for an element with a given key.
 *
 * @param tree address of the treeset_t object.
 * @param key the key to be searched for.
 * @param out_value out parameter for the value associated with the key.
 * @return true operation succeeded, in the `out_value` the value of the found
 * element is returned.
 * @return false operation failed, there is no element with the given key.
 */
static inline vbool_t
treeset_contains(treeset_t *tree, treeset_key_t key, void **out_value)
{
    ASSERT(tree);

    // x  = currently processed node (black)
    // f  = its father
    // cX = index of X's child that lies on the path

    treeset_node_t *f = &tree->head_sentinel;
    l_reader_acquire(&f->lock);
    treeset_node_t *x = f->child[0];

    if (unlikely(!x)) {
        l_reader_release(&f->lock);
        return false;
    }

    l_reader_acquire(&x->lock);
    l_reader_release(&f->lock);

    while (!x->external) {
        const vsize_t cx = x->key <= key;
        f                = x;
        x                = x->child[cx];
        l_reader_acquire(&x->lock);
        l_reader_release(&f->lock);
    }

    if (x->key != key) {
        l_reader_release(&x->lock);
        return false;
    }

    if (out_value) {
        *out_value = x->child[0];
    }
    l_reader_release(&x->lock);
    return true;
}

static inline void
_treeset_visit_recursive(treeset_node_t *node, treeset_visitor visitor,
                         void *arg)
{
    if (node->external) {
        visitor(node->key, node->child[0], arg);
    } else {
        _treeset_visit_recursive(node->child[0], visitor, arg);
        _treeset_visit_recursive(node->child[1], visitor, arg);
    }
}

/**
 * Visits all elements in the treeset.
 *
 * @note call only after thread join, or after all threads finished accessing
 * the treeset.
 * @param tree address of the treeset_t object.
 * @param visitor address of the function to call on each element.
 * @param arg the third argument to the visitor function.
 */
static inline void
treeset_visit(treeset_t *tree, treeset_visitor visitor, void *arg)
{
    ASSERT(tree);
    ASSERT(visitor);

    treeset_node_t *root = tree->head_sentinel.child[0];
    if (root) {
        _treeset_visit_recursive(root, visitor, arg);
    }
}

static inline void
_treeset_putall_recursive(treeset_t *tree, treeset_node_t *node)
{
    if (!node->external) {
        _treeset_putall_recursive(tree, node->child[0]);
        _treeset_putall_recursive(tree, node->child[1]);
    }
    l_destroy(&node->lock);
    _treeset_put_node(tree, node);
}

static inline void
_treeset_putall(treeset_t *tree)
{
    ASSERT(tree);

    treeset_node_t *root = tree->head_sentinel.child[0];
    if (root) {
        _treeset_putall_recursive(tree, root);
    }
}

static inline void
_treeset_verify_recursive(treeset_node_t *node, vbool_t unlimited_b,
                          treeset_key_t limit_b, vbool_t unlimited_e,
                          treeset_key_t limit_e)
{
    vbool_t limit_b_ok = unlimited_b || limit_b <= node->key;
    vbool_t limit_e_ok = unlimited_e || node->key < limit_e;
    ASSERT(limit_b_ok && "key invariant broken");
    ASSERT(limit_e_ok && "key invariant broken");

    if (!node->external) {
        _treeset_verify_recursive(node->child[0], unlimited_b, limit_b, false,
                                  node->key);
        _treeset_verify_recursive(node->child[1], false, node->key, unlimited_e,
                                  limit_e);
    }
}

static inline void
_treeset_verify(treeset_t *tree)
{
    ASSERT(tree);

    treeset_node_t *root = tree->head_sentinel.child[0];
    if (root) {
        // if unlimited is true, limit is ignored
        _treeset_verify_recursive(root, true, 0, true, 0);
    }
}

#endif
