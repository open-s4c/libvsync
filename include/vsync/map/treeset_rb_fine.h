/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TREESET_RB_FINE
#define VSYNC_TREESET_RB_FINE

/*******************************************************************************
 * @file treeset_rb_fine.h
 * @ingroup linearizable
 * @brief This implementation of treeset uses balanced red-black tree
 * (RB) and fine-grained locking.
 *
 * Refer to treeset_bst_coarse.h for more general information about treeset.
 *
 * @example
 * @include eg_treeset_rb_fine.c
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
    vbool_t red;
    vbool_t external;
    struct treeset_node_s *child[2];
} treeset_node_t;

typedef struct treeset_s {
    treeset_node_t head_sentinel;
    vmem_lib_t mem_lib;
} treeset_t;

    #include <vsync/map/internal/treeset/treeset_alloc.h>

#endif

static inline treeset_node_t *
_treeset_rebalance(treeset_t *tree, treeset_node_t *gg, treeset_node_t *g,
                   treeset_node_t *f, treeset_node_t *x, treeset_key_t key);

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
    tree->head_sentinel.red      = false;
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

    // x  = currently processed node (black)
    // f  = its father
    // g  = its grandfather
    // gg = its great-grandfather
    // y  = its child on the path
    // cX = index of X's child that lies on the path

    treeset_node_t *gg = NULL;
    treeset_node_t *g  = NULL;
    treeset_node_t *f  = &tree->head_sentinel;
    l_acquire(&f->lock);
    treeset_node_t *x = f->child[0];
    vbool_t f_red     = false;

    if (unlikely(!x)) {
        treeset_node_t *ext = _treeset_get_node(tree);
        ext->key            = key;
        ext->red            = false;
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
        const vbool_t Red[2] = {x->child[0]->red, x->child[1]->red};
        ASSERT(!x->red);

        if (Red[0] && Red[1]) {
            /* 4-node */
            x->child[0]->red = false;
            x->child[1]->red = false;
            x->red           = f != &tree->head_sentinel;

            if (f_red) {
                /* Case 4 */
                x = _treeset_rebalance(tree, gg, g, f, x, key);
            } else {
                /* Case 3 */
                l_release(&f->lock);
                const vsize_t cx  = x->key <= key;
                treeset_node_t *y = x->child[cx];
                l_acquire(&y->lock);
                l_release(&x->lock);
                x = y;
            }
            f_red = false;
        } else {
            if (f_red) {
                l_release(&gg->lock);
                l_release(&g->lock);
            }
            const vsize_t cx = x->key <= key;
            if (Red[cx]) {
                /* Case 2: 3-node bottom */
                gg = f;
                g  = x;
                x  = x->child[cx];
                l_acquire(&x->lock);
                f_red = true;
            } else {
                /* Case 1: 3-node top or 2-node */
                l_release(&f->lock);
                f_red = false;
            }
        }

        const vsize_t cx = x->key <= key;
        f                = x;
        x                = x->child[cx];
        l_acquire(&x->lock);
    }

    if (x->key == key) {
        if (f_red) {
            l_release(&gg->lock);
            l_release(&g->lock);
        }
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
    ext->red            = false;
    ext->external       = true;
    ext->child[0]       = value;
    ext->child[1]       = NULL;
    l_init(&ext->lock);

    treeset_node_t *mid = _treeset_get_node(tree);
    mid->red            = f != &tree->head_sentinel;
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
    l_release(&x->lock);

    if (f_red) {
        /* Case 6 */
        _treeset_rebalance(tree, gg, g, f, mid, key);
    } else {
        /* Case 5 */
        l_release(&f->lock);
        l_release(&mid->lock);
    }

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
    // b  = its sibling
    // bb = child of its sibling
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

    /* 2 or more elements */
    if (!x->child[0]->red && !x->child[1]->red) {
        /* root is 2-node */
        /* Assume f->red = true; */
        const vsize_t cx = x->key <= key;
        g                = f;
        f                = x;
        x                = x->child[cx];
        l_acquire(&x->lock);
    }

    while (!x->external) {
        const vbool_t Red[2] = {x->child[0]->red, x->child[1]->red};
        ASSERT(!x->red);

        const vsize_t cx = x->key <= key;
        if (Red[cx]) {
            /* Case 1: 4-node or 3-node bottom */
            if (g) {
                l_release(&g->lock);
            }
            l_release(&f->lock);
            g = x;
            f = x->child[cx];
            l_acquire(&f->lock);
        } else if (Red[!cx]) {
            /* Case 2: 3-node top */
            if (g) {
                l_release(&g->lock);
            }
            treeset_node_t *s = x->child[!cx];
            l_acquire(&s->lock);
            const vsize_t cf = (f != &tree->head_sentinel) && (f->key <= key);
            // rotate(x, s, !cx)
            x->child[!cx] = s->child[cx];
            s->child[cx]  = x;
            f->child[cf]  = s;
            s->red        = false;
            l_release(&f->lock);
            x->red = true;
            g      = s;
            f      = x;
        } else {
            ASSERT(g);
            ASSERT(f != &tree->head_sentinel);
            /* 2-node */
            const vsize_t cf  = f->key <= key;
            treeset_node_t *b = f->child[!cf];
            ASSERT(!b->external);
            ASSERT(!b->red);

            l_acquire(&b->lock);
            const vbool_t b_red[2] = {b->child[0]->red, b->child[1]->red};

            if (!b_red[0] && !b_red[1]) {
                /* Case 3: brother 2-node */
                l_release(&b->lock);
                f->red = false;
                x->red = true;
                b->red = true;
                l_release(&g->lock);
                g = f;
                f = x;
            } else if (b_red[cf]) {
                /* Case 4b: brother 4-node or same-dir 3-node */
                treeset_node_t *bb = b->child[cf];
                ASSERT(bb->red);
                l_acquire(&bb->lock);

                // rotate(b, bb, cf)
                b->child[cf]   = bb->child[!cf];
                bb->child[!cf] = b;

                l_release(&b->lock);

                // rotate(f, bb, !cf)
                f->child[!cf] = bb->child[cf];
                bb->child[cf] = f;

                const vsize_t cg =
                    (g != &tree->head_sentinel) && (g->key <= key);
                g->child[cg] = bb;
                f->red       = false;
                x->red       = true;
                bb->red      = g != &tree->head_sentinel;

                l_release(&g->lock);
                l_release(&bb->lock);
                g = f;
                f = x;
            } else {
                /* Case 4a: brother opposite-dir 3-node */
                treeset_node_t *bb = b->child[!cf];
                ASSERT(!bb->external);
                ASSERT(bb->red);

                // rotate(f, b, !cf)
                f->child[!cf] = b->child[cf];
                b->child[cf]  = f;

                const vsize_t cg =
                    (g != &tree->head_sentinel) && (g->key <= key);
                g->child[cg] = b;
                f->red       = false;
                x->red       = true;
                b->red       = g != &tree->head_sentinel;
                bb->red      = false;

                l_release(&g->lock);
                l_release(&b->lock);
                g = f;
                f = x;
            }
        }

        const vsize_t cf = f->key <= key;
        x                = f->child[cf];
        l_acquire(&x->lock);
    }

    ASSERT(g);

    /* x is external */
    if (x->key != key) {
        l_release(&g->lock);
        l_release(&f->lock);
        l_release(&x->lock);
        return false;
    }

    const vsize_t cg = (g != &tree->head_sentinel) && (g->key <= key);
    const vsize_t cf = f->key <= key;

    treeset_node_t *b = f->child[!cf];
    l_acquire(&b->lock);
    g->child[cg] = b;

    l_release(&g->lock);
    l_release(&b->lock);

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

static inline treeset_node_t *
_treeset_rebalance(treeset_t *tree, treeset_node_t *gg, treeset_node_t *g,
                   treeset_node_t *f, treeset_node_t *x, treeset_key_t key)
{
    // x  = currently processed node (black)
    // f  = its father
    // g  = its grandfather
    // gg = its great-grandfather
    // y  = its child on the path
    // cX = index of X's child that lies on the path

    const vsize_t cgg = (gg != &tree->head_sentinel) && (gg->key <= key);
    const vsize_t cg  = g->key <= key;
    const vsize_t cf  = f->key <= key;
    const vsize_t cx  = x->key <= key;

    treeset_node_t *y = x->child[cx];

    if (cg == cf) {
        /* Case a: single rotation */
        g->child[cg]   = f->child[!cg];
        f->child[!cg]  = g;
        gg->child[cgg] = f;
        f->red         = false;
        l_release(&gg->lock);
        g->red = true;
        l_release(&f->lock);
        l_release(&g->lock);
    } else {
        /* Case b: double rotation */
        g->child[cg]   = x->child[!cg];
        x->child[!cg]  = g;
        f->child[!cg]  = x->child[cg];
        x->child[cg]   = f;
        gg->child[cgg] = x;
        x->red         = false;
        l_release(&gg->lock);
        g->red = true;
        l_release(&x->lock);

        if (cx == cg) {
            l_release(&g->lock);
            x = f;
        } else {
            l_release(&f->lock);
            x = g;
        }
    }

    if (!y->external) {
        /* 4-node */
        l_acquire(&y->lock);
        l_release(&x->lock);
        x = y;
    } else {
        /* new */
        l_release(&x->lock);
        x = NULL;
    }

    return x;
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

static inline int
_treeset_verify_recursive(treeset_node_t *node, vbool_t f_red,
                          vbool_t unlimited_b, treeset_key_t limit_b,
                          vbool_t unlimited_e, treeset_key_t limit_e)
{
    vbool_t limit_b_ok = unlimited_b || limit_b <= node->key;
    vbool_t limit_e_ok = unlimited_e || node->key < limit_e;
    ASSERT(limit_b_ok && "key invariant broken");
    ASSERT(limit_e_ok && "key invariant broken");

    if (node->external) {
        ASSERT(!node->red && "external invariant broken");
        return 0;
    }
    int bh_le = _treeset_verify_recursive(
        node->child[0], node->red, unlimited_b, limit_b, false, node->key);
    int bh_ri = _treeset_verify_recursive(node->child[1], node->red, false,
                                          node->key, unlimited_e, limit_e);

    ASSERT(bh_le == bh_ri && "black invariant broken");
    ASSERT((!f_red || !node->red) && "red invariant broken");

    return bh_le + !node->red;
}

static inline void
_treeset_verify(treeset_t *tree)
{
    ASSERT(tree);

    treeset_node_t *root = tree->head_sentinel.child[0];
    if (root) {
        // if unlimited is true, limit is ignored
        _treeset_verify_recursive(root, true, true, 0, true, 0);
    }
}

#endif
