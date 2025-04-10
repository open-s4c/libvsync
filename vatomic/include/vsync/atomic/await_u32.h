/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VATOMIC_AWAIT_U32_H
#define VATOMIC_AWAIT_U32_H
/*******************************************************************************
 * @file await_u32.h
 * @brief Atomic await functions for vatomic32_t variables.
 *
 * This file declares and documents the atomic await functions operating on
 * vatomic32_t variables.
 *
 * Please do not include this file directly, instead use:
 * ```c
 * #include <vsync/atomic/await.h>
 * ```
 ******************************************************************************/
/* !!!Warning: File generated by tmplr; DO NOT EDIT.!!! */
#include <vsync/atomic/internal/types.h>

#ifndef VATOMIC_AWAIT_H
    #error "File included directly. Include <vsync/atomic/await.h> instead!"
#endif
#define vatomic32_await_true(_p_)  vatomic32_await_neq((_p_), 0)
#define vatomic32_await_false(_p_) vatomic32_await_eq((_p_), 0)

/**
 * Politely awaits the value pointed by a to be less than v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_lt(const vatomic32_t *a, vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_lt_acq(const vatomic32_t *a,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_lt_rlx(const vatomic32_t *a,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_le(const vatomic32_t *a, vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_le_acq(const vatomic32_t *a,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_le_rlx(const vatomic32_t *a,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_gt(const vatomic32_t *a, vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_gt_acq(const vatomic32_t *a,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_gt_rlx(const vatomic32_t *a,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_ge(const vatomic32_t *a, vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_ge_acq(const vatomic32_t *a,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_ge_rlx(const vatomic32_t *a,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_neq(const vatomic32_t *a, vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_neq_acq(const vatomic32_t *a,
                                                vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return value satisfying conditon
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_neq_rlx(const vatomic32_t *a,
                                                vuint32_t v);
/**
 * Politely awaits the value pointed by a to be equal to v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return last value before satisfying condition
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_eq(const vatomic32_t *a, vuint32_t v);
/**
 * Politely awaits the value pointed by a to be equal to v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return last value before satisfying condition
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_eq_acq(const vatomic32_t *a,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be equal to v.
 *
 * The return value is the first read value that satisfies the condition, as if
 * read with vatomic32_read(a).
 *
 * @param a atomic variable
 * @param v value to compare
 * @return last value before satisfying condition
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_eq_rlx(const vatomic32_t *a,
                                               vuint32_t v);

/**
 * Politely awaits the value pointed by a to be equal to c, adding v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_eq_add(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be equal to c, adding v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_eq_add_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be equal to c, adding v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_eq_add_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be equal to c, adding v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_eq_add_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be equal to c, subtracting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_eq_sub(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be equal to c, subtracting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_eq_sub_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be equal to c, subtracting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_eq_sub_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be equal to c, subtracting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_eq_sub_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be equal to c, setting v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_eq_set(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be equal to c, setting v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_eq_set_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be equal to c, setting v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_eq_set_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be equal to c, setting v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_eq_set_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than c, adding v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_neq_add(vatomic32_t *a, vuint32_t c,
                                                vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than c, adding v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_neq_add_acq(vatomic32_t *a, vuint32_t c,
                                                    vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than c, adding v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_neq_add_rel(vatomic32_t *a, vuint32_t c,
                                                    vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than c, adding v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_neq_add_rlx(vatomic32_t *a, vuint32_t c,
                                                    vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than c, subtracting v
 * to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_neq_sub(vatomic32_t *a, vuint32_t c,
                                                vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than c, subtracting v
 * to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_neq_sub_acq(vatomic32_t *a, vuint32_t c,
                                                    vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than c, subtracting v
 * to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_neq_sub_rel(vatomic32_t *a, vuint32_t c,
                                                    vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than c, subtracting v
 * to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_neq_sub_rlx(vatomic32_t *a, vuint32_t c,
                                                    vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than c, setting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_neq_set(vatomic32_t *a, vuint32_t c,
                                                vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than c, setting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_neq_set_acq(vatomic32_t *a, vuint32_t c,
                                                    vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than c, setting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_neq_set_rel(vatomic32_t *a, vuint32_t c,
                                                    vuint32_t v);
/**
 * Politely awaits the value pointed by a to be different than c, setting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_neq_set_rlx(vatomic32_t *a, vuint32_t c,
                                                    vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than c, adding v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_lt_add(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than c, adding v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_lt_add_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than c, adding v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_lt_add_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than c, adding v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_lt_add_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than c, subtracting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_lt_sub(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than c, subtracting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_lt_sub_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than c, subtracting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_lt_sub_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than c, subtracting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_lt_sub_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than c, setting v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_lt_set(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than c, setting v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_lt_set_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than c, setting v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_lt_set_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than c, setting v to a once
 * the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_lt_set_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to c, adding
 * v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_le_add(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to c, adding
 * v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_le_add_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to c, adding
 * v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_le_add_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to c, adding
 * v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_le_add_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to c,
 * subtracting v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_le_sub(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to c,
 * subtracting v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_le_sub_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to c,
 * subtracting v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_le_sub_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to c,
 * subtracting v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_le_sub_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to c, setting
 * v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_le_set(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to c, setting
 * v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_le_set_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to c, setting
 * v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_le_set_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be less than or equal to c, setting
 * v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_le_set_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than c, adding v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_gt_add(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than c, adding v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_gt_add_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than c, adding v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_gt_add_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than c, adding v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_gt_add_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than c, subtracting v to
 * a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_gt_sub(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than c, subtracting v to
 * a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_gt_sub_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than c, subtracting v to
 * a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_gt_sub_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than c, subtracting v to
 * a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_gt_sub_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than c, setting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_gt_set(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than c, setting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_gt_set_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than c, setting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_gt_set_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than c, setting v to a
 * once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_gt_set_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to c,
 * adding v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_ge_add(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to c,
 * adding v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_ge_add_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to c,
 * adding v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_ge_add_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to c,
 * adding v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_ge_add_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to c,
 * subtracting v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_ge_sub(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to c,
 * subtracting v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_ge_sub_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to c,
 * subtracting v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_ge_sub_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to c,
 * subtracting v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_ge_sub_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to c,
 * setting v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord seq_cst
 */
static inline vuint32_t vatomic32_await_ge_set(vatomic32_t *a, vuint32_t c,
                                               vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to c,
 * setting v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord acquire
 */
static inline vuint32_t vatomic32_await_ge_set_acq(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to c,
 * setting v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord release
 */
static inline vuint32_t vatomic32_await_ge_set_rel(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);
/**
 * Politely awaits the value pointed by a to be greater than or equal to c,
 * setting v to a once the condition is satisfied.
 *
 * @param a atomic variable
 * @param c value to compare
 * @param v value to apply the operation
 * @return old value
 * @memord relaxed
 */
static inline vuint32_t vatomic32_await_ge_set_rlx(vatomic32_t *a, vuint32_t c,
                                                   vuint32_t v);

#endif /* VATOMIC_AWAIT_TY_H */
