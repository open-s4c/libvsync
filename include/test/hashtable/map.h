/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VIHASHABLE_H
#define VIHASHABLE_H

#include <vsync/common/dbg.h>
#include <vsync/common/compiler.h>
#include <test/hashtable/hashtable.h>
#include <test/smr/ismr.h>
#include <stdlib.h>
#include <test/vmem_stdlib.h>
#include <test/rand.h>
#include <test/hashtable/user_key.h>

#define VHASH_PASS_KEY(_key_) ((vhashtable_key_t)(&(_key_)))

typedef struct hash_entry_s {
    user_key_t key;
    user_data_t data;
    vhashtable_entry_t hnode;
    smr_node_t smr_node;
} hash_entry_t;

vhashtable_t g_hashtable;

#define MIN_RECLAIM_COUNT 1
#define RECLAIM_FREQUENCY 1

vatomic64_t g_evict_count         = VATOMIC_INIT(0);
vatomic64_t g_failed_insert_count = VATOMIC_INIT(0);

#ifdef TEST_SANITY
vsize_t g_sanity_evict_count = 0;
#endif

static inline hash_entry_t *
_hnode_to_entry(vhashtable_entry_t *node)
{
    return node ? V_CONTAINER_OF(node, hash_entry_t, hnode) : NULL;
}

static inline hash_entry_t *
_snode_to_entry(smr_node_t *node)
{
    return node ? V_CONTAINER_OF(node, hash_entry_t, smr_node) : NULL;
}

static inline void
_reclaim_callback(smr_node_t *snode, void *args)
{
    hash_entry_t *entry = _snode_to_entry(snode);
    vmem_free(entry);
    V_UNUSED(args);
}

static inline int
_cmp_callback(vhashtable_entry_t *node, vhashtable_key_t k)
{
    user_key_t *key     = (user_key_t *)k;
    hash_entry_t *entry = _hnode_to_entry(node);
    return user_key_cmp(*key, entry->key);
}
#ifdef VLISTSET_NO_FUN_POINTER
inline int
vlistset_cmp_key(struct vlistset_node_s *node, vhashtable_key_t k)
{
    return _cmp_callback((vhashtable_entry_t *)node, k);
}
#endif

static inline void
_evict_callback(vhashtable_entry_t *hnode, void *args)
{
    hash_entry_t *entry = _hnode_to_entry(hnode);
#if defined(SMR_EBR) && !defined(ISMR_USE_TLS)
    #error ebr will not work without tid here
#endif
    ismr_retire(&entry->smr_node, _reclaim_callback, false);
    vatomic64_inc_rlx(&g_evict_count);
#ifdef TEST_SANITY
    g_sanity_evict_count++;
    DBG_GREEN("entry is evicted with timestamp %zu", entry->data.stamp);
#endif

    V_UNUSED(args);
}

vuint64_t
iisize(void)
{
    return vhashtable_get_entries_count(&g_hashtable);
}

#if defined(HASHTABLE_EVICTABLE)

static inline _ghost_ht_stats_t
_map_get_stats(void)
{
    return _vhashtable_get_stats(&g_hashtable);
}

#endif

static inline void
istats_verify(vbool_t expect_eq)
{
    vuint64_t alloc_count         = vmem_get_alloc_count();
    vuint64_t free_count          = vmem_get_free_count();
    vuint64_t evict_count         = vatomic64_read_rlx(&g_evict_count);
    vuint64_t failed_insert_count = vatomic64_read_rlx(&g_failed_insert_count);

    DBG_GREEN("alloc_count = %lu", alloc_count);
    DBG_YELLOW("evict_count = %lu", evict_count);
    DBG_YELLOW("failed_insert_count = %lu", failed_insert_count);
    DBG_RED("free_count  = %lu", free_count);

    if (expect_eq) {
#if !defined(SMR_NONE)
        ASSERT(free_count == alloc_count);
        ASSERT(evict_count + failed_insert_count == free_count);
#endif
    } else {
        ASSERT(free_count <= alloc_count);
        ASSERT(evict_count <= alloc_count);
    }
}

static inline user_key_t
gen_key(void)
{
#if defined(VSYNC_VERIFICATION) || defined(VSYNC_VERIFICATION_TEST)
    return 0;
#else
    user_key_t key = {0};
    vuint32_t max  = VINT32_MAX - 10U;
    key.ssid       = random_thread_safe_get_next(1, max);
    key.tsid       = random_thread_safe_get_next(1, max);
    key.tclass = (vuint16_t)random_thread_safe_get_next(1, VHASH_TCLASS_MAX);
    return key;
#endif
}

static inline void
map_init(void)
{
    ismr_init();

    vhashtable_init(&g_hashtable, _evict_callback, NULL, _cmp_callback);

#if !defined(VSYNC_VERIFICATION) && !defined(VSYNC_VERIFICATION_TEST)
    ismr_start_cleaner();
#endif
}

static inline void
map_destroy(void)
{
    // TODO: implement destroy for hashtable entries
    istats_verify(false);
#if !defined(VSYNC_VERIFICATION) && !defined(VSYNC_VERIFICATION_TEST)
    ismr_stop_cleaner();
#endif
    vhashtable_destroy(&g_hashtable);
    ismr_destroy();
    istats_verify(true);
}

static inline vuint64_t
map_evict(vsize_t tid, vuint64_t count)
{
#if defined(HASHTABLE_EVICTABLE)
    vuint64_t evict_count = vhashtable_evict(&g_hashtable, count);
    V_UNUSED(tid);
    return evict_count;
#else
    /* not supported */
    V_UNUSED(tid, count);
    return 0;
#endif
}

static inline vuint64_t
map_get_entries_count(void)
{
    return vhashtable_get_entries_count(&g_hashtable);
}

static inline vbool_t
map_insert_stamped(vsize_t tid, user_key_t key, vsize_t stamp)
{
    hash_entry_t *entry = vmem_malloc(sizeof(hash_entry_t));
    entry->key          = key;
    entry->data.stamp   = stamp;

    vbool_t success = vhashtable_insert(&g_hashtable, VHASH_PASS_KEY(key),
                                        &entry->hnode, user_key_hash(key));
    if (!success) {
        vatomic64_inc_rlx(&g_failed_insert_count);
        vmem_free(entry);
    }
    V_UNUSED(tid);
    return success;
}

static inline vbool_t
map_insert(vsize_t tid, user_key_t key)
{
    return map_insert_stamped(tid, key, 0);
}

static inline vbool_t
map_remove(vsize_t tid, user_key_t key)
{
    V_UNUSED(tid);
    return vhashtable_remove(&g_hashtable, VHASH_PASS_KEY(key),
                             user_key_hash(key));
}

static inline hash_entry_t *
map_get(vsize_t tid, user_key_t key)
{
    vhashtable_entry_t *hnode =
        vhashtable_get(&g_hashtable, VHASH_PASS_KEY(key), user_key_hash(key));
    hash_entry_t *entry = _hnode_to_entry(hnode);

    if (hnode) {
        ASSERT(user_key_eq(key, entry->key));
    }
    V_UNUSED(tid);
    return entry;
}

static inline void
map_enter(vsize_t tid)
{
    ismr_enter(tid);
}

static inline void
map_exit(vsize_t tid)
{
    ismr_exit(tid);
}

static inline void
map_reg(vsize_t tid)
{
    ismr_reg(tid);
}

static inline void
map_dereg(vsize_t tid)
{
    ismr_dereg(tid);
}

#endif
