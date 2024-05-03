/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/atomic.h>
#include <vsync/common/assert.h>

int
main(void)
{
    vatomic_cpu_pause();

    return 0;
}
