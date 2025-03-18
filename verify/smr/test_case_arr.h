/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H

#include <test/smr/ismr.h>
#include <vsync/atomic.h>

#define ARR_LEN 2

/* array of pointers */
mock_node_t *g_arr[ARR_LEN];

/* whenever a pointer from the array is retired the index advances (simulates
 * detachment ) */
vatomic32_t g_root = VATOMIC_INIT(0);

/*
 * a reader thread will scan the available elements looking for something
 * matching the target. If the SMR is unsafe it might tree to free a node while
 * the reader is working on it and this will trigger access to freed memory in
 * the model checker
 */
void
_reader(vsize_t tid, vsize_t target)
{
    vsize_t idx = 0;
    ismr_enter(tid);
    idx = vatomic32_read(&g_root);
    for (vsize_t i = idx; i < ARR_LEN; i++) {
        /* this will trigger access to freed memory if the smr is buggy */
        if (g_arr[i]->owner == target) {
            break;
        }
    }
    ismr_exit(tid);
}

void
t0(vsize_t tid)
{
    _reader(tid, ARR_LEN - 1);
}

void
t1(vsize_t tid)
{
    /* a writer thread will detach (advance the root) and retire the elements
     * one after the other */
    for (vsize_t i = 0; i < ARR_LEN; i++) {
        ismr_enter(tid);
        vatomic32_write(&g_root, i + 1);
        ismr_retire(&g_arr[i]->smr_node, mock_node_free, false);
        ismr_exit(tid);
        ismr_recycle(tid);
    }
}

void
t2(vsize_t tid)
{
#if defined(SMR_CLEANUP) ||                                                    \
    (defined(VSYNC_VERIFICATION_QUICK) &&                                      \
     (defined(VSYNC_VERIFICATION_IMM) || (SMR_GDUMPV2)))
    V_UNUSED(tid);
#else
    _reader(tid, 0);
#endif
}

void
post(void)
{
}
void
pre(void)
{
    vsize_t i = 0;

    mock_node_t *curr = NULL;
    for (i = 0; i < ARR_LEN; i++) {
        curr        = mock_node_alloc();
        curr->owner = i;
        g_arr[i]    = curr;
    }
}
#endif
