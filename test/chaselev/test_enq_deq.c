/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <vsync/atomic.h>
#include <vsync/common/assert.h>
#include <vsync/queue/chaselev.h>

#define NUM_ENTRIES 3U

vdeque_t g_vdeque;

void
test_push_pop(void)
{
    vuint32_t array_size = vdeque_memsize(NUM_ENTRIES);
    vdeque_array_t *a    = (vdeque_array_t *)malloc(array_size);

    ASSERT(a != NULL);
    vdeque_init(&g_vdeque, a, NUM_ENTRIES);

    vuint32_t num_push = 0, num_pop = 0;

    vuint32_t v1 = 1;
    vdeque_state_t status;
    while (vdeque_push_bottom(&g_vdeque, &v1) == VDEQUE_STATE_OK) {
        num_push++;
    }
    status = vdeque_push_bottom(&g_vdeque, &v1);
    ASSERT(status == VDEQUE_STATE_FULL);
    ASSERT(num_push == NUM_ENTRIES);

    void *v2 = NULL;
    while (vdeque_pop_bottom(&g_vdeque, &v2) == VDEQUE_STATE_OK) {
        num_pop++;
    }
    status = vdeque_pop_bottom(&g_vdeque, &v2);
    ASSERT(status == VDEQUE_STATE_EMPTY);

    V_UNUSED(status);

    ASSERT(num_push == num_pop);
}

int
main(void)
{
    test_push_pop();
    return 0;
}
