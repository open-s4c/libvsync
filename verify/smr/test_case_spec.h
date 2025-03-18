/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H

#include <test/smr/ismr.h>
#include <vsync/atomic.h>

/* checks the SMR against the specs */

vatomic32_t g_states[SMR_MAX_NTHREADS];

vbool_t r_states[SMR_MAX_NTHREADS];

#define STATE_ENTERED 1
#define STATE_EXITED  3

mock_node_t *node_to_retire = NULL;

void
record_state(void)
{
    for (vsize_t i = 0; i < SMR_MAX_NTHREADS; i++) {
        r_states[i] = vatomic32_read_rlx(&g_states[i]);
    }
}

void
confirm_state(mock_node_t *node)
{
    ASSERT(node == node_to_retire);

    /* all threads that were observed as in entered state must have been moved
     * to exited state by now */
    for (vsize_t i = 0; i < SMR_MAX_NTHREADS; i++) {
        if (r_states[i] == STATE_ENTERED) {
            ASSERT(vatomic32_read_rlx(&g_states[i]) == STATE_EXITED);
        }
    }
}

void
_reader(vsize_t tid)
{
    ismr_enter(tid);
    vatomic32_write_rlx(&g_states[tid], STATE_ENTERED);
    vatomic32_write_rlx(&g_states[tid], STATE_EXITED);
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
    record_state();
    ismr_retire(&node_to_retire->smr_node, mock_node_free, false);
    ismr_recycle(tid);
}

void
t2(vsize_t tid)
{
    _reader(tid);
}

void
post(void)
{
}
void
pre(void)
{
    node_to_retire = mock_node_alloc();
    /* g_notify is defined in mock_node.h */
    g_notify = confirm_state;
}
#endif
