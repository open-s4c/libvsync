/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef IREF_HASHTABLE
#define IREF_HASHTABLE

/**
 * !DISCLAIMER!
 * This is a wrapper for ref_hashtable.h/baseline impl.
 * The implementation is used for performance comparison and does not aim to be
 * sound or complete. There is an intentional memory leak in there.
 *
 * Note that only on VSYNC_ADDRESS_SANITIZER we try to workaround that, so we
 * pass the sanitize check. Since this is not aimed to be used outside the scope
 * of benchmarking it is ok to do so!
 *
 * We add ismr_none to handle the tracking of pointer and free them on
 * destruction, that introduces major performance overhead so it is only enabled
 * on VSYNC_ADDRESS_SANITIZER
 *
 */

#include <vsync/common/dbg.h>
#include <stdlib.h>
#include <test/vmem_stdlib.h>
#include <test/hashtable/ref_hashtable.h>
#include <test/hashtable/user_key.h>
#include <test/smr/ismr_none.h>

vuint64_t g_sanity_evict_count = 0;

typedef struct hash_entry_s {
    smr_node_t smr_node;
    user_key_t key;
    user_data_t data;
} hash_entry_t;


vhashtable_t g_hashtable;
vatomic64_t g_failed_insert_count;

static inline void
free_cb(smr_node_t *smr_node, void *args)
{
    free(smr_node);
    V_UNUSED(args);
}

static inline vuint64_t
iisize(void)
{
    return 0;
}

static inline void
map_init(void)
{
#if defined(VSYNC_ADDRESS_SANITIZER)
    ismr_init();
#endif
    vhashtable_init(&g_hashtable);
}

static inline void
map_destroy(void)
{
    vhashtable_destroy(&g_hashtable);
#if defined(VSYNC_ADDRESS_SANITIZER)
    ismr_destroy();
#endif
}

static inline vuint64_t
map_evict(vsize_t tid, vuint64_t count)
{
    V_UNUSED(tid, count);
    return 0;
}

static inline vbool_t
map_insert_stamped(vsize_t tid, user_key_t key, vsize_t stamp)
{
    hash_entry_t *entry = vmem_malloc(sizeof(hash_entry_t));
    entry->key          = key;
    entry->data.stamp   = stamp;

    void *evicted   = NULL;
    vbool_t success = vhashtable_insert(&g_hashtable, key, entry, &evicted);
    if (!success) {
        vatomic64_inc_rlx(&g_failed_insert_count);
        vmem_free(entry);
    }

#if defined(VSYNC_ADDRESS_SANITIZER)
    if (evicted) {
        ismr_retire(&((hash_entry_t *)evicted)->smr_node, free_cb, false);
    }
#endif

    V_UNUSED(tid);
    return success;
}
static inline vbool_t
map_insert(vsize_t tid, user_key_t key)
{
    return map_insert_stamped(tid, key, 0);
}

static inline hash_entry_t *
map_get(vsize_t tid, user_key_t key)
{
    hash_entry_t *entry = vhashtable_get(&g_hashtable, key);

    if (entry) {
        ASSERT(user_key_eq(entry->key, key));
    }
    V_UNUSED(tid);
    return entry;
}

static inline void
map_enter(vsize_t tid)
{
    V_UNUSED(tid);
}

static inline void
map_exit(vsize_t tid)
{
    V_UNUSED(tid);
}

static inline void
map_reg(vsize_t tid)
{
    V_UNUSED(tid);
}

static inline void
map_dereg(vsize_t tid)
{
    V_UNUSED(tid);
}
#endif
