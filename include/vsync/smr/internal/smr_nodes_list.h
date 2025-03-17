/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSMR_NODES_LIST_H
#define VSMR_NODES_LIST_H

#include <vsync/common/assert.h>
#include <vsync/vtypes.h>
#include <vsync/atomic.h>
#include <vsync/common/assert.h>
#include <vsync/smr/internal/smr_node.h>
#include <vsync/common/verify.h>

typedef struct smr_nodes_list_s {
    vatomicptr(void *) list;
    vatomic64_t count;
} smr_nodes_list_t;

#define SMR_NODES_LIST_INIT()                                                  \
    {                                                                          \
        .list = VATOMIC_INIT(NULL), .count = VATOMIC_INIT(0)                   \
    }

/**
 * Initializes the list.
 *
 * @param lst address of smr_nodes_list_t object
 */
static inline void
smr_nodes_list_init(smr_nodes_list_t *lst)
{
    ASSERT(lst);
    vatomicptr_init(&lst->list, NULL);
    vatomic64_init(&lst->count, 0);
}
/**
 * Checks if the list is empty.
 *
 * @param lst address of smr_nodes_list_t object
 * @return true the list is empty
 * @return false  the list has at least one node
 */
static inline vbool_t
smr_nodes_list_is_empty(smr_nodes_list_t *lst)
{
    ASSERT(lst);
    return (vatomicptr_read(&lst->list) == NULL);
}
/**
 * Returns the count of the nodes in the list.
 *
 * @note the count may not be always accurate
 * @param lst address of smr_nodes_list_t object
 * @return vuint64_t approximately count of nodes in the list
 */
static inline vuint64_t
smr_nodes_list_get_length(smr_nodes_list_t *lst)
{
    return vatomic64_read_rlx(&lst->count);
}
/**
 * Connects the given list `new_head`->...->`connector` to `lst`.
 *
 * @note to connect only one node pass `new_head = tail`
 * @param lst address of smr_nodes_list_t object.
 * @param tail address of smr_node_t object.
 * @param head address of smr_node_t object.
 */
static inline void
_smr_nodes_list_connect(smr_nodes_list_t *lst, smr_node_core_t *head,
                        smr_node_core_t *tail)
{
    void *old = NULL;
    void *cmp = NULL;

    ASSERT(lst);
    ASSERT(head);
    ASSERT(tail);

    /*
     * <node*> = zero or more nodes
     * Desired outcome:
     * new_head -> <node*> -> tail -> old_head  -> <node*>
     * if new_head is the same as tail then:
     * new_head -> old_head -> <node*>
     */

    old = vatomicptr_read(&lst->list);
    do {
        cmp        = old;
        tail->next = cmp;
        old        = vatomicptr_cmpxchg_rel(&lst->list, cmp, head);
        verification_assume(old == cmp);
    } while (cmp != old);
}
/**
 * Adds `node` to the head of the list.
 *
 * @param lst address of smr_nodes_list_t object.
 * @param node address of smr_node_t object.
 * @param destroy_fun address of destructor function.
 * @param destroy_fun_arg extra argument passed to `destroy_fun`.
 */
static inline void
smr_nodes_list_add(smr_nodes_list_t *lst, smr_node_t *node,
                   smr_node_destroy_fun destroy_fun, void *destroy_fun_arg)
{
    ASSERT(lst);
    ASSERT(node);
    ASSERT(destroy_fun);

    node->destroy_fun     = destroy_fun;
    node->destroy_fun_arg = destroy_fun_arg;
    node->core.next       = NULL;

    _smr_nodes_list_connect(lst, &node->core, &node->core);
    vatomic64_inc_rlx(&lst->count);
}
/**
 * Adds `node` to the head of the list.
 *
 * @param lst address of smr_nodes_list_t object.
 * @param node address of smr_node_t object;
 * @param type  value to set on type field.
 * @param destroy_fun address of destructor function.
 * @param destroy_fun_arg extra argument passed to `destroy_fun`.
 * @note This function will retire after `kcleanup.h` refactor.
 */
static inline void
smr_nodes_list_add_with_type(smr_nodes_list_t *lst, smr_node_t *node, int type,
                             smr_node_destroy_fun destroy_fun,
                             void *destroy_fun_arg)
{
    ASSERT(lst);
    ASSERT(node);
    ASSERT(destroy_fun);

    node->type            = type;
    node->core.next       = NULL;
    node->destroy_fun     = destroy_fun;
    node->destroy_fun_arg = destroy_fun_arg;

    _smr_nodes_list_connect(lst, &node->core, &node->core);
    vatomic64_inc_rlx(&lst->count);
}
/**
 * Merges the given `head->...->tail` list to `lst`.
 *
 * @param lst address of smr_nodes_list_t object. The list to merge into.
 * @param head address of smr_node_t object. The head of the list to merge.
 * @param tail address of smr_node_t object. The tail of the list to merge.
 * @param count length of the list to merge.
 */
static inline void
smr_nodes_list_merge(smr_nodes_list_t *lst, smr_node_t *head, smr_node_t *tail,
                     vuint64_t count)
{
    ASSERT(lst);
    ASSERT(head);
    ASSERT(tail);
    ASSERT(tail->core.next == NULL);

    _smr_nodes_list_connect(lst, &head->core, &tail->core);
    vatomic64_add_rlx(&lst->count, count);
}
/**
 * Empties the list, and returns the head.
 *
 * @param lst address of smr_nodes_list_t object
 * @return address of smr_node_t object
 */
static inline smr_node_t *
smr_nodes_list_get_and_empty(smr_nodes_list_t *lst)
{
    smr_node_t *node = vatomicptr_xchg_acq(&lst->list, NULL);
    vatomic64_write_rlx(&lst->count, 0);
    return node;
}
/**
 * Calls each nodes destructor starting from head node.
 *
 * @param head address of smr_node_t object. The head of the list to destroy.
 * @return vsize_t count of destroyed objects.
 */
static inline vsize_t
smr_nodes_list_destroy(smr_node_t *head)
{
    vsize_t count          = 0;
    smr_node_t *curr       = head;
    smr_node_t *freed_node = NULL;

    while (curr) {
        freed_node = curr;
        curr       = (smr_node_t *)curr->core.next;
        ASSERT(freed_node->destroy_fun);
        freed_node->destroy_fun(freed_node, freed_node->destroy_fun_arg);
        count++;
    }
    return count;
}
#endif
