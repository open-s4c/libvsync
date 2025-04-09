/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <assert.h>
#include <vsync/atomic.h>

int
main(void)
{
    vatomic_cpu_pause();

    return 0;
}
