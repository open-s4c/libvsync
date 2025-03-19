/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_ISMR_EBR_H
#define VSYNC_ISMR_EBR_H

#include <vsync/smr/ebr.h>
#include <vsync/common/dbg.h>
#include <test/smr/mock_node.h>
#include <vsync/common/verify.h>
#include <stdio.h>
#include "thread_lock.h"

typedef vebr_thread_t ismr_thread_t;
#include <test/smr/thread_storage.h>

/* Global Vars */
vebr_t g_smr;
thread_lock_t g_ebr_lock;
smr_lock_lib_t lock        = LOCK_LIB_DEFAULT(&g_ebr_lock);
__thread vsize_t g_tls_tid = MAIN_TID;

/* ISMR wrapper implementations */
static inline void
ismr_enter(vsize_t tid)
{
    ASSERT(tid == g_tls_tid);
    vebr_enter(&g_smr, ismr_get_thread_obj(tid));
}

static inline void
ismr_exit(vsize_t tid)
{
    ASSERT(tid == g_tls_tid);
    vebr_exit(&g_smr, ismr_get_thread_obj(tid));
}

static inline void
ismr_reg(vsize_t tid)
{
    g_tls_tid = tid;
    vebr_register(&g_smr, ismr_get_thread_obj(tid));
}

static inline void
ismr_dereg(vsize_t tid)
{
    ASSERT(tid == g_tls_tid);
    vebr_deregister(&g_smr, ismr_get_thread_obj(tid));
    ismr_destroy_thread_obj(tid);
}

static inline void
ismr_init(void)
{
    thread_lock_init(&g_ebr_lock);
    vebr_init(&g_smr, lock);
    ismr_reg(MAIN_TID);
}

static inline void
ismr_destroy(void)
{
    ismr_dereg(MAIN_TID);
    vebr_destroy(&g_smr);
}

static inline void
ismr_retire(smr_node_t *n, smr_node_destroy_fun destroy_fun, vbool_t local)
{
    vebr_retire(&g_smr, ismr_get_thread_obj(g_tls_tid), n, destroy_fun, NULL);
    V_UNUSED(local);
}

static inline void
ismr_retire_with_arg(smr_node_t *node, smr_node_destroy_fun destroy_fun,
                     void *args)
{
    vebr_retire(&g_smr, ismr_get_thread_obj(g_tls_tid), node, destroy_fun,
                args);
}

static inline vsize_t
ismr_recycle(vsize_t tid)
{
    V_UNUSED(tid);
    return vebr_recycle(&g_smr);
}

static inline void
ismr_assume_epoch(vuint64_t epoch)
{
    verification_assume(vatomic64_read(&g_smr.epoch_global) == epoch);
}

static inline vbool_t
ismr_sync(vsize_t tid)
{
    vebr_sync(&g_smr);
    V_UNUSED(tid);
    return true;
}

static inline char *
ismr_get_name(void)
{
    return "vebr";
}

/* @note: only for verification */
static inline vuint64_t
_ismr_get_epoch(void)
{
    return vatomic64_read_rlx(&g_smr.epoch_global);
}
/* @note: only for verification */
static inline void
_ismr_inc_epoch(void)
{
    vatomic64_inc_rlx(&g_smr.epoch_global);
}
#endif
