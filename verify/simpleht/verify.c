/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#define NTHREADS 3U
#include <test/map/isimple.h>
#include <test/boilerplate/test_case.h>

void
reg(vsize_t tid)
{
    imap_reg(tid);
}
void
dereg(vsize_t tid)
{
    imap_dereg(tid);
}

void
init(void)
{
    imap_init();
}
void
fini(void)
{
    imap_destroy();
}

int
main(void)
{
    tc();
    return 0;
}
