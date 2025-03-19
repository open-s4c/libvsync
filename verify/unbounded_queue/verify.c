/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef NTHREADS
    #define NTHREADS 3U
#endif
#include <test/queue/ub/queue_interface.h>

queue_t g_queue;
#define MAX_LEN 5U
vuint64_t g_final_state[MAX_LEN];
vsize_t g_len = 0;
void enq(vsize_t, vuint64_t k, char lbl);
data_t *deq(vsize_t);
void get_final_state_cb(void *qnode);
vbool_t empty(vsize_t);
#include <test/boilerplate/test_case.h>

int
main(void)
{
    tc();
    return 0;
}

void
init(void)
{
    queue_init(&g_queue);
#if defined(VQUEUE_UB_LF_RECYCLE) && defined(VSYNC_VERIFICATION)
    aligned_alloc_init(sizeof(queue_node_t));
#endif
}

void
fini(void)
{
    queue_destroy(&g_queue);
    ASSERT(vmem_no_leak());
}

void
enq(vsize_t tid, vuint64_t k, char lbl)
{
    queue_enq(tid, &g_queue, k, lbl);
}

data_t *
deq(vsize_t tid)
{
    return queue_deq(tid, &g_queue);
}

vbool_t
empty(vsize_t tid)
{
    return queue_empty(tid, &g_queue);
}

/**
 * exploits print to extract the final state
 *
 * @param qnode
 */
void
get_final_state_cb(void *data)
{
    ASSERT(data);
    data_t *node = data;
    ASSERT(g_len < MAX_LEN);
    g_final_state[g_len++] = node->key;
}
void
reg(vsize_t tid)
{
    queue_register(tid, &g_queue);
}
void
dereg(vsize_t tid)
{
    queue_deregister(tid, &g_queue);
}
