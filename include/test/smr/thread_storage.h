/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VTHREAD_STORAGE_H
#define VTHREAD_STORAGE_H

// should be included after the typedef of ismr_thread_t

#if defined(SMR_NONE)
static inline void *
ismr_get_thread_obj(vsize_t tid)
{
    V_UNUSED(tid);
    return NULL;
}
static inline void
ismr_destroy_thread_obj(vsize_t tid)
{
    V_UNUSED(tid);
}
#elif defined(ISMR_USE_TLS) && !defined(VSYNC_VERIFICATION)
    #include <malloc.h>
    #define MAIN_TID 0
static pthread_key_t g_key;
static pthread_once_t g_key_once = PTHREAD_ONCE_INIT;

static inline void
_ismr_destruct_callback(void *ptr)
{
    free(ptr);
}

static inline void
_ismr_init_key(void)
{
    pthread_key_create(&g_key, _ismr_destruct_callback);
}

static inline ismr_thread_t *
ismr_get_thread_obj(vsize_t tid)
{
    ismr_thread_t *ptr = NULL;
    int ret            = 0;

    V_UNUSED(tid);

    ret = pthread_once(&g_key_once, _ismr_init_key);
    ASSERT(ret == 0);

    if ((ptr = pthread_getspecific(g_key)) == NULL) {
        ptr = memalign(VSYNC_CACHELINE_SIZE, sizeof(ismr_thread_t));
        ASSERT(ptr);
        ret = pthread_setspecific(g_key, ptr);
        ASSERT(ret == 0);
    }
    V_UNUSED(ret);
    return ptr;
}
static inline void
ismr_destroy_thread_obj(vsize_t tid)
{
    V_UNUSED(tid);
}
#else
    #ifndef SMR_MAX_NTHREADS
        #error neither ISMR_USE_TLS nor SMR_MAX_THREADS is defined
    #endif
    #define ISMR_ARR_LEN (SMR_MAX_NTHREADS + 1)
    #define MAIN_TID     SMR_MAX_NTHREADS

    #if defined(VSYNC_VERIFICATION_DBG)
ismr_thread_t g_smr_threads[ISMR_ARR_LEN];
    #else
ismr_thread_t *g_smr_threads[ISMR_ARR_LEN];
    #endif

static inline ismr_thread_t *
ismr_get_thread_obj(vsize_t tid)
{
    ASSERT(tid < ISMR_ARR_LEN);
    #if defined(VSYNC_VERIFICATION_DBG)
    return &g_smr_threads[tid];
    #else
    if (g_smr_threads[tid] == NULL) {
        g_smr_threads[tid] = malloc(sizeof(ismr_thread_t));
        ASSERT(g_smr_threads[tid]);
    }
    return g_smr_threads[tid];
    #endif
}

static inline void
ismr_destroy_thread_obj(vsize_t tid)
{
    #if !defined(VSYNC_VERIFICATION_DBG)
    free(g_smr_threads[tid]);
    g_smr_threads[tid] = NULL;
    #endif
}
    #undef ISMR_ARR_LEN

#endif

#endif
