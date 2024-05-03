/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VUSER_KEY_H
#define VUSER_KEY_H

#include <vsync/vtypes.h>
#include <test/rand.h>

#include <vsync/map/internal/hashtable/hashtable_config.h>

#define VHASH_TCLASS_MAX (UINT16_MAX - 10U)

#if defined(VSYNC_VERIFICATION) || defined(VSYNC_VERIFICATION_TEST)
typedef vuint16_t user_key_t;
#else
typedef struct user_key_s {
    vuint32_t ssid;
    vuint32_t tsid;
    vuint16_t tclass;
} user_key_t;
#endif


typedef struct user_data_s {
    vsize_t stamp;
    // Place holder
} user_data_t;

static inline vbool_t
user_key_eq(user_key_t a, user_key_t b)
{
#if defined(VSYNC_VERIFICATION) || defined(VSYNC_VERIFICATION_TEST)
    return a == b;
#else
    return (a.tclass == b.tclass) && (a.ssid == b.ssid) && (a.tsid == b.tsid);
#endif
}

static inline int
user_key_cmp(user_key_t a, user_key_t b)
{
#if defined(VSYNC_VERIFICATION) || defined(VSYNC_VERIFICATION_TEST)
    if (a < b)
        return -1;
    if (a == b)
        return 0;
    return 1;
#else
    if (a.ssid != b.ssid) {
        if (a.ssid < b.ssid)
            return -1;
        return 1;
    } else if (a.tsid != b.tsid) {
        if (a.tsid < b.tsid)
            return -1;
        return 1;
    } else if (a.tclass != b.tclass) {
        if (a.tclass < b.tclass)
            return -1;
        return 1;
    } else {
        ASSERT(a.tclass == b.tclass);
        ASSERT(a.ssid == b.ssid);
        ASSERT(a.tsid == b.tsid);
        return 0;
    }
#endif
}


static inline vsize_t
user_key_hash(user_key_t key)
{
#if defined(VSYNC_VERIFICATION) || defined(VSYNC_VERIFICATION_TEST)
    vsize_t idx = key % VHASHTABLE_BUCKET_COUNT;
#else
    vsize_t idx = (key.ssid ^ (key.tsid << 2) ^ (key.tclass << 4)) &
                  (VHASHTABLE_BUCKET_COUNT - 1);
#endif

    return idx;
}


static inline void
iprint_key(char *prefix, vsize_t tid, user_key_t key)
{
#if defined(VSYNC_VERIFICATION) || defined(VSYNC_VERIFICATION_TEST)
    DBG_BLUE("[%s][T%zu] {%u}", prefix, tid, key);
#else
    DBG_BLUE("[%s][T%zu] {%u, %u, %u}", prefix, tid, key.ssid, key.tsid,
             key.tclass);
#endif
}


static inline user_key_t
user_key_rand_gen(void)
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

#endif
