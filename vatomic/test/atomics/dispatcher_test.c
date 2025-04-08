/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <vsync/atomic.h>
#include <vsync/atomic/dispatch.h>
#define NEW_VALUE 2U

void
test8(void)
{
    vatomic8_t x = {0};
    vuint8_t o   = 0;

    vatomic_init(&x, 1U);
    o = vatomic_cmpxchg_rel(&x, 1U, NEW_VALUE);
    o = vatomic_xchg_rel(&x, 1U);
    o = vatomic_get_add_rlx(&x, 1U);
    o = vatomic_add_get(&x, 1U);
    vatomic_add(&x, 1U);
    o = vatomic_read(&x);
    vatomic_write_rel(&x, 0U);

    (void)o;
}

void
test16(void)
{
    vatomic16_t x = {0};
    vuint16_t o   = 0;

    vatomic_init(&x, 1U);
    o = vatomic_cmpxchg_rel(&x, 1U, NEW_VALUE);
    o = vatomic_xchg_rel(&x, 1U);
    o = vatomic_get_add_rlx(&x, 1U);
    o = vatomic_add_get(&x, 1U);
    vatomic_add(&x, 1U);
    o = vatomic_read(&x);
    vatomic_write_rel(&x, 0U);

    (void)o;
}

void
test32(void)
{
    vatomic32_t x = {0};
    vuint32_t o   = 0;

    vatomic_init(&x, 1U);
    o = vatomic_cmpxchg_rel(&x, 1U, NEW_VALUE);
    o = vatomic_xchg_rel(&x, 1U);
    o = vatomic_get_add_rlx(&x, 1U);
    o = vatomic_add_get(&x, 1U);
    vatomic_add(&x, 1U);
    o = vatomic_read(&x);
    vatomic_write_rel(&x, 0U);

    (void)o;
}

void
testptr(void)
{
    vatomicptr_t x = {0};
    void *o        = NULL;

    vatomic_init(&x, 0U);
    o = vatomic_cmpxchg(&x, (void *)1, (void *)NEW_VALUE);
    o = vatomic_xchg(&x, (void *)1);
    o = vatomic_read(&x);
    vatomic_write(&x, 0);

    (void)o;
}
void
test64(void)
{
    vatomic64_t x = {0};
    vuint64_t o   = 0;

    vatomic_init(&x, 1UL);
    o = vatomic_cmpxchg(&x, 1UL, NEW_VALUE);
    o = vatomic_xchg(&x, 1UL);
    o = vatomic_get_add_rlx(&x, 1UL);
    o = vatomic_get_add_rlx(&x, 1UL);
    o = vatomic_add_get(&x, 1UL);
    vatomic_add(&x, 1UL);
    o = vatomic_read(&x);

    (void)o;
}

void
testsz(void)
{
    vatomicsz_t x = {0};
    vsize_t o     = 0;

    vatomic_init(&x, 1UL);
    o = vatomic_cmpxchg(&x, 1UL, NEW_VALUE);
    o = vatomic_xchg(&x, 1UL);
    o = vatomic_get_add_rlx(&x, 1UL);
    o = vatomic_get_add_rlx(&x, 1UL);
    o = vatomic_add_get(&x, 1UL);
    vatomic_add(&x, 1UL);
    o = vatomic_read(&x);

    (void)o;
}

int
main(void)
{
    test8();
    test16();
    test32();
    test64();
    testsz();
    testptr();
    return 0;
}
