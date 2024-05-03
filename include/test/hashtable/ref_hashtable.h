/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VREF_HASHTABLE_H
#define VREF_HASHTABLE_H

#include <vsync/common/assert.h>
#include <vsync/spinlock/seqlock.h>
#include <vsync/map/internal/hashtable/hashtable_config.h>
#include <test/hashtable/user_key.h>
#include <vsync/utils/math.h>

/**
 * !DISCLAIMER!
 * @note this implementation is there for performance comparison purposes.
 * It is not sound or accurate, it is just an approximation.
 *
 * Do not use it in any application!! or outside the scope of hashtable
 * benchmark
 *
 */

/* extension */
static inline vbool_t
seqlock_is_writer_active(seqlock_t *seq)
{
    return VIS_ODD(vatomic32_read_rlx(&seq->seqcount));
}


typedef struct ventry_s {
    user_key_t key;
    void *value;
    seqlock_t lock;
} ventry_t;

typedef struct vbucket_s {
    ventry_t entries[VHASHTABLE_BUCKET_LEN];
    vatomic8_t hint;
    vatomic8_t num_entries;
} vbucket_t;

typedef struct vhashtable_s {
    vbucket_t buckets[VHASHTABLE_BUCKET_COUNT];
} vhashtable_t;


static inline void
vhashtable_init(vhashtable_t *table)
{
    ASSERT(table);
    ventry_t *entry   = NULL;
    vbucket_t *bucket = NULL;

    for (vsize_t i = 0; i < VHASHTABLE_BUCKET_COUNT; i++) {
        bucket = &table->buckets[i];
        vatomic8_write_rlx(&bucket->num_entries, 0);
        vatomic8_write_rlx(&bucket->hint, 0);

        for (vsize_t j = 0; j < VHASHTABLE_BUCKET_LEN; j++) {
            entry = &bucket->entries[j];
            seqlock_init(&entry->lock);
        }
    }
}

static inline void
vhashtable_destroy(vhashtable_t *table)
{
    ventry_t *entry   = NULL;
    vbucket_t *bucket = NULL;

    for (vsize_t i = 0; i < VHASHTABLE_BUCKET_COUNT; i++) {
        bucket = &table->buckets[i];
        for (vsize_t j = 0; j < VHASHTABLE_BUCKET_LEN; j++) {
            entry = &bucket->entries[j];
            free(entry->value);
        }
    }
    V_UNUSED(table);
}

static inline void
_vhashtable_write_entry(ventry_t *entry, user_key_t key, void *val)
{
    entry->key   = key;
    entry->value = val;
}

static vbool_t
_vhashtable_bucket_update(vbucket_t *bucket, user_key_t key, void *val,
                          void **evicted)
{
    vsize_t idx     = 0;
    ventry_t *entry = NULL;

    uint8_t hint = vatomic8_get_inc_rlx(&bucket->hint);

    for (idx = 0; idx < VHASHTABLE_BUCKET_LEN; idx++, hint++) {
        entry = &bucket->entries[hint % VHASHTABLE_BUCKET_LEN];

        /* someone else is writing this entry, skip */
        if (seqlock_is_writer_active(&entry->lock)) {
            DBG_RED("writer active");
            continue;
        }
        seqlock_acquire(&entry->lock);
        *evicted = entry->value;
        _vhashtable_write_entry(entry, key, val);
        seqlock_release(&entry->lock);
        return true;
    }

    return false;
}

static inline vbool_t
vhashtable_insert(vhashtable_t *table, user_key_t key, void *val,
                  void **evicted)
{
    vsize_t idx = user_key_hash(key);
    ASSERT(table);
    ASSERT(val);
    ASSERT(idx < VHASHTABLE_BUCKET_COUNT);
    vbucket_t *bucket    = &table->buckets[idx];
    ventry_t *entry      = NULL;
    vuint8_t num_entries = vatomic8_read_rlx(&bucket->num_entries);
    // check if a new entry can be added
    if (num_entries < VHASHTABLE_BUCKET_LEN) {
        entry = &bucket->entries[num_entries];
        seqlock_acquire(&entry->lock);
        *evicted = entry->value;
        _vhashtable_write_entry(entry, key, val);
        vatomic8_write_rlx(&bucket->num_entries, num_entries + 1);
        seqlock_release(&entry->lock);
        return true;
    } else {
        return _vhashtable_bucket_update(bucket, key, val, evicted);
    }
}

static inline void *
_vhashtable_entry_match(ventry_t *entry, user_key_t key)
{
    void *val     = NULL;
    seqvalue_t sv = seqlock_rbegin(&entry->lock);
    if (user_key_eq(entry->key, key)) {
        val = entry->value;
    } else {
        return NULL;
    }
    return seqlock_rend(&entry->lock, sv) ? val : NULL;
}

static inline void *
vhashtable_get(vhashtable_t *table, user_key_t key)
{
    ASSERT(table);
    vsize_t idx = user_key_hash(key);
    ASSERT(idx < VHASHTABLE_BUCKET_COUNT);

    vbucket_t *bucket    = &table->buckets[idx];
    ventry_t *entry      = NULL;
    void *val            = NULL;
    vuint8_t num_entries = vatomic8_read_rlx(&bucket->num_entries);

    DBG_YELLOW("%u", num_entries);

    for (vsize_t i = 0; i < num_entries; i++) {
        entry = &bucket->entries[i];
        val   = _vhashtable_entry_match(entry, key);

        DBG_YELLOW("val %p", val);

        if (val) {
            break;
        }
    }
    return val;
}

static inline void
vhashtable_visit_entries(vhashtable_t *table, void (*visit)(void *))
{
    for (vsize_t i = 0; i < VHASHTABLE_BUCKET_COUNT; i++) {
        vbucket_t *bucket   = &table->buckets[i];
        vsize_t num_entries = vatomic8_read_rlx(&bucket->num_entries);
        for (vsize_t j = 0; j < num_entries; j++) {
            visit(bucket->entries[j].value);
        }
    }
}

#endif
