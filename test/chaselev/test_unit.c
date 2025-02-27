/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <vsync/atomic.h>
#include <vsync/common/assert.h>
#include <vsync/queue/chaselev.h>

#define NUM_ENTRIES 1U

vdeque_t g_vdeque;

void
test_vqueue_cl(void)
{
    vsize_t array_size = vdeque_memsize(NUM_ENTRIES);
    ASSERT(array_size == sizeof(vdeque_array_t) + sizeof(void *));
    vdeque_array_t *a = (vdeque_array_t *)malloc(array_size);
    vdeque_state_t status;

    ASSERT(a != NULL);
    vdeque_init(&g_vdeque, a, 1);
    ASSERT(vatomicsz_read_rlx(&a->size) == 1);
    ASSERT(vatomicsz_read_rlx(&g_vdeque.top) == 0);
    ASSERT(vatomicsz_read_rlx(&g_vdeque.bottom) == 0);

    // Test vdeque_push_bottom
    vsize_t v1 = 1;
    ASSERT(vatomicsz_read_rlx(&g_vdeque.top) == 0);
    ASSERT(vatomicsz_read_rlx(&g_vdeque.bottom) == 0);
    status = vdeque_push_bottom(&g_vdeque, &v1);
    ASSERT(status == VDEQUE_STATE_OK);
    ASSERT(vatomicsz_read_rlx(&g_vdeque.top) == 0);
    ASSERT(vatomicsz_read_rlx(&g_vdeque.bottom) == 1);

    // Test vdeque_pop_bottom
    void *v2 = NULL;
    ASSERT(vatomicsz_read_rlx(&g_vdeque.top) == 0);
    ASSERT(vatomicsz_read_rlx(&g_vdeque.bottom) == 1);
    status = vdeque_pop_bottom(&g_vdeque, &v2);
    ASSERT(status == VDEQUE_STATE_OK);
    ASSERT(vatomicsz_read_rlx(&g_vdeque.top) == 1);
    ASSERT(vatomicsz_read_rlx(&g_vdeque.bottom) == 1);
    ASSERT(*(vsize_t *)v2 == v1);

    // Test vdeque_steal
    void *v3 = NULL;
    ASSERT(vatomicsz_read_rlx(&g_vdeque.top) == 1);
    ASSERT(vatomicsz_read_rlx(&g_vdeque.bottom) == 1);
    status = vdeque_steal(&g_vdeque, &v3);
    ASSERT(status == VDEQUE_STATE_EMPTY);
    ASSERT(vatomicsz_read_rlx(&g_vdeque.top) == 1);
    ASSERT(vatomicsz_read_rlx(&g_vdeque.bottom) == 1);

    V_UNUSED(status);
}

int
main(void)
{
    test_vqueue_cl();
    return 0;
}
