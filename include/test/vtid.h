/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_VITID
#define VSYNC_VITID

#include <vsync/common/assert.h>
#include <pthread.h>
#include <vsync/atomic.h>
#include <vsync/common/dbg.h>

#define VTID_NOT_ASSIGNED 0

vatomic32_t g_pqueue_tid_generator = VATOMIC_INIT(1);

__thread vuint32_t pqueue_tid = VTID_NOT_ASSIGNED;

static inline vuint32_t
vtid_get_thread_id(void)
{
    if (pqueue_tid == VTID_NOT_ASSIGNED) {
        pqueue_tid = vatomic32_get_inc_rlx(&g_pqueue_tid_generator);
        ASSERT(pqueue_tid != VTID_NOT_ASSIGNED);
    }
    return pqueue_tid;
}


#endif
