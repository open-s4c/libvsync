/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSIMPLE_HT_LF_H
#define VSIMPLE_HT_LF_H

/******************************************************************************
 * @file simpleht.h
 * @brief Simple lock-free hashtable
 * @ingroup lock_free linearizable
 *
 * This is an adaptation of the simple lock-free hashtable mentioned in the
 * reference. The adaptation allow for remove operation if users need it.
 * Users should be aware that enabling the remove incurs overhead on
 * add/get operations. The remove operation can be quite slow as it rebalances
 * the hashtable and when that happens lock-freedom is compromised.
 *
 * This hashtable is ideal for when no remove operations are needed, in that
 * case users should disable the support of remove by compiling with
 * `-DVSIMPLEHT_DISABLE_REMOVE`. Or keep it, and make sure remove is rarely
 * called.
 *
 * # Operating Conditions
 *
 * The hashtable is supposed to keep gaps for optimal performance. Users
 * should create the table with at least a capacity of 2*N, where N
 * is the maximum number of entries to be inserted.
 *
 * As long as the hashtable is not full, every search is guaranteed to
 * finish either by locating the desired key, or by locating an entry whose key
 * is zero, which means that the desired key does not exist in the hashtable.
 *
 * There is a tradeoff between performance and the number of empty slots in the
 * hashtable. The performance degrades as the hashtable fills. lock-freedom
 * of add is compromised if the hashtable becomes full.
 *
 * @example
 * @include eg_simpleht.c
 *
 * @cite
 * [The World's Simplest Lock-Free Hash Table]
 * (https://preshing.com/20130605/the-worlds-simplest-lock-free-hash-table/)
 *
 *****************************************************************************/
#include <vsync/atomic.h>
#include <vsync/common/dbg.h>
#include <vsync/common/assert.h>
/**
 * @def VSIMPLEHT_DISABLE_REMOVE
 * @brief defines VSIMPLEHT_DISABLE_REMOVE to use the simple hashtable without
 * entry removal support. This makes the insert and get operations faster
 */
#if !defined(VSIMPLEHT_DISABLE_REMOVE)
    #if defined(VSYNC_VERIFICATION)
        #include <verify/rwlock.h>
    #else
        #include <vsync/spinlock/rwlock.h>
    #endif
#endif

/**
 * @def VSIMPLEHT_RELATIVE_THRESHOLD
 * @brief Controls when the rebalancing of the hashtable is triggered.
 * Everytime the number of removed nodes is equal to
 * capacity/VSIMPLEHT_RELATIVE_THRESHOLD.
 *
 */
#if !defined(VSIMPLEHT_RELATIVE_THRESHOLD)
    #define VSIMPLEHT_RELATIVE_THRESHOLD 4U
#endif

typedef vint8_t (*vsimpleht_cmp_key_t)(vuintptr_t key_a, vuintptr_t key_b);
typedef vuint64_t (*vsimpleht_hash_key_t)(vuintptr_t key);
typedef void (*vsimpleht_destroy_entry_t)(void *entry);

typedef struct vsimpleht_entry_s {
    vatomicptr(vuintptr_t) key;
    vatomicptr(void *) value;
} vsimpleht_entry_t;

typedef struct vsimpleht_s {
    vsize_t capacity;
    vsimpleht_entry_t *entries;
    vsimpleht_cmp_key_t cmp_key;
    vsimpleht_hash_key_t hash_key;
    vsimpleht_destroy_entry_t cb_destroy;
#if !defined(VSIMPLEHT_DISABLE_REMOVE)
    vsize_t cleaning_threshold;
    vatomicsz_t deleted_count;
    rwlock_t lock;
#endif
} vsimpleht_t;

typedef struct vsimpleht_iter_s {
    vsimpleht_t *tbl;
    vsize_t idx;
} vsimpleht_iter_t;

typedef enum vsimpleht_ret_e {
    VSIMPLEHT_RET_OK,
    VSIMPLEHT_RET_TBL_FULL,
    VSIMPLEHT_RET_KEY_EXISTS,
    VSIMPLEHT_RET_KEY_DNE,
} vsimpleht_ret_t;
/******************************************************************************
 * Prototypes - internal functions
 *****************************************************************************/
static inline void _vsimpleht_cleanup(vsimpleht_t *tbl, void *val);
static inline void _vsimpleht_trigger_cleanup(vsimpleht_t *tbl, void *val);
static inline void _vsimpleht_give_cleanup_a_chance(vsimpleht_t *tbl);
static inline vsimpleht_ret_t _vsimpleht_add(vsimpleht_t *tbl, vuintptr_t key,
                                             void *value);

/**
 * Calculates the size of the buffer needed by the hashtable.
 *
 * @param capacity capacity of the hashtable. Maximum number of items to be
 * inserted in the hashtable.
 * @return vsize_t required buffer size to fit `len` items.
 *
 * @note capacity must be power of two.
 */
static inline vsize_t
vsimpleht_buff_size(vsize_t capacity)
{
    ASSERT(capacity > 0);
    ASSERT((capacity & (capacity - 1)) == 0 && "capacity must be power of 2");
    return sizeof(vsimpleht_entry_t) * capacity;
}
/**
 * Initializes the hashtable.
 *
 * @param tbl address of vsimpleht_t object.
 * @param buff address of the hashtable buffer. Allocated by the user.
 * @param capacity capacity of the hashtable. Same value passed to
 * vsimpleht_buff_size.
 * @param cmp_key compare key callback function address.
 * @param hash_key address of callback function. Used to hash the key.
 * @param destroy_cb address of destroy callback function. Used to destroy the
 * hashtable stored objects. Called via `vsimpleht_remove` and
 * `vsimpleht_destroy`.
 */
static inline void
vsimpleht_init(vsimpleht_t *tbl, void *buff, vsize_t capacity,
               vsimpleht_cmp_key_t cmp_fun, vsimpleht_hash_key_t hash_fun,
               vsimpleht_destroy_entry_t destroy_cb)
{
    ASSERT(tbl);
    ASSERT(buff);
    ASSERT(capacity > 0);
    ASSERT((capacity & (capacity - 1)) == 0 && "Array size must be power of 2");

    tbl->capacity   = capacity;
    tbl->entries    = buff;
    tbl->cmp_key    = cmp_fun;
    tbl->hash_key   = hash_fun;
    tbl->cb_destroy = destroy_cb;

    for (vsize_t i = 0; i < tbl->capacity; i++) {
        vatomicptr_init(&tbl->entries[i].key, NULL);
        vatomicptr_init(&tbl->entries[i].value, NULL);
    }
#if !defined(VSIMPLEHT_DISABLE_REMOVE)
    tbl->cleaning_threshold = (capacity / VSIMPLEHT_RELATIVE_THRESHOLD);
    vatomicsz_write_rlx(&tbl->deleted_count, 0);
    rwlock_init(&tbl->lock);
#endif
}

/**
 * Destroys the stored hashtable values.
 *
 * @param tbl address of vsimpleht_t object.
 */
static inline void
vsimpleht_destroy(vsimpleht_t *tbl)
{
    vsimpleht_entry_t *entry = NULL;
    void *obj                = NULL;
    ASSERT(tbl);
    for (vsize_t i = 0; i < tbl->capacity; i++) {
        entry = &tbl->entries[i];
        obj   = vatomicptr_read_rlx(&entry->value);
        if (obj) {
            tbl->cb_destroy(obj);
        }
    }
}
/**
 * Registers the caller thread.
 *
 * @param tbl address of vsimpleht_t object.
 *
 * @note should be called once per thread, before the calling thread accesses
 * the hashtable for the first time.
 */
static inline void
vsimpleht_thread_register(vsimpleht_t *tbl)
{
#if defined(VSIMPLEHT_DISABLE_REMOVE)
    V_UNUSED(tbl);
#else
    rwlock_read_acquire(&tbl->lock);
#endif
}
/**
 * Deregisters the caller thread.
 *
 * @param tbl address of vsimpleht_t object.
 *
 * @note should be called once per thread, after the calling thread accesses
 * the hashtable for the last time.
 */
static inline void
vsimpleht_thread_deregister(vsimpleht_t *tbl)
{
#if defined(VSIMPLEHT_DISABLE_REMOVE)
    V_UNUSED(tbl);
#else
    rwlock_read_release(&tbl->lock);
#endif
}
/**
 * Inserts the given value into the hashtable.
 *
 * @param tbl address of vsimpleht_t object.
 * @param key key of the value to add.
 * @param value address of the object to insert.
 * @return VSIMPLEHT_RET_OK key does not exist, value was added.
 * @return VSIMPLEHT_RET_KEY_EXISTS key exists, value was not added.
 * @return VSIMPLEHT_RET_TBL_FULL table is full. Consider
 *  - increasing the capacity
 *  - decreasing VSIMPLEHT_RELATIVE_THRESHOLD
 *  - removing items from the table
 *
 * @note neither key can be 0 nor value can be NULL.
 */
static inline vsimpleht_ret_t
vsimpleht_add(vsimpleht_t *tbl, vuintptr_t key, void *value)
{
    ASSERT(key != 0);
    ASSERT(value != NULL);
    _vsimpleht_give_cleanup_a_chance(tbl);
    return _vsimpleht_add(tbl, key, value);
}
/**
 * Searches the hashtable for a value associated with the given key.
 *
 * @param tbl address of vsimpleht_t object.
 * @param key key to search for.
 * @return void* address of the object associated with the given key if exists.
 * @return NULL if there is no value found associated with given key.
 */
static inline void *
vsimpleht_get(vsimpleht_t *tbl, vuintptr_t key)
{
    vsize_t index         = 0;
    vuintptr_t probed_key = 0;
    _vsimpleht_give_cleanup_a_chance(tbl);
    for (index = tbl->hash_key(key);; index++) {
        index &= tbl->capacity - 1;
        ASSERT(index < tbl->capacity);
        probed_key = (vuintptr_t)vatomicptr_read(&tbl->entries[index].key);
        if (probed_key == 0) {
            return NULL;
        } else if (tbl->cmp_key(key, probed_key) == 0) {
            return vatomicptr_read_acq(&tbl->entries[index].value);
        }
    }
}
/**
 * Initializes the given iterator object `iter` for operating `tbl` object.
 *
 * @param tbl address of vsimpleht_t object.
 * @param iter address of vsimpleht_iter_t object to initialize.
 */
static inline void
vsimpleht_iter_init(vsimpleht_t *tbl, vsimpleht_iter_t *iter)
{
    ASSERT(tbl);
    ASSERT(iter);
    iter->tbl = tbl;
    iter->idx = 0;
}
/**
 * Reads the current entry (key/value) the iterator has reached.
 *
 * After the values are read, the iterator advances to the next entry.
 *
 * @param iter address of vsimpleht_iter_t object.
 * @param key output parameter where the current key value is stored.
 * @param val output parameter where the current object value is stored.
 * @return true the current output of key and val is valid.
 * @return false there are no more objects to iterate. Stored values in key, val
 * parameters should be ignored by the user.
 *
 * @pre iter must be initalized via vsimpleht_iter_init.
 *
 * @note the iterator is mainly meant for sequential setting. When called in
 * parallel the results might not be consistent.
 *
 * @example
 *  ```c
 *  vuintptr_t key = 0;
 *  data_t *date = NULL;
 *  vsimpleht_iter_t iter;
 *  vsimpleht_iter_init(&g_simpleht, &iter);
 *   while (vsimpleht_iter_next(&iter, &key, (void **)&data)) {
 *       // access key
 *       // access data
 *   }
 * ```
 */
static inline vbool_t
vsimpleht_iter_next(vsimpleht_iter_t *iter, vuintptr_t *key, void **val)
{
    vuintptr_t k               = 0;
    void *v                    = NULL;
    vsimpleht_entry_t *entries = NULL;
    ASSERT(iter);
    ASSERT(iter->tbl);
    ASSERT(key);
    ASSERT(val);
    entries = iter->tbl->entries;
    ASSERT(entries);
    for (vsize_t i = iter->idx; i < iter->tbl->capacity; i++) {
        k = (vuintptr_t)vatomicptr_read(&entries[i].key);
        v = vatomicptr_read(&entries[i].value);
        if (k && v) {
            iter->idx = i + 1;
            *key      = k;
            *val      = v;
            return true;
        }
    }
    return false;
}
/**
 * Removes the node associated with the given key from the hashtable.
 *
 * @param tbl address of vsimpleht_t object.
 * @param key key to remove the value associated with.
 * @return VSIMPLEHT_RET_OK key exists, node was removed.
 * @return VSIMPLEHT_RET_KEY_DNE key does not exist.
 *
 * @note this operation is not lock-free, if the the removal is successful. It
 * can be super-slow, and the associated value can be freed via the destroy
 * callback registered at `vsimpleht_init`.
 *
 */
static inline vsimpleht_ret_t
vsimpleht_remove(vsimpleht_t *tbl, vuintptr_t key)
{
#if defined(VSIMPLEHT_DISABLE_REMOVE)
    ASSERT(0 &&
           "vsimpleht_remove has no effect when VSIMPLEHT_DISABLE_REMOVE is "
           "defined.");
    V_UNUSED(tbl, key);
    return VSIMPLEHT_RET_KEY_DNE;
#else
    vsize_t index         = 0;
    vuintptr_t probed_key = 0;
    void *probed_value    = NULL;
    vsize_t start_index   = tbl->hash_key(key);
    index                 = start_index & (tbl->capacity - 1);
    _vsimpleht_give_cleanup_a_chance(tbl);
    do {
        probed_key = (vuintptr_t)vatomicptr_read(&tbl->entries[index].key);
        if (probed_key == 0) {
            // if we reached the end of the bucket
            // then the key does not exist.
            return VSIMPLEHT_RET_KEY_DNE;
        } else if (tbl->cmp_key(key, probed_key) == 0) {
            probed_value = vatomicptr_read_acq(&tbl->entries[index].value);
            // we found the key
            // we check if the value exists
            if (probed_value == NULL) {
                // the item was there but is logically removed
                return VSIMPLEHT_RET_KEY_DNE;
            }
            /* logically remove the entry by marking the value as NULL */
            if (vatomicptr_cmpxchg_rel(&tbl->entries[index].value, probed_value,
                                       NULL) == probed_value) {
                vatomicsz_inc_rlx(&tbl->deleted_count);
                /* logical removal is successful, we trigger actual cleanup */
                _vsimpleht_trigger_cleanup(tbl, probed_value);
                return VSIMPLEHT_RET_OK;
            }
            return VSIMPLEHT_RET_KEY_DNE;
        }

        index++;
        index &= tbl->capacity - 1;
    } while (index != start_index);
    // if we circled around once the key
    // does not exist
    return VSIMPLEHT_RET_KEY_DNE;
#endif
}
/**
 * Inserts the given value into the hashtable.
 *
 * @param tbl address of vsimpleht_t object.
 * @param key key of the value to add.
 * @param value address of the object to insert.
 * @return VSIMPLEHT_RET_OK key does not exist, value was added.
 * @return VSIMPLEHT_RET_KEY_EXISTS key exists, value was not added.
 * @return VSIMPLEHT_RET_TBL_FULL table is full.
 *
 * @note neither key can be 0 nor value can be NULL.
 * @note this API is internal and should not be called by users.
 */
static inline vsimpleht_ret_t
_vsimpleht_add(vsimpleht_t *tbl, vuintptr_t key, void *value)
{
    vsize_t index         = 0;
    vuintptr_t probed_key = 0;
    void *val             = NULL;
    vsize_t cnt           = 0;

    ASSERT(key && "NULL key is not allowed!");
    ASSERT(value && "NULL value is not allowed!");
    // linear search at an index determined by hashing the key.
    // scan the array and store the item in the first entry whose
    // existing key is either 0, or matches the desired key
    for (index = tbl->hash_key(key); cnt < tbl->capacity; cnt++, index++) {
        // keep the index within the array boundaries
        index &= tbl->capacity - 1;
        ASSERT(index < tbl->capacity);
        // Load the key that was there.
        probed_key = (vuintptr_t)vatomicptr_read(&tbl->entries[index].key);
        /*
            There is no key in this slot, so we try
            to occupy it.
        */
        if (probed_key == 0) {
            probed_key = (vuintptr_t)vatomicptr_cmpxchg(
                &tbl->entries[index].key, NULL, (void *)key);
            /* someone stole it, and inserted a different key */
            if (probed_key != 0 && tbl->cmp_key(key, probed_key) != 0) {
                continue;
            }
        } else if (tbl->cmp_key(key, probed_key) != 0) {
            /* There exists a key at this slot, but it is not equal
            to the one we want to insert, so we move on */
            continue;
        }
        ASSERT(tbl->cmp_key(key, (vuintptr_t)vatomicptr_read(
                                     &tbl->entries[index].key)) == 0);
        /*
            we reach here under the following circumstances
            1. There is a key in this slot that is equal to ours.
            2. There was no key and we managed to insert ours.

            so we try to insert the value:
            1. there is already a value we fail
            2. there is no value, entry has either been deleted, so
            we just reuse the slot. or someone with the same key is
           in the process of inserting its value in that case she fails
           and we succeed.
           We use cas with expected old value NULL
        */
        val = vatomicptr_cmpxchg(&tbl->entries[index].value, NULL, value);
        return (val == NULL) ? VSIMPLEHT_RET_OK : VSIMPLEHT_RET_KEY_EXISTS;
    }
    return VSIMPLEHT_RET_TBL_FULL;
}
/**
 * Releases the reader lock briefly if there is a writer waiting to acquire it.
 *
 * @note this gives the cleaner thread a chance to rebalance the table and free
 * the removed node.
 *
 * @param tbl address of vsimpleht_t object.
 */
static inline void
_vsimpleht_give_cleanup_a_chance(vsimpleht_t *tbl)
{
#if defined(VSIMPLEHT_DISABLE_REMOVE)
    V_UNUSED(tbl);
#else
    if (rwlock_acquired_by_writer(&tbl->lock)) {
        ASSERT(rwlock_acquired_by_readers(&tbl->lock) &&
               "You seem to have forgotten to call the "
               " thread register function");
        rwlock_read_release(&tbl->lock);
        rwlock_read_acquire(&tbl->lock);
    }
#endif
}
/**
 * Releases the reader-lock, triggers cleaning if needed and then reacquires the
 * lock.
 *
 * @param tbl address of vsimpleht_t object.
 * @param val address of the removed object.
 */
static inline void
_vsimpleht_trigger_cleanup(vsimpleht_t *tbl, void *val)
{
#if defined(VSIMPLEHT_DISABLE_REMOVE)
    V_UNUSED(tbl, val);
#else
    ASSERT(rwlock_acquired_by_readers(&tbl->lock) &&
           "You seem to have forgotten to call the "
           " thread register function");
    rwlock_read_release(&tbl->lock);
    _vsimpleht_cleanup(tbl, val);
    rwlock_read_acquire(&tbl->lock);
#endif
}
#if defined(VSIMPLEHT_DISABLE_REMOVE)
static inline void
_vsimpleht_cleanup(vsimpleht_t *tbl, void *val)
{
    V_UNUSED(tbl, val);
}
#else
/**
 * Destructs val, and rebalances the hashtable buckets if the threshold is met.
 *
 * @param tbl address of vsimpleht_t object.
 * @param val address of the removed object.
 *
 * @note this is a blocking operation, acquires the lock in write-mode.
 */
static inline void
_vsimpleht_cleanup(vsimpleht_t *tbl, void *val)
{
    vsize_t e = 0;
    vsize_t i = 0;
    vuintptr_t key = 0;
    void *value = NULL;
    vsize_t len = tbl->capacity;
    vsimpleht_entry_t *entries = tbl->entries;
    vsize_t start_index = len - 1;
    uint8_t ret;

    /* we have to go through the lock now */
    rwlock_write_acquire(&tbl->lock);
    /* double check if we got lucky in the meantime, someone else did the
     * job for us */
    if (vatomicsz_read_rlx(&tbl->deleted_count) < tbl->cleaning_threshold) {
        goto CLEANUP_EXIT;
    }
    /* avoid bug in the next loop in which we first pass an element that
     * needs to be resettled, but due to wrap-around of the bucket the
     * position into which it should be resettled (logically deleted) has
     * not yet been seen by the loop and freed.
     * To avoid this we make sure we start the next loop at the beginning of
     * a bucket not in the middle.
     * Find a beginning of a bucket.
     */
    for (e = 0; e < len; e++) {
        key = (vuintptr_t)vatomicptr_read_rlx(&entries[e].key);
        value = vatomicptr_read_rlx(&entries[e].value);
        if (key == 0) {
            start_index = e;
            break;
        }
    }
    /* scan the whole array starting from the start_index (index of first
     * bucket found above), delete logically removed items, and resettle
     * existing items. Resettling is important, to avoid having a gap within
     * a bucket. The end of the bucket is recognized as a slot with an empty
     * key. If a bucket has a gap, we will mistake elements of the bucket
     * as not existing when they are placed after the gap.
     */
    for (e = 0; e < len; e++) {
        i = (e + start_index) & (len - 1);
        key = (vuintptr_t)vatomicptr_read_rlx(&entries[i].key);
        value = vatomicptr_read_rlx(&entries[i].value);
        if (key != 0 && value != NULL) {
            /* the addition may fail if the resettled position is equal to
             * the current position. In that case, we just leave the entry
             * intact. */
            ret = _vsimpleht_add(tbl, key, value);
            if (ret == VSIMPLEHT_RET_OK) {
                vatomicptr_write_rlx(&entries[i].key, NULL);
                vatomicptr_write_rlx(&entries[i].value, NULL);
            }
            ASSERT(ret != VSIMPLEHT_RET_TBL_FULL &&
                   "since we are inserting what is already in the table, "
                   "this should never happen");
        } else if (key != 0 && value == NULL) {
            vatomicptr_write_rlx(&entries[i].key, NULL);
        }
    }
    vatomicsz_write_rlx(&tbl->deleted_count, 0);

CLEANUP_EXIT:
    tbl->cb_destroy(val);
    rwlock_write_release(&tbl->lock);
}
#endif

#endif
