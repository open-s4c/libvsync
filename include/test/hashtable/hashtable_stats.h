/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VHASHTABLE_STATS_H
#define VHASHTABLE_STATS_H

typedef struct _ghost_ht_stats_s {
    double bucket_len_avg;
    vuint64_t bucket_len_min;
    vuint64_t bucket_len_max;
    vuint64_t bucket_len_sum;
} _ghost_ht_stats_t;

static inline _ghost_ht_stats_t
_vhashtable_get_stats(vhashtable_t *table)
{
    _ghost_ht_stats_t stats = {0};
    stats.bucket_len_min    = VUINT64_MAX;
    vuint64_t len           = 0;

    for (vsize_t i = 0; i < VHASHTABLE_BUCKET_COUNT; i++) {
        len = vbucket_get_length(&table->buckets[i]);
        stats.bucket_len_sum += len;
        stats.bucket_len_min = VMIN(stats.bucket_len_min, len);
        stats.bucket_len_max = VMAX(stats.bucket_len_min, len);
    }
    stats.bucket_len_avg =
        (double)stats.bucket_len_sum / VHASHTABLE_BUCKET_COUNT;

    return stats;
}

#endif
