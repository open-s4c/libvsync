/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/atomic.h>
#include <vsync/atomic/atomicptr_stamped.h>
#include <vsync/common/assert.h>
#include <stdio.h>
#include <vsync/vtypes.h>
#include <vsync/common/dbg.h>

/*******************************************************************************
 * @file  atomic_stamped_test.c
 * @brief unit test for atomic_stamped.h
 *
 * We only need to test that the public interface satisfies the requirements and
 * meets the expectations of the user. We also call the functions with all
 * available barriers to ensure that the interface exists for them. We don't
 * need multithreaded testing as we trust the integrity of atomicptr and the
 * tests available for it. Assuming the correctness of atomicptr passing this
 * unit test implies the correctness of atomicptr_stamped
 *
 ******************************************************************************/

typedef struct place_holder_s {
    vuint32_t value;
} __attribute__((aligned(128))) place_holder_t;

typedef enum { SC = 0, RLX, REL, ACQ, BARRIER_COUNT } barrier_t;

typedef void (*set_fun)(vatomicptr_stamped_t *, void *, vuint8_t);

typedef void *(*get_fun)(vatomicptr_stamped_t *, vuint8_t *);

typedef void *(*get_pointer_fun)(vatomicptr_stamped_t *);

typedef vbool_t (*attempt_stamp_fun)(vatomicptr_stamped_t *, void *, vuint8_t);

typedef vbool_t (*cmpxchg_fun)(vatomicptr_stamped_t *, void *, vuint8_t, void *,
                               vuint8_t);

#define LENGTH ((vsize_t)BARRIER_COUNT)

set_fun g_set[LENGTH] = {vatomicptr_stamped_set, vatomicptr_stamped_set_rlx,
                         vatomicptr_stamped_set_rel, NULL};

get_fun g_get[LENGTH] = {vatomicptr_stamped_get, vatomicptr_stamped_get_rlx,
                         NULL, vatomicptr_stamped_get_acq};

get_pointer_fun g_get_pointer[LENGTH] = {
    vatomicptr_stamped_get_pointer, vatomicptr_stamped_get_pointer_rlx, NULL,
    vatomicptr_stamped_get_pointer_acq};

cmpxchg_fun g_cmpxchg[LENGTH] = {
    vatomicptr_stamped_cmpxchg, vatomicptr_stamped_cmpxchg_rlx,
    vatomicptr_stamped_cmpxchg_rel, vatomicptr_stamped_cmpxchg_acq};

attempt_stamp_fun g_attempt[LENGTH] = {
    vatomicptr_stamped_attempt_stamp, vatomicptr_stamped_attempt_stamp_rlx,
    vatomicptr_stamped_attempt_stamp_rel, vatomicptr_stamped_attempt_stamp_acq};

#define STAMP_VAL          111U
#define MAX_STAMP_VAL      127U
#define OVERFLOW_STAMP_VAL 129U

static void
ut_set_stamped(vatomicptr_stamped_t *a, void *v, vuint8_t stamp,
               barrier_t barrier)
{
    vsize_t idx = (vsize_t)(barrier == ACQ ? SC : barrier);
    g_set[idx](a, v, stamp);
}

static void
ut_get_stamped(vatomicptr_stamped_t *a, void *e_v, vuint8_t e_stamp,
               barrier_t barrier)
{
    void *out_v;
    vuint8_t out_stamp;

    vsize_t idx = (vsize_t)(barrier == REL ? SC : barrier);
    out_v       = g_get_pointer[idx](a);
    ASSERT(out_v == e_v);

    // expect &x false
    out_v = g_get[idx](a, &out_stamp);
    ASSERT(out_v == e_v);
    ASSERT(out_stamp == e_stamp);

    V_UNUSED(out_v);
}

static void
ut_cmpxchg_stamped(vatomicptr_stamped_t *a, void *e_v, vuint8_t e_stamp,
                   void *v, vuint8_t stamp, vbool_t expect_success,
                   barrier_t barrier)
{
    vbool_t success = g_cmpxchg[barrier](a, e_v, e_stamp, v, stamp);
    if (expect_success) {
        ASSERT(success);
        ut_get_stamped(a, v, stamp, barrier);
    } else {
        ASSERT(!success);
    }
    V_UNUSED(success);
}

static void
ut_attempt_stamp(vatomicptr_stamped_t *a, void *e_v, vuint8_t stamp,
                 vbool_t expect_success, barrier_t barrier)
{
    vbool_t success = g_attempt[barrier](a, e_v, stamp);

    if (expect_success) {
        ASSERT(success);
        ut_get_stamped(a, e_v, stamp, barrier);
    } else {
        ASSERT(!success);
    }
    V_UNUSED(success);
}

static void
unit_test_sanity(barrier_t barrier)
{
    if ((vsize_t)barrier < LENGTH) {
        place_holder_t value = {.value = 10};
        place_holder_t *p    = &value;

        place_holder_t value2 = {.value = 50};
        place_holder_t *fake  = &value2;

        place_holder_t value3   = {.value = 100};
        place_holder_t *new_val = &value3;

        /* we don't care what value is there */
        value3.value = STAMP_VAL;
        vatomicptr_stamped_t m_ptr;
        vatomicptr_stamped_t *a = &m_ptr;

        ut_set_stamped(a, new_val, 0, barrier);
        ut_get_stamped(a, new_val, 0, barrier);

        ut_set_stamped(a, new_val, STAMP_VAL, barrier);
        ut_get_stamped(a, new_val, STAMP_VAL, barrier);

        ut_set_stamped(a, p, STAMP_VAL, barrier);
        ut_get_stamped(a, p, STAMP_VAL, barrier);

        // current address is p, current stamp is STAMP_VAL

        // (1) incorrect address, correct stamp
        ut_cmpxchg_stamped(a, fake, STAMP_VAL, new_val, 0, false, barrier);
        ut_attempt_stamp(a, fake, 0, false, barrier);
        // expect no change
        ut_get_stamped(a, p, STAMP_VAL, barrier);

        // (2) correct address, incorrect stamp
        ut_cmpxchg_stamped(a, p, 0, new_val, 0, false, barrier);
        ut_attempt_stamp(a, p, STAMP_VAL, true, barrier);
        // expect no change
        ut_get_stamped(a, p, STAMP_VAL, barrier);

        // (3) incorrect address, incorrect stamp
        ut_cmpxchg_stamped(a, fake, 0, new_val, 0, false, barrier);
        // expect no change
        ut_get_stamped(a, p, STAMP_VAL, barrier);

        // (4) correct address, correct stamp
        ut_cmpxchg_stamped(a, p, STAMP_VAL, new_val, MAX_STAMP_VAL, true,
                           barrier);
        // expect no change
        ut_get_stamped(a, new_val, MAX_STAMP_VAL, barrier);

        // test stamp overflow

        vbool_t success = g_cmpxchg[barrier](a, new_val, MAX_STAMP_VAL, new_val,
                                             OVERFLOW_STAMP_VAL);
        ASSERT(success);
        ut_get_stamped(a, new_val, 1, barrier);

        success = g_attempt[barrier](a, new_val, OVERFLOW_STAMP_VAL);
        ASSERT(success);
        ut_get_stamped(a, new_val, 1, barrier);

        V_UNUSED(success);
    }
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
    return 0;
}
