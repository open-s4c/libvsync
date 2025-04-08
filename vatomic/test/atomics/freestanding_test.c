/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#define VSYNC_ENABLE_FREESTANDING
#include <vsync/atomic.h>

void
test_vuintptr(void)
{
    int x         = 1337;
    int *p        = &x;
    vuintptr_t p2 = (vuintptr_t)p;
    (void)p2;
}
int
main(void)
{
    return 0;
}
