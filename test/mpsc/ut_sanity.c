/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <test/impsc.h>

void
ut_empty(void)
{
    ASSERT(is_empty());
    ASSERT(get_len() == 0);
    for (vsize_t i = 0; i < 10; i++) {
        ASSERT(deq() == 0);
    }
    ASSERT(is_empty());
    ASSERT(get_len() == 0);
}

void
ut_len(void)
{
    const vsize_t count = 10000;
    for (vsize_t i = 1; i <= count; i++) {
        enq(0, (vuintptr_t)i);
        ASSERT(get_len() == i);
    }
    ASSERT(!is_empty());
    ASSERT(get_len() == count);
    for (vsize_t i = 1; i <= count; i++) {
        ASSERT(deq() == i);
        if (get_len() != count - i) {
            fprintf(stderr, "%zu %zu\n", get_len(), count - i);
        }
        ASSERT(get_len() == (count - i));
    }
    ASSERT(is_empty());
}

int
main(void)
{
    init();
    ut_empty();
    ut_len();
    fini();
    return 0;
}
