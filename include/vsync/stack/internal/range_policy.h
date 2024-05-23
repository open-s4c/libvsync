/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_RANGE_POLICY_H
#define VSYNC_RANGE_POLICY_H
#include <vsync/common/assert.h>
#include <vsync/vtypes.h>

typedef struct range_policy_s {
    vuint32_t max;
    vuint32_t range;
    vuint32_t successful_operations;
    vuint32_t failed_operations;
} range_policy_t;

/**
 * Initializes the given `policy`
 *
 * @param policy
 * @param max maximum allowed range
 */
static inline void
range_policy_record_init(range_policy_t *policy, vuint32_t max)
{
    ASSERT(policy);
    policy->range                 = max;
    policy->max                   = max;
    policy->successful_operations = 0;
    policy->failed_operations     = 0;
    ASSERT(policy->range <= policy->max);
}
/**
 * Returns the current range
 *
 * @param policy address of `range_policy_t` object
 * @return vuint32_t current range
 */
static inline vuint32_t
range_policy_get_range(range_policy_t *policy)
{
    ASSERT(policy);
    return policy->range;
}
/**
 * Records a successful operation, and expands the range as a result
 *
 * @param policy address of `range_policy_t` object
 */
static inline void
range_policy_record_success(range_policy_t *policy)
{
    ASSERT(policy);
    policy->successful_operations++;

    if (policy->range < policy->max) {
        policy->range++;
    }
}
/**
 * Records a failed operation, and shrinks the range as a result
 *
 * @param policy address of `range_policy_t` object
 */
static inline void
range_policy_record_fail(range_policy_t *policy)
{
    ASSERT(policy);
    policy->failed_operations++;

    if (policy->range > 0) {
        policy->range--;
    }
}

#endif
