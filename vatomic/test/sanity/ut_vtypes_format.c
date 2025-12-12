/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <vsync/vtypes.h>
#include <stdio.h>

int
main(void)
{
    vuint32_t v32   = VUINT32_MAX;
    vuint64_t v64   = VUINT64_MAX;
    vuintptr_t vptr = VUINTPTR_MAX;

    printf("VUINT32_MAX = %" VUINT32_FORMAT "\n", v32);
    printf("VUINT64_MAX = %" VUINT64_FORMAT "\n", v64);
    printf("VUINTPTR_FORMAT = %" VUINTPTR_FORMAT "\n", vptr);
    return 0;
}
