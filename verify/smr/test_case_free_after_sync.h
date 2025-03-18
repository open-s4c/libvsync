/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H

#undef SMR_MAX_NTHREADS
#define SMR_MAX_NTHREADS 2
#include <test/smr/ismr.h>

#undef TRIGGER_BUG_ON_HW
#ifdef TRIGGER_BUG_ON_HW
    #include <unistd.h>
#endif

#if defined(SMR_KCLEANUP)
mock_node_t *g_node = NULL;
#endif
/* GOAL: provide the sync. guarantees HM requires */
vatomicptr_t p;

void
t0(vsize_t tid)
{
    mock_node_t *node = vatomicptr_read_rlx(&p);
    ASSERT(node);
    ASSERT(node->x == 10);
#ifdef TRIGGER_BUG_ON_HW
    usleep(199);
#endif
    vatomicptr_write_rlx(&p, NULL); // detach + retire

    vbool_t success = ismr_sync(tid);

#if defined(SMR_KCLEANUP)
    g_node = node;
#endif
    if (success) {
        mock_node_free(&node->smr_node, NULL);
    }
}

void
t1(vsize_t tid)
{
    ismr_enter(tid);
    mock_node_t *node = vatomicptr_read_rlx(&p);
#ifdef TRIGGER_BUG_ON_HW
    sleep(1);
#endif
    if (node) {
        ASSERT(node->x == 10);
    }
    ismr_exit(tid);
}

void
t2(vsize_t tid)
{
    V_UNUSED(tid);
}

void
post(void)
{
#if defined(SMR_KCLEANUP)
    // kcleanup does not implement sync so this test case is not fully accurate
    // for it
    vmem_free(g_node);
#endif
}
void
pre(void)
{
    mock_node_t *node = mock_node_alloc();
    node->x           = 10;
    vatomicptr_write_rlx(&p, node);
}
#endif
