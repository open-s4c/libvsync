/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#define NTHREADS 3U
#include <test/stack/stack_interface.h>
#include <test/boilerplate/test_case.h>

void
init(void)
{
    istack_init();
}
void
fini(void)
{
#ifndef SKIP_VERIFY
    verify(0);
#endif
    istack_destroy();
}

void
reg(vsize_t tid)
{
    istack_reg(tid);
}
void
dereg(vsize_t tid)
{
    istack_dereg(tid);
}

int
main(void)
{
    tc();
    return 0;
}
