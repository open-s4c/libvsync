/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TREESET_RB_COARSE
#define VSYNC_TREESET_RB_COARSE

/*******************************************************************************
 * @file treeset_rb_coarse.h
 * @ingroup linearizable
 * @brief This implementation of treeset uses balanced red-black tree
 * (RB) and coarse-grained locking.
 *
 * Refer to treeset_bst_coarse.h for more general information about treeset.
 *
 * @example
 * @include eg_treeset_rb_coarse.c
 ******************************************************************************/

#include <vsync/vtypes.h>
#include <vsync/common/assert.h>
#include <vsync/utils/alloc.h>
#include <vsync/map/internal/treeset/treeset_common.h>
#include <vsync/map/internal/treeset/treeset_lock.h>

typedef struct treeset_node_s {
    treeset_key_t key;
    vbool_t red;
    vbool_t external;
    struct treeset_node_s *child[2];
} treeset_node_t;

typedef struct treeset_s {
    lock_t coarse_lock;
    treeset_node_t head_sentinel;
    vmem_lib_t mem_lib;
} treeset_t;

#include <vsync/map/internal/treeset/treeset_alloc.h>

#define TREESET_REPLACE_HEADER
#define treeset_init     treeset_init_fine
#define treeset_destroy  treeset_destroy_fine
#define treeset_add      treeset_add_fine
#define treeset_remove   treeset_remove_fine
#define treeset_contains treeset_contains_fine
#define treeset_visit    treeset_visit_fine
#define l_init(l)
#define l_destroy(l)
#define l_acquire(l)
#define l_release(l)
#define l_reader_acquire(l)
#define l_reader_release(l)

#include <vsync/map/treeset_rb_fine.h>

#undef TREESET_REPLACE_HEADER
#undef treeset_init
#undef treeset_destroy
#undef treeset_add
#undef treeset_remove
#undef treeset_contains
#undef treeset_visit
#undef l_init
#undef l_destroy
#undef l_acquire
#undef l_release
#undef l_reader_acquire
#undef l_reader_release

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
    l_init(&tree->coarse_lock);
    treeset_init_fine(tree, mem_lib);
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
    l_destroy(&tree->coarse_lock);
    treeset_destroy_fine(tree);
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
    l_acquire(&tree->coarse_lock);
    vbool_t result = treeset_add_fine(tree, key, value, out_value);
    l_release(&tree->coarse_lock);
    return result;
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
    l_acquire(&tree->coarse_lock);
    vbool_t result = treeset_remove_fine(tree, key, out_value);
    l_release(&tree->coarse_lock);
    return result;
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
    l_reader_acquire(&tree->coarse_lock);
    vbool_t result = treeset_contains_fine(tree, key, out_value);
    l_reader_release(&tree->coarse_lock);
    return result;
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
    treeset_visit_fine(tree, visitor, arg);
}

#endif
