/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef VATOMIC_INTERNAL_MACROS_H
#define VATOMIC_INTERNAL_MACROS_H

#include <vsync/vtypes.h>
#include <vsync/common/macros.h>

/*******************************************************************************
 * memory order function suffix
 ******************************************************************************/

/* function suffix */
#define V_SUFFIX(mo) V_CONCAT(V_SUFFIX_, mo)
#define V_SUFFIX_mo_seq
#define V_SUFFIX_mo_acq _acq
#define V_SUFFIX_mo_rel _rel
#define V_SUFFIX_mo_rlx _rlx

/* mo remap */
#define V_REMAP(mo) V_CONCAT(V_REMAP_, mo)
#if defined(VATOMIC_ENABLE_ATOMIC_SC)
	#define V_REMAP_mo_seq mo_seq
	#define V_REMAP_mo_acq mo_seq
	#define V_REMAP_mo_rel mo_seq
	#define V_REMAP_mo_rlx mo_seq
#elif defined(VATOMIC_ENABLE_ATOMIC_RLX)
	#define V_REMAP_mo_seq mo_rlx
	#define V_REMAP_mo_acq mo_rlx
	#define V_REMAP_mo_rel mo_rlx
	#define V_REMAP_mo_rlx mo_rlx
#else
	#define V_REMAP_mo_seq mo_seq
	#define V_REMAP_mo_acq mo_acq
	#define V_REMAP_mo_rel mo_rel
	#define V_REMAP_mo_rlx mo_rlx
#endif

/* V_MAP_A(pfx): maps a prefix P to atomic type A, eg, vatomic32 to vatomic32_t
 */
#define V_MAP_A(P)		   V_CONCAT(V_MAP_A_, P)
#define V_MAP_A_vatomic8   vatomic8_t
#define V_MAP_A_vatomic16  vatomic16_t
#define V_MAP_A_vatomic32  vatomic32_t
#define V_MAP_A_vatomic64  vatomic64_t
#define V_MAP_A_vatomicptr vatomicptr_t
#define V_MAP_A_vatomicsz  vatomicsz_t

/* V_MAP_T(pfx): maps a prefix P to variable type T, eg, vatomic32 to vuint32_t
 */
#define V_MAP_T(P)		   V_CONCAT(V_MAP_T_, P)
#define V_MAP_T_vatomic8   vuint8_t
#define V_MAP_T_vatomic16  vuint16_t
#define V_MAP_T_vatomic32  vuint32_t
#define V_MAP_T_vatomic64  vuint64_t
#define V_MAP_T_vatomicptr void *
#define V_MAP_T_vatomicsz  vsize_t

#define V_MAP_await(mo)	   V_CONCAT(V_MAP_await_, mo)
#define V_MAP_await_mo_seq mo_seq
#define V_MAP_await_mo_acq mo_rlx
#define V_MAP_await_mo_rel mo_rlx
#define V_MAP_await_mo_rlx mo_rlx

/*******************************************************************************
 * function name generators
 ******************************************************************************/

#define V_FUNC(...)				V_NR_VARS(_vatomic_func_, __VA_ARGS__)(__VA_ARGS__)
#define _vatomic_func_5(x, ...) V_JOIN(x, _vatomic_func_4(__VA_ARGS__))
#define _vatomic_func_4(x, ...) V_JOIN(x, _vatomic_func_3(__VA_ARGS__))
#define _vatomic_func_3(x, ...) V_JOIN(x, _vatomic_func_2(__VA_ARGS__))
#define _vatomic_func_2(x, mo)	V_CONCAT(x, V_SUFFIX(mo))

/*******************************************************************************
 * function definition selector
 ******************************************************************************/

#define V_DEFINE_FUNC(P, F, mo, ...)                                           \
	V_NR_VARS(V_DEFINE_FUNC_, __VA_ARGS__)(P, F, mo, ##__VA_ARGS__)
#define V_DEFINE_FUNC_6(P, F, mo, suf, ...)                                    \
	V_JOIN(V_DEFINE, F)(P, mo, suf) V_DEFINE_FUNC_5(P, F, mo, __VA_ARGS__)
#define V_DEFINE_FUNC_5(P, F, mo, suf, ...)                                    \
	V_JOIN(V_DEFINE, F)(P, mo, suf) V_DEFINE_FUNC_4(P, F, mo, __VA_ARGS__)
#define V_DEFINE_FUNC_4(P, F, mo, suf, ...)                                    \
	V_JOIN(V_DEFINE, F)(P, mo, suf) V_DEFINE_FUNC_3(P, F, mo, __VA_ARGS__)
#define V_DEFINE_FUNC_3(P, F, mo, suf, ...)                                    \
	V_JOIN(V_DEFINE, F)(P, mo, suf) V_DEFINE_FUNC_2(P, F, mo, __VA_ARGS__)
#define V_DEFINE_FUNC_2(P, F, mo, suf, ...)                                    \
	V_JOIN(V_DEFINE, F)(P, mo, suf) V_DEFINE_FUNC_1(P, F, mo, __VA_ARGS__)
#define V_DEFINE_FUNC_1(P, F, mo, ...) V_JOIN(V_DEFINE, F)(P, mo, ##__VA_ARGS__)

/*******************************************************************************
 * @def V_CAST_INT(T, v)
 * @brief a plain cast applicable to integer casting
 *
 * @param T target type vuint32_t or vuint64_t
 * @param v value to cast
 ******************************************************************************/
#define V_CAST_INT(T, v) ((T)(v))

/*******************************************************************************
 * @def V_CAST(T, v)
 * @brief powerful cast applicable to integers and pointers
 *
 * @param T target type vatomic32_t, vuint32_t, vuint64_t, void*, ...
 * @param v value to cast
 ******************************************************************************/
#define V_CAST(T, v) ((V_CAST_UNION(T, __typeof__(v))){.in = v}.out)

/*******************************************************************************
 * @def V_CAST_UNION
 * @brief Creates an union to safely convert an in_type value to an out_type.
 *
 * Usage example:
 *
 * ```c
 * out_type o = _CAST_UNION(out_type, in_type){0, .in = in}.out;
 * ```
 ******************************************************************************/
#define V_CAST_UNION(out_type, in_type)                                        \
	union {                                                                    \
		vuint64_t clear;                                                       \
                                                                               \
		V_ASSERT(sizeof(out_type) <= sizeof(vuint64_t), out_larger_than64);    \
		V_ASSERT(sizeof(in_type) <= sizeof(vuint64_t), in_larger_than64);      \
                                                                               \
		out_type out;                                                          \
		struct {                                                               \
			char pad[V_CAST_PAD_SIZE(out_type, in_type)];                      \
			in_type in;                                                        \
		};                                                                     \
	}

/* size greater or equal */
#define V_CAST_SZGE(out_type, in_type) (sizeof(out_type) >= sizeof(in_type))

/* size difference */
#define V_CAST_SZDIFF(out_type, in_type) (sizeof(out_type) - sizeof(in_type))

/* padding for big endian */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	#define V_CAST_PAD_SIZE(out_type, in_type) 0
#else
	#define V_CAST_PAD_SIZE(out_type, in_type)                                 \
		(V_CAST_SZGE(out_type, in_type) ? V_CAST_SZDIFF(out_type, in_type) : 0)
#endif

#endif /* VATOMIC_INTERNAL_MACROS_H */
