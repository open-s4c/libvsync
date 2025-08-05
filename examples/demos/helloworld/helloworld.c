/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/atomic.h>
#include <stdio.h>

int
main()
{
    vatomic64_t x             = VATOMIC_INIT(0);
    const vuint64_t old_value = vatomic64_cmpxchg(&x, 0, 1);
    const vuint64_t new_value = vatomic64_read(&x);
    printf("Hello libvsync! New value: %" PRIu64 ", old value: %" PRIu64 ".\n",
           new_value, old_value);
    return 0;
}
