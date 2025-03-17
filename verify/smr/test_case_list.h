/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H

#include <test/smr/ismr.h>
#include <vsync/atomic.h>

/* over simplified list simulation. Note: that example works because there is
 * only one writer who detaches nodes. Multiple writers (insert/remove) require
 * a design similar to listset this simplification is to focus on smr and make
 * reduce the time the model checker needs to complete the verification
 */
mock_node_t *g_head = NULL;

void
_reader(vsize_t tid)
{
    /* the reader needs to pass over the detached node to find the node with
     * owner 2, this can trigger access after free if the SMR is not working
     * probably */
    ismr_enter(tid);
    mock_node_t *curr = g_head;
    while (curr) {
        if (curr->owner == 2)
            break;
        curr = vatomicptr_read(&curr->next);
    }
    ismr_exit(tid);
}

void
t0(vsize_t tid)
{
    _reader(tid);
}

void
t1(vsize_t tid)
{
    ismr_enter(tid);
    mock_node_t *curr = vatomicptr_read(&g_head->next);
    mock_node_t *succ = vatomicptr_read(&curr->next);
    /* detach second node, -> no longer reachable from the root
    (lst_head)
     */
    ASSERT(curr->owner == 1);
    vatomicptr_write(&g_head->next, succ);
    /* retire the detached node */

    ismr_retire(&curr->smr_node, mock_node_free, false);
    ismr_exit(tid);
    ismr_recycle(tid);
}

void
t2(vsize_t tid)
{
    _reader(tid);
}

void
verify(void)
{
    mock_node_t *curr = g_head;
    mock_node_t *succ = NULL;
    mock_node_no_leak();
    while (curr) {
        succ = vatomicptr_read(&curr->next);
        mock_node_free(&curr->smr_node, NULL);
        curr = succ;
    }
}

void
prepare(void)
{
    vsize_t i = 0;

    mock_node_t *prev = NULL;
    mock_node_t *curr = NULL;

    for (i = 0; i < 3; i++) {
        curr        = mock_node_alloc();
        curr->owner = 2 - i;
        vatomicptr_write_rlx(&curr->next, prev);
        prev = curr;
    }
    g_head = curr;
}
#endif
