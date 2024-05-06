/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_VBITMAP_H
#define VSYNC_VBITMAP_H
/*******************************************************************************
 * @file  bitmap.h
 * @brief A bitmap implementation.
 *
 * Bitmap with basic functionality.
 *
 * @example
 * @include eg_bitmap.c
 *
 * @cite
 * The interface is inspired by and similar to [ck_bitmap]
 * (https://github.com/concurrencykit/ck/blob/master/include/ck_bitmap.h)
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/utils/math.h>
#include <vsync/common/dbg.h>

typedef struct vbitmap_s {
    vsize_t maps_cnt;
    vsize_t bit_cnt;
    vatomic64_t maps[];
} vbitmap_t;

typedef struct vbitmap_iter_s {
    vbitmap_t *map;
    vsize_t idx;
    vuint64_t last_val;
} vbitmap_iter_t;

#define VBITMAP_CALC_SLOT_IDX(_i_)     ((_i_) / ((vsize_t)VUINT64_WIDTH))
#define VBITMAP_CALC_BIT_SLOT_IDX(_i_) ((_i_) % ((vsize_t)VUINT64_WIDTH))
#define VBITMAP_BIT_MASK(_i_)          (((vuint64_t)1) << VBITMAP_CALC_BIT_SLOT_IDX(_i_))

/**
 * Returns the mask for setting bits in [idx: 63].
 *
 * @param idx LSB index.
 * @return vuint64_t mask.
 */
static inline vuint64_t
_vbitmap_lsb_mask(vsize_t idx)
{
    vuint64_t mask = 1;
    mask           = mask << ((vuint64_t)idx);
    mask--;
    return ~mask;
}
/**
 * Returns the mask for setting bits in [0: idx].
 *
 * @param idx MSB index.
 * @return vuint64_t mask.
 */
static inline vuint64_t
_vbitmap_msb_mask(vsize_t idx)
{
    vuint64_t mask = 1;
    mask           = mask << ((vuint64_t)idx);
    mask           = mask + (mask - 1);
    return mask;
}
/**
 * Returns mask for setting bits in [from, to].
 *
 * @param from LSB index.
 * @param to MSB index.
 * @return vuint64_t mask.
 */
static inline vuint64_t
_vbitmap_range_mask(vsize_t from, vsize_t to)
{
    vuint64_t lsb  = _vbitmap_lsb_mask(from);
    vuint64_t msb  = _vbitmap_msb_mask(to);
    vuint64_t mask = msb & lsb;
    return mask;
}
/**
 * Calculates the size of vbitmap_t object based on the given number of bits.
 *
 * @param bit_count number of bits.
 * @return vsize_t size of vbitmap_t object needed to accommodate `bit_count` of
 * bits.
 */
static inline vsize_t
vbitmap_size(vsize_t bit_count)
{
    vsize_t arr_sz = VCEIL_DIV(bit_count, VUINT64_WIDTH) * sizeof(vatomic64_t);
    return (sizeof(vbitmap_t) + arr_sz);
}
/**
 * Initializes the given `bitmap` object.
 *
 * All bits are unset initially.
 *
 * @param bitmap address of vbitmap_t object.
 * @param bit_count number of bits.
 * @param set true if all bits should be set, false otherwise.
 */
static inline void
vbitmap_init(vbitmap_t *bitmap, vsize_t bit_count, vbool_t set)
{
    vuint64_t init_val = set ? VUINT64_MAX : 0U;
    bitmap->bit_cnt    = bit_count;
    bitmap->maps_cnt   = VCEIL_DIV(bit_count, VUINT64_WIDTH);
    for (vsize_t i = 0; i < bitmap->maps_cnt; i++) {
        vatomic64_init(&bitmap->maps[i], init_val);
    }
}
/**
 * Clears all bits in the given bitmap.
 *
 * @param bitmap address of vbitmap_t object.
 */
static inline void
vbitmap_clear(vbitmap_t *bitmap)
{
    for (vsize_t i = 0; i < bitmap->maps_cnt; i++) {
        vatomic64_write(&bitmap->maps[i], 0U);
    }
}
/**
 * Returns the value of the bit at index `bit_idx`.
 *
 * @param bitmap address of vbitmap_t object.
 * @param bit_idx index of the bit to set.
 * @return true the bit is set.
 * @return false the bit is unset.
 */
static inline vbool_t
vbitmap_get(vbitmap_t *bitmap, vsize_t bit_idx)
{
    ASSERT(bitmap);
    ASSERT(bit_idx < bitmap->bit_cnt);
    vsize_t idx = VBITMAP_CALC_SLOT_IDX(bit_idx);
    ASSERT(idx < bitmap->maps_cnt);
    vuint64_t mask = VBITMAP_BIT_MASK(bit_idx);
    vuint64_t val  = vatomic64_read(&bitmap->maps[idx]);
    vbool_t is_set = (val & mask) == mask;
    return is_set;
}
/**
 * Sets the bit at index `bit_idx`.
 *
 * @param bitmap address of vbitmap_t object.
 * @param bit_idx index of the bit to set.
 */
static inline void
vbitmap_set_bit(vbitmap_t *bitmap, vsize_t bit_idx)
{
    ASSERT(bitmap);
    ASSERT(bit_idx < bitmap->bit_cnt);
    vsize_t idx = VBITMAP_CALC_SLOT_IDX(bit_idx);
    ASSERT(idx < bitmap->maps_cnt);
    vuint64_t mask = VBITMAP_BIT_MASK(bit_idx);
    vuint64_t val  = vatomic64_or_get(&bitmap->maps[idx], mask);
    ASSERT((val & mask) == mask);
}
/**
 * Sets bits in slot idx using the given mask.
 *
 * @param bitmap address of vbitmap_t object.
 * @param idx slot index.
 * @param mask mask of bits to set.
 */
static inline void
_vbitbmap_set_slot(vbitmap_t *bitmap, vsize_t idx, vuint64_t mask)
{
    ASSERT(idx < bitmap->maps_cnt);
    vatomic64_or(&bitmap->maps[idx], mask);
}
/**
 * Clears bits in slot idx using the given mask.
 *
 * @param bitmap address of vbitmap_t object.
 * @param idx slot index.
 * @param mask mask of bits to clear.
 */
static inline void
_vbitbmap_clr_slot(vbitmap_t *bitmap, vsize_t idx, vuint64_t mask)
{
    ASSERT(idx < bitmap->maps_cnt);
    vatomic64_and(&bitmap->maps[idx], mask);
}
/**
 * Sets all bits with indexes that are in range `[from:to]`.
 *
 * @note the range is inclusive both bits at index `from` and index `to` will be
 * set.
 *
 * @param bitmap address of vbitmap_t object.
 * @param from index of the LSB in the range.
 * @param to index of the MSB in the range.
 *
 * @note This function is not atomic. Bits inside the range might not be
 * raised at once. Raising bits occur in multiple steps.
 */
static inline void
vbitmap_set_range(vbitmap_t *bitmap, vsize_t from, vsize_t to)
{
    vsize_t lsb_slot = 0;
    vsize_t msb_slot = 0;
    vsize_t lsb_idx  = 0;
    vsize_t msb_idx  = 0;
    vuint64_t mask   = 0;

    ASSERT(bitmap);
    ASSERT(from <= to);
    ASSERT(to < bitmap->bit_cnt);

    lsb_slot = VBITMAP_CALC_SLOT_IDX(from);
    msb_slot = VBITMAP_CALC_SLOT_IDX(to);
    lsb_idx  = VBITMAP_CALC_BIT_SLOT_IDX(from);
    msb_idx  = VBITMAP_CALC_BIT_SLOT_IDX(to);

    ASSERT(lsb_slot <= msb_slot);
    ASSERT(msb_slot < bitmap->maps_cnt);

    /* LSB and MSB are within the same slot */
    if (lsb_slot == msb_slot) {
        mask = _vbitmap_range_mask(lsb_idx, msb_idx);
        _vbitbmap_set_slot(bitmap, lsb_slot, mask);
    } else {
        mask = _vbitmap_lsb_mask(lsb_idx);
        _vbitbmap_set_slot(bitmap, lsb_slot, mask);
        for (vsize_t i = lsb_slot + 1; i < msb_slot; i++) {
            vatomic64_write(&bitmap->maps[i], VUINT64_MAX);
        }
        mask = _vbitmap_msb_mask(msb_idx);
        _vbitbmap_set_slot(bitmap, msb_slot, mask);
    }
}
/**
 * Clears all bits with indexes that are in range `[from:to]`.
 *
 * @note the range is inclusive both bits at index `from` and index `to` will be
 * set.
 *
 * @param bitmap address of vbitmap_t object.
 * @param from index of the LSB in the range.
 * @param to index of the MSB in the range.
 *
 * @note This function is not atomic. Bits inside the range might not be
 * cleared at once. Clearing bits occur in multiple steps.
 */
static inline void
vbitmap_clr_range(vbitmap_t *bitmap, vsize_t from, vsize_t to)
{
    vsize_t lsb_slot = 0;
    vsize_t msb_slot = 0;
    vsize_t lsb_idx  = 0;
    vsize_t msb_idx  = 0;
    vuint64_t mask   = 0;

    ASSERT(bitmap);
    ASSERT(from <= to);
    ASSERT(to < bitmap->bit_cnt);

    lsb_slot = VBITMAP_CALC_SLOT_IDX(from);
    msb_slot = VBITMAP_CALC_SLOT_IDX(to);
    lsb_idx  = VBITMAP_CALC_BIT_SLOT_IDX(from);
    msb_idx  = VBITMAP_CALC_BIT_SLOT_IDX(to);

    ASSERT(lsb_slot <= msb_slot);
    ASSERT(msb_slot < bitmap->maps_cnt);

    /* LSB and MSB are within the same slot */
    if (lsb_slot == msb_slot) {
        mask = ~_vbitmap_range_mask(lsb_idx, msb_idx);
        _vbitbmap_clr_slot(bitmap, lsb_slot, mask);
    } else {
        mask = ~_vbitmap_lsb_mask(lsb_idx);
        _vbitbmap_clr_slot(bitmap, lsb_slot, mask);
        for (vsize_t i = lsb_slot + 1; i < msb_slot; i++) {
            vatomic64_write(&bitmap->maps[i], 0UL);
        }
        mask = ~_vbitmap_msb_mask(msb_idx);
        _vbitbmap_clr_slot(bitmap, msb_slot, mask);
    }
}
/**
 * Clears the bit at index `bit_idx`.
 *
 * @param bitmap address of vbitmap_t object.
 * @param bit_idx index of the bit to unset.
 */
static inline void
vbitmap_clr_bit(vbitmap_t *bitmap, vsize_t bit_idx)
{
    ASSERT(bitmap);
    ASSERT(bit_idx < bitmap->bit_cnt);
    vsize_t idx = VBITMAP_CALC_SLOT_IDX(bit_idx);
    ASSERT(idx < bitmap->maps_cnt);
    vuint64_t mask = VBITMAP_BIT_MASK(bit_idx);
    vuint64_t val  = vatomic64_and_get(&bitmap->maps[idx], ~mask);
    ASSERT((val & mask) == 0UL);
}
/**
 * Initializes the given vbitmap_iter_t object.
 *
 * @param bitmap address of vbitmap_t to iterate over.
 * @param iter address of vbitmap_iter_t object.
 */
static inline void
vbitmap_iter_init(vbitmap_t *bitmap, vbitmap_iter_t *iter)
{
    ASSERT(bitmap);
    ASSERT(iter);
    ASSERT(bitmap->maps_cnt > 0);

    iter->map      = bitmap;
    iter->idx      = 0;
    iter->last_val = vatomic64_read(&bitmap->maps[0]);
}
/**
 * Iterates to the next set bit.
 *
 * @param iter address of vbitmap_iter_t object.
 * @param out_bit_idx index of the next set bit.
 * @return true next set bit is found.
 * @return false no more set bits are found.
 *
 * @pre call `vbitmap_iter_init` to initialize `iter` before first use.
 */
static inline vbool_t
vbitmap_iter_next(vbitmap_iter_t *iter, vsize_t *out_bit_idx)
{
    vatomic64_t *maps = iter->map->maps;
    vsize_t maps_cnt  = iter->map->maps_cnt;
    vsize_t bit_idx   = 0;
    vsize_t idx       = 0;
    vsize_t out_idx   = 0;

    if (iter->last_val == 0) {
        /* search for the next slot where there is a bit set */
        for (idx = iter->idx + 1; idx < maps_cnt; idx++) {
            iter->last_val = vatomic64_read(&maps[idx]);
            if (iter->last_val) {
                iter->idx = idx;
                goto VBITMAP_LBL_VAL;
            }
        }
        /* no such slot is found */
        return false;
    }

    /* process non zero val, find the set bit */
VBITMAP_LBL_VAL:
    /* find the index of the first set bit */
    bit_idx = (vsize_t)__builtin_ctzll(iter->last_val);
    out_idx = (VUINT64_WIDTH * iter->idx) + bit_idx;
    if (out_idx >= iter->map->bit_cnt) {
        return false;
    }
    /* calc and save the set bit index */
    *out_bit_idx = out_idx;
    /* save a copy of the value without the currently detected bit */
    iter->last_val &= ~VBITMAP_BIT_MASK(bit_idx);
    return true;
}
#undef VBITMAP_CALC_SLOT_IDX
#undef VBITMAP_CALC_BIT_SLOT_IDX
#undef VBITMAP_BIT_MASK
#endif
