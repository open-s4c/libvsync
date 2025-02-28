/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <pthread.h>
#include <vsync/atomic.h>
#include <vsync/common/assert.h>
#include <vsync/queue/chaselev.h>

#define NUM_ENTRIES 1U

vdeque_t g_vdeque;

void
test_vdeque_single_threaded(vdeque_t *vdeque)
{
    vdeque_state_t status;
    vuint32_t obj = 0;

    // Test vdeque_pop_bottom
    void *value;
    status = vdeque_pop_bottom(vdeque, &value);
    ASSERT(status == VDEQUE_STATE_EMPTY);
    ASSERT(vatomicsz_read_rlx(&vdeque->bottom) == 0);
    ASSERT(vatomicsz_read_rlx(&vdeque->top) == 0);

    // Test vdeque_push_bottom
    status = vdeque_push_bottom(vdeque, &obj);
    ASSERT(status == VDEQUE_STATE_OK);
    ASSERT(vatomicsz_read_rlx(&vdeque->bottom) == 1);
    ASSERT(vatomicsz_read_rlx(&vdeque->top) == 0);

    // Test vdeque_steal
    status = vdeque_steal(vdeque, &value);
    ASSERT(status == VDEQUE_STATE_OK);
    ASSERT(vatomicsz_read_rlx(&vdeque->bottom) == 1);
    ASSERT(vatomicsz_read_rlx(&vdeque->top) == 1);

    V_UNUSED(status);
}

int
main(void)
{
    vsize_t array_size = vdeque_memsize(NUM_ENTRIES);
    ASSERT(array_size == sizeof(vdeque_array_t) + sizeof(void *));
    vdeque_array_t *a = (vdeque_array_t *)malloc(array_size);

    ASSERT(a != NULL);
    vdeque_init(&g_vdeque, a, 1);
    test_vdeque_single_threaded(&g_vdeque);

    return 0;
}
