/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#define INIT_WITH_N_NODES 1
/**
 * check queue_empty()
 */

/**
 * for thread with tid = 0
 *
 */
void
t1(vsize_t tid)
{
    enq(tid, 2, 'A');
    void *data = deq(tid);
    free(data);
}

/**
 * for thread with tid = 1
 *
 */
int msg = 0;
void
t2(vsize_t tid)
{
    msg        = 1;
    void *data = deq(tid);
    free(data);
}
/**
 * for thread with tid = 2
 *
 */
void
t3(vsize_t tid)
{
    /* must imply both dequeues are done and thus the msg must be passed */
    if (empty(tid)) {
        ASSERT(msg == 1);
    } else {
        queue_clean(tid);
    }
}
void
verify(void)
{
    ASSERT(g_len == 0);
}
