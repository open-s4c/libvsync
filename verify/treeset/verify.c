/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#define NTHREADS 4U
#include <test/map/itreeset.h>
#include <test/map/treeset_test_interface.h>
#include <test/boilerplate/test_case.h>

void
init(void)
{
    tr_init();
}

void
fini(void)
{
    tr_destroy();
}

void
reg(vsize_t tid)
{
    V_UNUSED(tid);
}
void
dereg(vsize_t tid)
{
    V_UNUSED(tid);
}

int
main(void)
{
    tc();
    return 0;
}
