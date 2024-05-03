/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/common/assert.h>
#include <vsync/vtypes.h>
#include <stdio.h>
#include <vsync/atomic.h>
#include <vsync/atomic/atomicptr_markable.h>
#include <vsync/common/dbg.h>

/*******************************************************************************
 * @file  atomic_markable_test.c
 * @brief unit test for atomic_markable.h
 *
 * We only need to test that the public interface satisfies the requirements and
 * meets the expectations of the user. We also call the functions with all
 * available barriers to ensure that the interface exists for them. We don't
 * need multithreaded testing as we trust the integrity of atomicptr and the
 * tests available for it. Assuming the correctness of atomicptr passing this
 * unit test implies the correctness of atomicptr_markable
 *
 ******************************************************************************/

typedef enum { SC = 0, RLX, REL, ACQ, BARRIER_COUNT } barrier_t;

typedef void (*set_fun)(vatomicptr_markable_t *, void *, vbool_t);

typedef void *(*get_fun)(vatomicptr_markable_t *, vbool_t *);

typedef void *(*get_pointer_fun)(vatomicptr_markable_t *);

typedef vbool_t (*get_mark_fun)(vatomicptr_markable_t *);

typedef vbool_t (*attempt_mark_fun)(vatomicptr_markable_t *, void *, vbool_t);

typedef vbool_t (*cmpxchg_fun)(vatomicptr_markable_t *, void *, vbool_t, void *,
                               vbool_t);

#define LENGTH ((vsize_t)BARRIER_COUNT)

set_fun g_set[LENGTH] = {vatomicptr_markable_set, vatomicptr_markable_set_rlx,
                         vatomicptr_markable_set_rel, NULL};

get_fun g_get[LENGTH] = {vatomicptr_markable_get, vatomicptr_markable_get_rlx,
                         NULL, vatomicptr_markable_get_acq};

get_pointer_fun g_get_pointer[LENGTH] = {
    vatomicptr_markable_get_pointer, vatomicptr_markable_get_pointer_rlx, NULL,
    vatomicptr_markable_get_pointer_acq};

get_mark_fun g_get_mark[LENGTH] = {vatomicptr_markable_get_mark,
                                   vatomicptr_markable_get_mark_rlx, NULL,
                                   vatomicptr_markable_get_mark_acq};

cmpxchg_fun g_cmpxchg[LENGTH] = {
    vatomicptr_markable_cmpxchg, vatomicptr_markable_cmpxchg_rlx,
    vatomicptr_markable_cmpxchg_rel, vatomicptr_markable_cmpxchg_acq};

attempt_mark_fun g_attempt_mark[LENGTH] = {
    vatomicptr_markable_attempt_mark, vatomicptr_markable_attempt_mark_rlx,
    vatomicptr_markable_attempt_mark_rel, vatomicptr_markable_attempt_mark_acq};

static void
ut_set_markable(vatomicptr_markable_t *a, void *v, vbool_t mark,
                barrier_t barrier)
{
    vsize_t idx = (vsize_t)(barrier == ACQ ? SC : barrier);
    g_set[idx](a, v, mark);
}

static void
ut_get_markable(vatomicptr_markable_t *a, void *e_v, vbool_t e_mark,
                barrier_t barrier)
{
    void *out_v;
    vbool_t out_mark;

    vsize_t idx = (vsize_t)(barrier == REL ? SC : barrier);

    out_v = g_get_pointer[idx](a);
    ASSERT(out_v == e_v);

    // expect &x false
    out_v = g_get[idx](a, &out_mark);
    ASSERT(out_v == e_v);
    ASSERT(out_mark == e_mark);

    out_mark = g_get_mark[idx](a);
    ASSERT(out_mark == e_mark);

    V_UNUSED(out_v, out_mark);
}

static void
ut_attempt_markable(vatomicptr_markable_t *a, void *e_v, vbool_t m,
                    vbool_t expect_success, barrier_t barrier)
{
    vbool_t success = g_attempt_mark[(vsize_t)barrier](a, e_v, m);

    if (expect_success) {
        ASSERT(success);
        ut_get_markable(a, e_v, m, barrier);
    } else {
        ASSERT(!success);
    }
    V_UNUSED(success);
}

static void
ut_cmpxchg_markable(vatomicptr_markable_t *a, void *e_v, vbool_t e_m, void *v,
                    vbool_t m, vbool_t expect_success, barrier_t barrier)
{
    vbool_t success = g_cmpxchg[barrier](a, e_v, e_m, v, m);
    if (expect_success) {
        ASSERT(success);
        ut_get_markable(a, v, m, barrier);
    } else {
        ASSERT(!success);
    }
    V_UNUSED(success);
}

static void
unit_test_sanity(barrier_t barrier)
{
    int value = 10;
    int *p    = &value;

    int value2 = 50;
    int *fake  = &value2;

    int value3   = 100;
    int *new_val = &value3;

    vatomicptr_markable_t m_ptr;
    vatomicptr_markable_t *a = &m_ptr;

    ut_set_markable(a, new_val, false, barrier);
    ut_get_markable(a, new_val, false, barrier);

    ut_set_markable(a, new_val, true, barrier);
    ut_get_markable(a, new_val, true, barrier);

    ut_set_markable(a, p, true, barrier);
    ut_get_markable(a, p, true, barrier);

    // (1) incorrect address, unmark
    ut_attempt_markable(a, fake, false, false, barrier);
    // no update
    ut_get_markable(a, p, true, barrier);

    // (2) incorrect address, mark
    ut_attempt_markable(a, fake, true, false, barrier);
    // no update
    ut_get_markable(a, p, true, barrier);

    // (3) correct address, mark
    ut_attempt_markable(a, p, true, true, barrier);
    // mark should be updated, address intact
    ut_get_markable(a, p, true, barrier);

    // (4) correct address, unmark
    ut_attempt_markable(a, p, false, true, barrier);
    // mark should be updated, address intact
    ut_get_markable(a, p, false, barrier);

    // (1) incorrect address, correct mark
    ut_cmpxchg_markable(a, fake, false, new_val, false, false, barrier);
    // expect no change
    ut_get_markable(a, p, false, barrier);

    // (2) correct address, incorrect mark
    ut_cmpxchg_markable(a, p, true, new_val, false, false, barrier);
    // expect no change
    ut_get_markable(a, p, false, barrier);

    // (3) correct address, correct mark, change address only
    ut_cmpxchg_markable(a, p, false, new_val, false, true, barrier);
    // expected only address change
    ut_get_markable(a, new_val, false, barrier);

    // (4) correct address, correct mark, change mark only
    ut_cmpxchg_markable(a, new_val, false, new_val, true, true, barrier);
    // expected only mark change
    ut_get_markable(a, new_val, true, barrier);

    // (5) correct address, correct mark, change both
    ut_cmpxchg_markable(a, new_val, true, p, false, true, barrier);
    // expect both to change
    ut_get_markable(a, p, false, barrier);
}

int
main(void)
{
    for (vuint32_t b = SC; b < (vuint32_t)BARRIER_COUNT; b++) {
        barrier_t barrier = (barrier_t)b;

        if (barrier == RLX) {
            DBG_GREEN("testing relaxed");
        } else if (barrier == REL) {
            DBG_GREEN("testing release");
        } else if (barrier == SC) {
            DBG_GREEN("testing sc");
        } else {
            DBG_GREEN("testing acq");
        }
        unit_test_sanity(barrier);
    }
    DBG_GREEN("PASSED\n");
    return 0;
}
