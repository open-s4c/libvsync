/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef SMR_MAX_NTHREADS
    #define SMR_MAX_NTHREADS 3U
#endif
#define NTHREADS SMR_MAX_NTHREADS
#include <test/boilerplate/test_case.h>

void
init(void)
{
    ismr_init();
}

void
fini(void)
{
    ismr_destroy();
    ASSERT(mock_node_no_leak());
}

void
reg(vsize_t tid)
{
    ismr_reg(tid);
}

void
dereg(vsize_t tid)
{
    ismr_dereg(tid);
}

int
main(void)
{
    tc();
    return 0;
}
