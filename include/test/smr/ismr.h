/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_SMR_INTERFACE_H
#define VSYNC_SMR_INTERFACE_H

#include <pthread.h>
#include <vsync/smr/internal/smr_node.h>

/**
 * @note If registration of threads is required, then this function must
 * register the main thread.
 *
 */
static inline void ismr_init(void);
static inline void ismr_reg(vsize_t tid);
static inline void ismr_dereg(vsize_t tid);
static inline void ismr_enter(vsize_t tid);
static inline void ismr_exit(vsize_t tid);
static inline vbool_t ismr_sync(vsize_t tid);
static inline vsize_t ismr_recycle(vsize_t tid);
static inline void ismr_retire(smr_node_t *n, smr_node_destroy_fun destroy_fun,
                               vbool_t local);
/**
 * @note If registration of threads is required, then this function must
 * deregister the main thread.
 *
 */
static inline void ismr_retire(smr_node_t *n, smr_node_destroy_fun destroy_fun,
                               vbool_t local);
static inline void ismr_destroy(void);


#if defined(VSYNC_SMR_NOT_AVAILABLE) || defined(SMR_NONE)
    #include <test/smr/ismr_none.h>
#elif defined(SMR_CLEANUP)
    #include <test/smr/ismr_cleanup.h>
#elif defined(SMR_KCLEANUP)
    #include <test/smr/ismr_kcleanup.h>
#elif defined(SMR_EBR)
    #include <test/smr/ismr_ebr.h>
#elif defined(SMR_GUS)
    #include <test/smr/ismr_gus.h>
#elif defined(SMR_CEBR) || defined(SMR_CEBR_ALT)
    #include <test/smr/ismr_cebr.h>
#elif defined(SMR_GDUMP_LT)
    #include <test/smr/ismr_gdump_lt.h>
#else
    // can be multiple
    #include <test/smr/ismr_gdump.h>
#endif

#if !defined(MAIN_TID)
    #error "MAIN_TID must be defined by ismr implementation"
#endif

size_t g_cycles            = 0;
size_t g_freed_nodes       = 0;
vatomic32_t g_stop_cleaner = VATOMIC_INIT(0);
pthread_t g_cleaner;

#if !defined(CLEANER_TID)
    #define CLEANER_TID 0U
#endif

void *
ismr_cleaner(void *args)
{
    g_freed_nodes = 0;
    g_cycles      = 0;
    while (vatomic32_read(&g_stop_cleaner) != 1) {
        g_freed_nodes += ismr_recycle(CLEANER_TID);
        g_cycles++;
    }
    ismr_recycle(CLEANER_TID);
    V_UNUSED(args);
    return NULL;
}

static inline void
ismr_start_cleaner(void)
{
#if !defined(SMR_NONE)
    pthread_create(&g_cleaner, NULL, ismr_cleaner, NULL);
#endif
}

static inline void
ismr_stop_cleaner(void)
{
#if !defined(SMR_NONE)
    vatomic32_write(&g_stop_cleaner, 1);
    pthread_join(g_cleaner, NULL);
#endif
}
#endif
