/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_SMR_LOCK_H
#define VSYNC_SMR_LOCK_H

#include <vsync/vtypes.h>
#include <vsync/common/assert.h>

typedef void (*smr_lock_fun_t)(void *);

typedef struct smr_rwlock_lib_s {
    smr_lock_fun_t read_acq;
    smr_lock_fun_t read_rel;
    smr_lock_fun_t write_acq;
    smr_lock_fun_t write_rel;
    void *arg;
} smr_rwlock_lib_t;

typedef struct smr_lock_lib_s {
    smr_lock_fun_t acq;
    smr_lock_fun_t rel;
    void *arg;
} smr_lock_lib_t;

static inline vbool_t
smr_rwlock_lib_is_set(smr_rwlock_lib_t *lock_lib)
{
    if (!lock_lib) {
        return false;
    }
    if (!lock_lib->read_acq) {
        return false;
    }
    if (!lock_lib->read_rel) {
        return false;
    }
    if (!lock_lib->write_acq) {
        return false;
    }
    if (!lock_lib->write_rel) {
        return false;
    }
    return true;
}

static inline vbool_t
smr_lock_lib_is_set(smr_lock_lib_t *lock_lib)
{
    if (!lock_lib) {
        return false;
    }
    if (!lock_lib->acq) {
        return false;
    }
    if (!lock_lib->rel) {
        return false;
    }
    return true;
}

/**
 * Copys from one struct to another.
 *
 * *des = *src.
 *
 * @note This function was created to overcome the issue of memset in genmc.
 *
 * @param des address of destination smr_rwlock_lib_t object.
 * @param src address of source smr_rwlock_lib_t object.
 */
static inline void
smr_rwlock_lib_copy(smr_rwlock_lib_t *des, smr_rwlock_lib_t *src)
{
    ASSERT(des);
    ASSERT(src);
#if defined(VSYNC_VERIFICATION)
    des->read_acq  = src->read_acq;
    des->read_rel  = src->read_rel;
    des->write_acq = src->write_acq;
    des->write_rel = src->write_rel;
    des->arg       = src->arg;
#else
    *des = *src;
#endif
}

#endif
