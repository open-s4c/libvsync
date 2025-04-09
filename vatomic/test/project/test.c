/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <vsync/atomic.h>
#include <stdio.h>

vatomic32_t count;

int
main()
{
    vatomic_inc(&count);
    printf("count: %u\n", vatomic_read(&count));
    return 0;
}
