/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#ifndef VSYNC_SMR_TEST_LOCK_H
#define VSYNC_SMR_TEST_LOCK_H

typedef pthread_mutex_t thread_lock_t;

static inline void
thread_lock_init(void *arg)
{
    pthread_mutex_t *lock = (pthread_mutex_t *)arg;
    int ret               = pthread_mutex_init(lock, NULL);
    ASSERT(ret == 0);
    V_UNUSED(ret);
}

static inline void
thread_lock_acq(void *arg)
{
    pthread_mutex_t *lock = (pthread_mutex_t *)arg;
    int ret               = pthread_mutex_lock(lock);
    ASSERT(ret == 0);
    V_UNUSED(ret);
}
static inline void
thread_lock_rel(void *arg)
{
    pthread_mutex_t *lock = (pthread_mutex_t *)arg;
    int ret               = pthread_mutex_unlock(lock);
    ASSERT(ret == 0);
    V_UNUSED(ret);
}

#define LOCK_LIB_DEFAULT(_l_)                                                  \
    (smr_lock_lib_t)                                                           \
    {                                                                          \
        thread_lock_acq, thread_lock_rel, _l_                                  \
    }

#endif
