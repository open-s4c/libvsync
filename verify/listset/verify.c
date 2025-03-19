/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#define NTHREADS 3U
#include <test/listset/listset_test_interface.h>
#include <test/boilerplate/test_case.h>

void
init(void)
{
    lsts_init();
}

void
fini(void)
{
    lsts_destroy();
}

void
reg(vsize_t tid)
{
    lst_reg(tid);
}

void
dereg(vsize_t tid)
{
    lst_dereg(tid);
}

int
main(void)
{
    tc();
    return 0;
}
