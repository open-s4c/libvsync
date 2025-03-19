/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#define NTHREADS 3U
#include <test/prio/pqueue_interface.h>
#include <test/boilerplate/test_case.h>

void
init(void)
{
    pqueue_init();
}
void
fini(void)
{
    pqueue_destroy();
}
void
reg(vsize_t tid)
{
    pqueue_reg(tid);
}
void
dereg(vsize_t tid)
{
    pqueue_dereg(tid);
}
int
main(void)
{
    tc();
    return 0;
}
