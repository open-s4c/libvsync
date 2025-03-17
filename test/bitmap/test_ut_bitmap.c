/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/bitmap/bitmap.h>
#include <vsync/utils/math.h>
#include <vsync/common/dbg.h>
#include <stdlib.h>
#include <stdio.h>

#define VBITMAP_ENTRY_COUNT     1000U
#define VBITMAP_ENTRY_COUNT_MAX 92274688U

vbitmap_t *
_create(vsize_t len)
{
    vsize_t size      = vbitmap_size(len);
    vbitmap_t *bitmap = (vbitmap_t *)malloc(size);
    vbitmap_init(bitmap, len, false);
    return bitmap;
}

void
_destroy(vbitmap_t *bitmap)
{
    free(bitmap);
}

void
ut_iter_set(void)
{
    vbitmap_iter_t iter = {0};
    vsize_t len         = (VUINT64_WIDTH * 2) + 2;
    vsize_t size        = vbitmap_size(len);
    vbitmap_t *bitmap   = (vbitmap_t *)malloc(size);
    vsize_t idx         = 0;
    vsize_t expected    = 0;

    vbitmap_init(bitmap, len, true);
    vbitmap_iter_init(bitmap, &iter);

    while (vbitmap_iter_next(&iter, &idx)) {
        ASSERT(expected == idx);
        ASSERT(idx < len);
        expected++;
    }
    vbool_t set = vbitmap_iter_next(&iter, &idx);
    ASSERT(!set);
    ASSERT(idx == len - 1);
    free(bitmap);
}

void
ut_set_range_funs(void)
{
    vsize_t len       = VBITMAP_ENTRY_COUNT_MAX;
    vbitmap_t *bitmap = NULL;
    vsize_t from_idx  = 1;
    vsize_t to_idx    = len - 2;
    vuint64_t val     = 0;

    bitmap = _create(len);
    vbitmap_set_range(bitmap, from_idx, to_idx);

    val = vatomic64_read(&bitmap->maps[0]);

    ASSERT(val == VUINT64_MAX - 1);

    for (vsize_t i = 1; i < bitmap->maps_cnt - 1; i++) {
        vuint64_t val = vatomic64_read(&bitmap->maps[i]);
        ASSERT(val == VUINT64_MAX);
    }

    ASSERT(vatomic64_read(&bitmap->maps[bitmap->maps_cnt - 1]) ==
           ((VUINT64_MAX << 1U) >> 1U));

    _destroy(bitmap);
}

void
ut_range_1slot_funs(void)
{
    vsize_t len       = VUINT64_WIDTH;
    vbitmap_t *bitmap = NULL;
    bitmap            = _create(len);

#define PAIRS 8U
    vuint64_t indexes[PAIRS][3] = {
        {0, 63, VUINT64_MAX},           {1, 63, 0xFFFFFFFFFFFFFFFEUL},
        {0, 62, 0x7FFFFFFFFFFFFFFFUL},  {1, 62, 0x7FFFFFFFFFFFFFFEUL},
        {31, 32, 0x180000000UL},        {4, 60, 0x1FFFFFFFFFFFFFF0UL},
        {63, 63, 0x8000000000000000UL}, {0, 0, 0x0000000000000001UL}};

    for (vsize_t i = 0; i < PAIRS; i++) {
        vsize_t from       = indexes[i][0];
        vsize_t to         = indexes[i][1];
        vuint64_t expected = indexes[i][2];

        vatomic64_write(&bitmap->maps[0], 0);

        vbitmap_set_range(bitmap, from, to);

        vuint64_t val = vatomic64_read(&bitmap->maps[0]);

        DBG_GRAY("[TC-%zu] expected: %lx == real %lx", i, expected, val);
        ASSERT(expected == val);
    }

    for (vsize_t i = 0; i < PAIRS; i++) {
        vsize_t from       = indexes[i][0];
        vsize_t to         = indexes[i][1];
        vuint64_t expected = ~indexes[i][2];

        vatomic64_write(&bitmap->maps[0], VUINT64_MAX);

        vbitmap_clr_range(bitmap, from, to);

        vuint64_t val = vatomic64_read(&bitmap->maps[0]);

        DBG_GRAY("[TC-%zu] expected: %lx == real %lx", i, expected, val);
        ASSERT(expected == val);
    }

    _destroy(bitmap);
}

void
ut_range_multi_slot_funs(vsize_t len, vsize_t from, vsize_t to)
{
    vbitmap_t *bitmap = NULL;
    bitmap            = _create(len);
    vbool_t set       = false;

    vbitmap_set_range(bitmap, from, to);

    for (vsize_t i = 0; i < len; i++) {
        set = vbitmap_get(bitmap, i);
        if (i >= from && i <= to) {
            ASSERT(set);
        } else {
            ASSERT(!set);
        }
    }

    vbitmap_set_range(bitmap, 0, from);
    vbitmap_set_range(bitmap, from, len - 1);
    vbitmap_clr_range(bitmap, from, to);

    for (vsize_t i = 0; i < len; i++) {
        set = vbitmap_get(bitmap, i);
        if (i >= from && i <= to) {
            ASSERT(!set);
        } else {
            ASSERT(set);
        }
    }

    _destroy(bitmap);
}

void
ut_ranges(void)
{
    ut_range_multi_slot_funs(1, 0, 0);
    ut_range_multi_slot_funs(VUINT64_WIDTH, 1, VUINT64_WIDTH - 1);
    ut_range_multi_slot_funs((VUINT64_WIDTH * 3) + 1, 24, (VUINT64_WIDTH * 3));
}

void
ut_basic_funs(void)
{
    vbool_t is_set    = false;
    vbitmap_t *bitmap = _create(VBITMAP_ENTRY_COUNT);

    for (vsize_t i = 0; i < VBITMAP_ENTRY_COUNT; i++) {
        is_set = vbitmap_get(bitmap, i);
        ASSERT(is_set == false);
    }

    for (vsize_t i = 0; i < VBITMAP_ENTRY_COUNT; i++) {
        vbitmap_set_bit(bitmap, i);
        is_set = vbitmap_get(bitmap, i);
        ASSERT(is_set == true);
    }

    for (vsize_t i = 0; i < VBITMAP_ENTRY_COUNT; i++) {
        is_set = vbitmap_get(bitmap, i);
        ASSERT(is_set == true);
    }

    for (vsize_t i = 0; i < VBITMAP_ENTRY_COUNT; i++) {
        vbitmap_clr_bit(bitmap, i);
        is_set = vbitmap_get(bitmap, i);
        ASSERT(is_set == false);
    }

    for (vsize_t i = 0; i < VBITMAP_ENTRY_COUNT; i++) {
        is_set = vbitmap_get(bitmap, i);
        ASSERT(is_set == false);
    }

    for (vsize_t i = 0; i < VBITMAP_ENTRY_COUNT; i++) {
        if (VIS_EVEN(i)) {
            vbitmap_set_bit(bitmap, i);
        }
    }

    for (vsize_t i = 0; i < VBITMAP_ENTRY_COUNT; i++) {
        is_set = vbitmap_get(bitmap, i);
        ASSERT(is_set == VIS_EVEN(i));
    }

    for (vsize_t i = 0; i < VBITMAP_ENTRY_COUNT; i++) {
        if (VIS_EVEN(i)) {
            vbitmap_clr_bit(bitmap, i);
        } else {
            vbitmap_set_bit(bitmap, i);
        }
    }

    for (vsize_t i = 0; i < VBITMAP_ENTRY_COUNT; i++) {
        is_set = vbitmap_get(bitmap, i);
        ASSERT(is_set == VIS_ODD(i));
    }

    _destroy(bitmap);
}

void
ut_test_iterator_all_set(void)
{
    vbitmap_iter_t iter;
    vsize_t bit_idx = 0;
    vsize_t idx     = 0;

    vbitmap_t *bitmap = _create(VBITMAP_ENTRY_COUNT);

    for (vsize_t i = 0; i < VBITMAP_ENTRY_COUNT; i++) {
        vbitmap_set_bit(bitmap, i);
    }

    vbitmap_iter_init(bitmap, &iter);

    while (vbitmap_iter_next(&iter, &bit_idx)) {
        ASSERT(bit_idx == idx);
        idx++;
    }

    _destroy(bitmap);
}

void
ut_test_iterator_all_clr(void)
{
    vbitmap_iter_t iter;
    vsize_t bit_idx = 0;

    vbitmap_t *bitmap = _create(VBITMAP_ENTRY_COUNT);

    vbitmap_iter_init(bitmap, &iter);

    ASSERT(vbitmap_iter_next(&iter, &bit_idx) == false);

    _destroy(bitmap);
}

void
ut_test_iterator_partial_set_even(void)
{
    vbitmap_iter_t iter;
    vsize_t bit_idx = 0;
    vsize_t idx     = 0;

    vbitmap_t *bitmap = _create(VBITMAP_ENTRY_COUNT);

    for (vsize_t i = 0; i < VBITMAP_ENTRY_COUNT; i++) {
        if (VIS_EVEN(i)) {
            vbitmap_set_bit(bitmap, i);
        }
    }

    vbitmap_iter_init(bitmap, &iter);

    while (vbitmap_iter_next(&iter, &bit_idx)) {
        ASSERT(bit_idx == idx);
        idx += 2;
    }
    _destroy(bitmap);
}

void
ut_test_iterator_partial_set_odd(void)
{
    vbitmap_iter_t iter;
    vsize_t bit_idx = 0;
    vsize_t idx     = 1;

    vbitmap_t *bitmap = _create(VBITMAP_ENTRY_COUNT);

    for (vsize_t i = 0; i < VBITMAP_ENTRY_COUNT; i++) {
        if (VIS_ODD(i)) {
            vbitmap_set_bit(bitmap, i);
        }
    }

    vbitmap_iter_init(bitmap, &iter);

    while (vbitmap_iter_next(&iter, &bit_idx)) {
        ASSERT(bit_idx == idx);
        idx += 2;
    }
    _destroy(bitmap);
}
int
main(void)
{
    ut_basic_funs();
    ut_test_iterator_all_set();
    ut_test_iterator_all_clr();
    ut_test_iterator_partial_set_even();
    ut_test_iterator_partial_set_odd();
    ut_set_range_funs();
    ut_range_1slot_funs();
    ut_ranges();
    ut_iter_set();
}
