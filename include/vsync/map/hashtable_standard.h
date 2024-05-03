/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VHASHTABLE_STANDARD_H
#define VHASHTABLE_STANDARD_H
/*******************************************************************************
 * @file hashtable_standard.h
 * @ingroup requires_smr lock_free linearizable
 * @brief This is a lock-free listset based hashtable.
 *
 * This hashtable uses a lock-free listset per bucket. The listset is ordered by
 * keys, and keys are expected to be unique.
 *
 * The table consists of `VHASHTABLE_BUCKET_COUNT` number of buckets with a
 * default value of `1024`. This can be overwritten by the user by compiling
 * with `-DVHASHTABLE_BUCKET_COUNT=N`. User control the mapping between keys and
 * buckets by passing `hash_idx` to the APIs. It is expected that users use
 * their own hash functions that map a key to a certain bucket index. Thus
 * the given `hash_idx` is expected to be `< VHASHTABLE_BUCKET_COUNT `.
 *
 * @note users may easily build their own hashtables e.g. when dynamic
 * allocation of the table is needed. Users can use vsync/map/listset_lf.h
 * or any other listset e.g. vsync/map/listset_lazy.h depending on the user
 * needs. listset_lf.h and listset_lazy.h can provide performance benefits under
 * high contention and oversubscription scenarios. If memory overhead is not an
 * issue and the buckets can grow long then skiplist_lf.h can be a better fit.
 *
 * @example
 * @include eg_hashtable_standard.c
 ******************************************************************************/
#include <vsync/map/listset_lf.h>
#include <vsync/map/internal/hashtable/hashtable_config.h>

typedef vlistset_t vbucket_t;
typedef vlistset_key_t vhashtable_key_t;
typedef vlistset_node_t vhashtable_entry_t;
typedef vlistset_cmp_key_t vhashtable_cmp_key_t;
typedef vlistset_handle_node_t vhashtable_entry_retire_t;

typedef struct vhashtable_s {
    vbucket_t buckets[VHASHTABLE_BUCKET_COUNT];
    vatomic64_t num_entries;
} vhashtable_t;

/**
 * Initializes the hashtable.
 *
 * @param table address of vhashtable_t object.
 * @param retire_cb address of a callback function of type
 * `vhashtable_entry_retire_t`. The function is called whenever an entry is
 * detached from the hashtable.
 * @param retire_cb_arg address of an extra argument of `retire_cb`.
 * @param cmp_cb address of callback function of type `vhashtable_cmp_key_t`.
 * Used for comparing an entry key to a given key.
 * @note must be called before threads start accessing the hashtable.
 */
static inline void
vhashtable_init(vhashtable_t *table, vhashtable_entry_retire_t retire_cb,
                void *retire_cb_arg, vhashtable_cmp_key_t cmp_cb)
{
    ASSERT(table);
    for (vsize_t i = 0; i < VHASHTABLE_BUCKET_COUNT; i++) {
        vlistset_init(&table->buckets[i], retire_cb, retire_cb_arg, cmp_cb);
    }
}
/**
 * Retires all entries in the hashtable.
 *
 * @param table address of vhashtable_t object.
 * @note this function is not thread-safe, can be called iff all threads are
 * done accessing the hashtable.
 */
static inline void
vhashtable_destroy(vhashtable_t *table)
{
    ASSERT(table);
    for (vsize_t i = 0; i < VHASHTABLE_BUCKET_COUNT; i++) {
        vlistset_destroy(&table->buckets[i]);
    }
}
/**
 * Inserts the given `entry` into the hashtable.
 *
 * @param table address of vhashtable_t object.
 * @param key value of the key object.
 * @param entry address of vhashtable_entry_t object.
 * @param hash_idx a hash value of the key used as a bucket index. must be
 * `< VHASHTABLE_BUCKET_COUNT`
 * @return true successful insertion.
 * @return false failed insertion, an entry associated with the given `key`
 * already exists.
 * @note this function must be called inside an SMR critical section.
 */
static inline vbool_t
vhashtable_insert(vhashtable_t *table, vhashtable_key_t key,
                  vhashtable_entry_t *val, vsize_t hash_idx)
{
    ASSERT(table);
    ASSERT(val);
    ASSERT(hash_idx < VHASHTABLE_BUCKET_COUNT);
    vbool_t success = vlistset_add(&table->buckets[hash_idx], key, val);
    if (success) {
        vatomic64_inc_rlx(&table->num_entries);
    }
    return success;
}
/**
 * Looks for the entry associated with the given `key`.
 *
 * @param table address of vhashtable_t object.
 * @param key value of the key object.
 * @param hash_idx a hash value of the key used as a bucket index. must be
 * `< VHASHTABLE_BUCKET_COUNT`
 * @return vhashtable_entry_t* address of the
 * `vhashtable_entry_t` object associated with the given key, if found.
 * @return `NULL` if key is not found.
 * @note this function must be called inside an SMR critical section.
 */
static inline vhashtable_entry_t *
vhashtable_get(vhashtable_t *table, vhashtable_key_t key, vsize_t hash_idx)
{
    ASSERT(table);
    ASSERT(hash_idx < VHASHTABLE_BUCKET_COUNT);
    return vlistset_get(&table->buckets[hash_idx], key);
}
/**
 * Removes the entry associated with the given key from the the hashtable.
 *
 * @param table address of vhashtable_t object.
 * @param key value of the key object.
 * @param hash_idx a hash value of the key used as a bucket index. must be
 * `< VHASHTABLE_BUCKET_COUNT`.
 * @return true successful remove, the entry associated with the given key was
 * removed.
 * @return false failed remove, no entry associated with the given key exists.
 */
static inline vbool_t
vhashtable_remove(vhashtable_t *table, vhashtable_key_t key, vsize_t hash_idx)
{
    ASSERT(table);
    ASSERT(hash_idx < VHASHTABLE_BUCKET_COUNT);
    vbool_t success = vlistset_remove(&table->buckets[hash_idx], key);
    if (success) {
        vatomic64_dec_rlx(&table->num_entries);
    }
    return success;
}
/**
 * Returns the count of entries currently available in the hashtable.
 *
 * @param table address of vhashtable_t object.
 * @return vuint64_t an approximate count of the entries available in the
 * hashtable.
 */
static inline vuint64_t
vhashtable_get_entries_count(vhashtable_t *table)
{
    ASSERT(table);
    return vatomic64_read_rlx(&table->num_entries);
}

#endif
