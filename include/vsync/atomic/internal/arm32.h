/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

/* memory order mappings follow the table from this link:
 *   https://www.cl.cam.ac.uk/~pes20/cpp/cpp0xmappings.html
 *
 * Compared against the compiler output of corresponding C11
 * implementations, both v7 and v8.
 */
#ifndef VATOMIC_ARM32_H
#define VATOMIC_ARM32_H

#include "macros.h"
#include "arm_common.h"

/*******************************************************************************
 * options
 ******************************************************************************/

#if defined(__ARM_ARCH) && __ARM_ARCH == 8
	#define VATOMIC_ARMV8
#endif

/*******************************************************************************
 * mappings
 ******************************************************************************/

/* V_MAP_R: maps a prefix P to a register type R, eg, vatomic32 to 32 */
#define V_MAP_R(P)		   V_CONCAT(V_MAP_R_, P)
#define V_MAP_R_vatomic32  32
#define V_MAP_R_vatomic64  64
#define V_MAP_R_vatomicptr 32

#define V_MAP_LD(mo) V_CONCAT(V_MAP_LD_, V_REMAP(mo))
#if defined(VATOMIC_ARMV8)
	#define V_MAP_LD_mo_seq "a"
	#define V_MAP_LD_mo_acq "a"
	#define V_MAP_LD_mo_rel "r"
	#define V_MAP_LD_mo_rlx "r"
#else
	#define V_MAP_LD_mo_seq "r"
	#define V_MAP_LD_mo_acq "r"
	#define V_MAP_LD_mo_rel "r"
	#define V_MAP_LD_mo_rlx "r"
#endif

#define V_MAP_ST(mo) V_CONCAT(V_MAP_ST_, V_REMAP(mo))
#if defined(VATOMIC_ARMV8)
	#define V_MAP_ST_mo_seq "l"
	#define V_MAP_ST_mo_acq "r"
	#define V_MAP_ST_mo_rel "l"
	#define V_MAP_ST_mo_rlx "r"
#else
	#define V_MAP_ST_mo_seq "r"
	#define V_MAP_ST_mo_acq "r"
	#define V_MAP_ST_mo_rel "r"
	#define V_MAP_ST_mo_rlx "r"
#endif

/*******************************************************************************
 * vatomic_fence
 ******************************************************************************/
#if defined(VATOMIC_ARMV8)
	#define V_FENCE_mo_seq "dmb ish"
	#define V_FENCE_mo_acq "dmb ishld"
	#define V_FENCE_mo_rel "dmb ish"
	#define V_FENCE_mo_rlx V_FENCE_RLX_INSTRUCTION
#else
	#define V_FENCE_mo_seq "dmb ish"
	#define V_FENCE_mo_acq "dmb ish"
	#define V_FENCE_mo_rel "dmb ish"
	#define V_FENCE_mo_rlx V_FENCE_RLX_INSTRUCTION
#endif

/*******************************************************************************
 * ARM32 specific barriers for lda/stl
 ******************************************************************************/

#if defined(VATOMIC_ARMV8)
	#define V_FENCE_PRE_ST(mo)
	#define V_FENCE_POST_ST(mo)
	#define V_FENCE_POST_LD(mo)
#else
	#define V_FENCE_PRE_ST(mo)	  V_CONCAT(V_FENCE_PRE_ST_, V_REMAP(mo)) "\n"
	#define V_FENCE_PRE_ST_mo_seq "dmb ish"
	#define V_FENCE_PRE_ST_mo_acq
	#define V_FENCE_PRE_ST_mo_rel "dmb ish"
	#define V_FENCE_PRE_ST_mo_rlx

	#define V_FENCE_POST_ST(mo)	   V_CONCAT(V_FENCE_POST_ST_, V_REMAP(mo)) "\n"
	#define V_FENCE_POST_ST_mo_seq "dmb ish"
	#define V_FENCE_POST_ST_mo_acq
	#define V_FENCE_POST_ST_mo_rel
	#define V_FENCE_POST_ST_mo_rlx

	#define V_FENCE_POST_LD(mo)	   V_CONCAT(V_FENCE_POST_LD_, V_REMAP(mo)) "\n"
	#define V_FENCE_POST_LD_mo_seq "dmb ish"
	#define V_FENCE_POST_LD_mo_acq "dmb ish"
	#define V_FENCE_POST_LD_mo_rel
	#define V_FENCE_POST_LD_mo_rlx
#endif

/*******************************************************************************
 * assembly instructions
 ******************************************************************************/

#define V_OP_LD(mo, suf) "ld" V_MAP_LD(mo) suf " "

#define V_LD(P, mo, r, a) V_CONCAT(V_LD_, V_MAP_R(P))(mo, r, a)
#define V_LD_32(mo, r, a) V_OP_LD(mo, ) V_R2(, r, , a) "\n"
#define V_LD_64(mo, r, a) V_OP_LD(mo, "exd") V_R2(, r, , a) "\n"

#define V_LDX(P, mo, r, a) V_CONCAT(V_LDX_, V_MAP_R(P))(mo, r, a)
#define V_LDX_32(mo, r, a) V_OP_LD(mo, "ex") V_R2(, r, , a) "\n"
#define V_LDX_64(mo, r, a) V_OP_LD(mo, "exd") V_R3(, r, "H", r, , a) "\n"

#define V_OP_ST(mo, suf) "st" V_MAP_ST(mo) suf " "

#define V_ST(P, mo, s, r, a) V_CONCAT(V_ST_, V_MAP_R(P))(mo, s, r, a)
#define V_ST_32(mo, r, a)	 V_OP_ST(mo, ) V_R2(, r, , a) "\n"
#define V_ST_64(mo, r, a)	 V_ASSERT(0, "not implemented")

#define V_STX(P, mo, s, r, a) V_CONCAT(V_STX_, V_MAP_R(P))(mo, s, r, a)
#define V_STX_32(mo, s, r, a) V_OP_ST(mo, "ex") V_R3(, s, , r, , a) "\n"
#define V_STX_64(mo, s, r, a)                                                  \
	V_OP_ST(mo, "exd") V_R(, s) "," V_R3(, r, "H", r, , a) "\n"

/* compare with immediate */
#define V_CMPI(r, i) "cmp" V_R(, r) "," i "\n"

#define V_CMPEQ(P, r1, r2) V_CONCAT(V_CMPEQ_, V_MAP_R(P))(r1, r2)
#define V_CMPEQ_32(r1, r2) "cmp" V_R2(, r1, , r2) "\n"
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	#define V_CMPEQ_64(r1, r2)                                                 \
		"cmp" V_R2("H", r1, "H", r2) "\ncmpeq" V_R2(, r1, , r2) "\n"
#else
	#define V_CMPEQ_64(r1, r2)                                                 \
		"cmp" V_R2(, r1, , r2) "\ncmpeq" V_R2("H", r1, "H", r2) "\n"
#endif

#define V_OP(P, op1, op2, op3, n, o, v)                                        \
	V_CONCAT(V_OP_, V_MAP_R(P))(op1, op2, op3, n, o, v)
#define V_OP_32(op1, _1, _2, n, o, v) #op1 V_R3(, n, , o, , v) "\n"
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	#define V_OP_64(_, op2, op3, n, o, v)                                      \
		#op2 V_R3(, n, , o, , v) "\n" #op3 V_R3("H", n, "H", o, "H", v) "\n"
#else
	#define V_OP_64(_, op2, op3, n, o, v)                                      \
		#op2 V_R3("H", n, "H", o, "H", v) "\n" #op3 V_R3(, n, , o, , v) "\n"
#endif

/*******************************************************************************
 * vatomic_{read, write}
 ******************************************************************************/

#define V_DEFINE_read(P, mo)                                                   \
	static inline V_MAP_T(P) V_FUNC(P, read, mo)(V_MAP_A(P) *a)                \
	{                                                                          \
		V_MAP_T(P) val;                                                        \
		V_ASM(V_LD(P, mo, v, a) V_FENCE_POST_LD(mo)                            \
			  : [v] "=&r"(val)                                                 \
			  : [a] "Q"(a->_v)                                                 \
			  : "memory");                                                     \
		return val;                                                            \
	}

#define V_DEFINE_write_32(P, mo)                                               \
	static inline void V_FUNC(P, write, mo)(V_MAP_A(P) *a, V_MAP_T(P) v)       \
	{                                                                          \
		V_ASM(V_FENCE_PRE_ST(mo) V_ST_32(mo, v, a) V_FENCE_POST_ST(mo)         \
			  :                                                                \
			  : [v] "r"(v), [a] "Q"(a->_v)                                     \
			  : "memory");                                                     \
	}

/* According to DDI0406C, a ld*exd is required to indicate an active
 * exclusive access in the local monitor, which ensures the following
 * st*exd to be atomic.
 * As a result, atomic write of double word falls back to xchg. */
#define V_DEFINE_write_64(P, mo)                                               \
	static inline void V_FUNC(P, write, mo)(V_MAP_A(P) *a, V_MAP_T(P) v)       \
	{                                                                          \
		V_MAP_T(P) old;                                                        \
		vuint32_t tmp;                                                         \
		V_ASM(V_FENCE_PRE_ST(mo) "1:\n" V_LDX_64(mo, oldv, a)                  \
				  V_STX_64(mo, tmp, newv, a) V_CMPI(tmp, "#0") V_BNE(1b)       \
					  V_FENCE_POST_ST(mo)                                      \
			  : [oldv] "=&r"(old), [tmp] "=&r"(tmp)                            \
			  : [newv] "r"(v), [a] "Q"(a->_v)                                  \
			  : "memory", "cc");                                               \
	}

#define V_DEFINE_write(P, mo) V_CONCAT(V_DEFINE_write_, V_MAP_R(P))(P, mo)

/*******************************************************************************
 * vatomic_xchg
 ******************************************************************************/

#define V_DEFINE_xchg(P, mo)                                                   \
	static inline V_MAP_T(P) V_FUNC(P, xchg, mo)(V_MAP_A(P) *a, V_MAP_T(P) v)  \
	{                                                                          \
		V_MAP_T(P) old;                                                        \
		vuint32_t tmp;                                                         \
		V_ASM(V_FENCE_PRE_ST(mo) "1:\n" V_LDX(P, mo, oldv, a)                  \
				  V_STX(P, mo, tmp, newv, a) V_CMPI(tmp, "#0") V_BNE(1b)       \
					  V_FENCE_POST_LD(mo)                                      \
			  : [oldv] "=&r"(old), [tmp] "=&r"(tmp)                            \
			  : [newv] "r"(v), [a] "Q"(a->_v)                                  \
			  : "memory", "cc");                                               \
		return old;                                                            \
	}

/*******************************************************************************
 * vatomic_cmpxchg
 ******************************************************************************/

#define V_DEFINE_cmpxchg(P, mo)                                                \
	static inline V_MAP_T(P) V_FUNC(P, cmpxchg, mo)(                           \
		V_MAP_A(P) *a, V_MAP_T(P) e, V_MAP_T(P) v)                             \
	{                                                                          \
		V_MAP_T(P) old;                                                        \
		vuint32_t tmp;                                                         \
		V_ASM(V_FENCE_PRE_ST(mo) "1:\n" V_LDX(P, mo, oldv, a)                  \
				  V_CMPEQ(P, oldv, expv) V_BNE(2f) V_STX(P, mo, tmp, newv, a)  \
					  V_CMPI(tmp, "#0") V_BNE(1b) "2:\n" V_FENCE_POST_LD(mo)   \
			  : [oldv] "=&r"(old), [tmp] "=&r"(tmp)                            \
			  : [newv] "r"(v), [expv] "r"(e), [a] "Q"(a->_v)                   \
			  : "memory", "cc");                                               \
		return old;                                                            \
	}

/*******************************************************************************
 * RMW helper
 ******************************************************************************/

#define V_OP_RMW(P, mo, op1, op2, op3)                                         \
	{                                                                          \
		V_MAP_T(P) newv;                                                       \
		vuint32_t tmp;                                                         \
		V_ASM(V_FENCE_PRE_ST(mo) "1:\n" V_LDX(P, mo, oldv, a)                  \
				  V_OP(P, op1, op2, op3, newv, oldv, v)                        \
					  V_STX(P, mo, tmp, newv, a) V_CMPI(tmp, "#0") V_BNE(1b)   \
						  V_FENCE_POST_LD(mo)                                  \
			  : [oldv] "=&r"(oldv), [newv] "=&r"(newv), [tmp] "=&r"(tmp)       \
			  : [v] "r"(v), [a] "Q"(a->_v)                                     \
			  : "memory", "cc");                                               \
	}

/*******************************************************************************
 * vatomic_get_add
 ******************************************************************************/

#define V_DEFINE_get_add(P, mo)                                                \
	static inline V_MAP_T(P) V_FUNC(P, get_add, mo)(V_MAP_A(P) *a,             \
													V_MAP_T(P) v)              \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(P, mo, add, adds, adc);                                       \
		return oldv;                                                           \
	}

/*******************************************************************************
 * vatomic_get_or
 ******************************************************************************/

#define V_DEFINE_get_or(P, mo)                                                 \
	static inline V_MAP_T(P) V_FUNC(P, get_or, mo)(V_MAP_A(P) *a,              \
												   V_MAP_T(P) v)               \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(P, mo, orr, orr, orr);                                        \
		return oldv;                                                           \
	}

/*******************************************************************************
 * vatomic_get_xor
 ******************************************************************************/

#define V_DEFINE_get_xor(P, mo)                                                \
	static inline V_MAP_T(P) V_FUNC(P, get_xor, mo)(V_MAP_A(P) *a,             \
													V_MAP_T(P) v)              \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(P, mo, eor, eor, eor);                                        \
		return oldv;                                                           \
	}

/*******************************************************************************
 * vatomic_get_and
 ******************************************************************************/

#define V_DEFINE_get_and(P, mo)                                                \
	static inline V_MAP_T(P) V_FUNC(P, get_and, mo)(V_MAP_A(P) *a,             \
													V_MAP_T(P) v)              \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(P, mo, and, and, and);                                        \
		return oldv;                                                           \
	}

/*******************************************************************************
 * vatomic_get_sub
 ******************************************************************************/

#define V_DEFINE_get_sub(P, mo)                                                \
	static inline V_MAP_T(P) V_FUNC(P, get_sub, mo)(V_MAP_A(P) *a,             \
													V_MAP_T(P) v)              \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(P, mo, sub, subs, sbc);                                       \
		return oldv;                                                           \
	}

/*******************************************************************************
 * Generate definitions
 ******************************************************************************/

V_DEFINE_FUNC(vatomic, fence, mo_seq)
#define VATOMIC_FENCE
V_DEFINE_FUNC(vatomic, fence, mo_rel)
#define VATOMIC_FENCE_REL
V_DEFINE_FUNC(vatomic, fence, mo_acq)
#define VATOMIC_FENCE_ACQ
V_DEFINE_FUNC(vatomic, fence, mo_rlx)
#define VATOMIC_FENCE_RLX

V_DEFINE_FUNC(vatomic32, read, mo_seq)
#define VATOMIC32_READ
V_DEFINE_FUNC(vatomic32, read, mo_acq)
#define VATOMIC32_READ_ACQ
V_DEFINE_FUNC(vatomic32, read, mo_rlx)
#define VATOMIC32_READ_RLX
V_DEFINE_FUNC(vatomic64, read, mo_seq)
#define VATOMIC64_READ
V_DEFINE_FUNC(vatomic64, read, mo_acq)
#define VATOMIC64_READ_ACQ
V_DEFINE_FUNC(vatomic64, read, mo_rlx)
#define VATOMIC64_READ_RLX
V_DEFINE_FUNC(vatomicptr, read, mo_seq)
#define VATOMICPTR_READ
V_DEFINE_FUNC(vatomicptr, read, mo_acq)
#define VATOMICPTR_READ_ACQ
V_DEFINE_FUNC(vatomicptr, read, mo_rlx)
#define VATOMICPTR_READ_RLX

V_DEFINE_FUNC(vatomic32, write, mo_seq)
#define VATOMIC32_WRITE
V_DEFINE_FUNC(vatomic32, write, mo_rel)
#define VATOMIC32_WRITE_REL
V_DEFINE_FUNC(vatomic32, write, mo_rlx)
#define VATOMIC32_WRITE_RLX
V_DEFINE_FUNC(vatomic64, write, mo_seq)
#define VATOMIC64_WRITE
V_DEFINE_FUNC(vatomic64, write, mo_rel)
#define VATOMIC64_WRITE_REL
V_DEFINE_FUNC(vatomic64, write, mo_rlx)
#define VATOMIC64_WRITE_RLX
V_DEFINE_FUNC(vatomicptr, write, mo_seq)
#define VATOMICPTR_WRITE
V_DEFINE_FUNC(vatomicptr, write, mo_rel)
#define VATOMICPTR_WRITE_REL
V_DEFINE_FUNC(vatomicptr, write, mo_rlx)
#define VATOMICPTR_WRITE_RLX

V_DEFINE_FUNC(vatomic32, xchg, mo_seq)
#define VATOMIC32_XCHG
V_DEFINE_FUNC(vatomic32, xchg, mo_acq)
#define VATOMIC32_XCHG_ACQ
V_DEFINE_FUNC(vatomic32, xchg, mo_rel)
#define VATOMIC32_XCHG_REL
V_DEFINE_FUNC(vatomic32, xchg, mo_rlx)
#define VATOMIC32_XCHG_RLX
V_DEFINE_FUNC(vatomic64, xchg, mo_seq)
#define VATOMIC64_XCHG
V_DEFINE_FUNC(vatomic64, xchg, mo_acq)
#define VATOMIC64_XCHG_ACQ
V_DEFINE_FUNC(vatomic64, xchg, mo_rel)
#define VATOMIC64_XCHG_REL
V_DEFINE_FUNC(vatomic64, xchg, mo_rlx)
#define VATOMIC64_XCHG_RLX
V_DEFINE_FUNC(vatomicptr, xchg, mo_seq)
#define VATOMICPTR_XCHG
V_DEFINE_FUNC(vatomicptr, xchg, mo_acq)
#define VATOMICPTR_XCHG_ACQ
V_DEFINE_FUNC(vatomicptr, xchg, mo_rel)
#define VATOMICPTR_XCHG_REL
V_DEFINE_FUNC(vatomicptr, xchg, mo_rlx)
#define VATOMICPTR_XCHG_RLX

V_DEFINE_FUNC(vatomic32, cmpxchg, mo_seq)
#define VATOMIC32_CMPXCHG
V_DEFINE_FUNC(vatomic32, cmpxchg, mo_acq)
#define VATOMIC32_CMPXCHG_ACQ
V_DEFINE_FUNC(vatomic32, cmpxchg, mo_rel)
#define VATOMIC32_CMPXCHG_REL
V_DEFINE_FUNC(vatomic32, cmpxchg, mo_rlx)
#define VATOMIC32_CMPXCHG_RLX
V_DEFINE_FUNC(vatomic64, cmpxchg, mo_seq)
#define VATOMIC64_CMPXCHG
V_DEFINE_FUNC(vatomic64, cmpxchg, mo_acq)
#define VATOMIC64_CMPXCHG_ACQ
V_DEFINE_FUNC(vatomic64, cmpxchg, mo_rel)
#define VATOMIC64_CMPXCHG_REL
V_DEFINE_FUNC(vatomic64, cmpxchg, mo_rlx)
#define VATOMIC64_CMPXCHG_RLX
V_DEFINE_FUNC(vatomicptr, cmpxchg, mo_seq)
#define VATOMICPTR_CMPXCHG
V_DEFINE_FUNC(vatomicptr, cmpxchg, mo_acq)
#define VATOMICPTR_CMPXCHG_ACQ
V_DEFINE_FUNC(vatomicptr, cmpxchg, mo_rel)
#define VATOMICPTR_CMPXCHG_REL
V_DEFINE_FUNC(vatomicptr, cmpxchg, mo_rlx)
#define VATOMICPTR_CMPXCHG_RLX

V_DEFINE_FUNC(vatomic32, get_and, mo_seq)
#define VATOMIC32_GET_AND
V_DEFINE_FUNC(vatomic32, get_and, mo_acq)
#define VATOMIC32_GET_AND_ACQ
V_DEFINE_FUNC(vatomic32, get_and, mo_rel)
#define VATOMIC32_GET_AND_REL
V_DEFINE_FUNC(vatomic32, get_and, mo_rlx)
#define VATOMIC32_GET_AND_RLX
V_DEFINE_FUNC(vatomic64, get_and, mo_seq)
#define VATOMIC64_GET_AND
V_DEFINE_FUNC(vatomic64, get_and, mo_acq)
#define VATOMIC64_GET_AND_ACQ
V_DEFINE_FUNC(vatomic64, get_and, mo_rel)
#define VATOMIC64_GET_AND_REL
V_DEFINE_FUNC(vatomic64, get_and, mo_rlx)
#define VATOMIC64_GET_AND_RLX

V_DEFINE_FUNC(vatomic32, get_or, mo_seq)
#define VATOMIC32_GET_OR
V_DEFINE_FUNC(vatomic32, get_or, mo_acq)
#define VATOMIC32_GET_OR_ACQ
V_DEFINE_FUNC(vatomic32, get_or, mo_rel)
#define VATOMIC32_GET_OR_REL
V_DEFINE_FUNC(vatomic32, get_or, mo_rlx)
#define VATOMIC32_GET_OR_RLX
V_DEFINE_FUNC(vatomic64, get_or, mo_seq)
#define VATOMIC64_GET_OR
V_DEFINE_FUNC(vatomic64, get_or, mo_acq)
#define VATOMIC64_GET_OR_ACQ
V_DEFINE_FUNC(vatomic64, get_or, mo_rel)
#define VATOMIC64_GET_OR_REL
V_DEFINE_FUNC(vatomic64, get_or, mo_rlx)
#define VATOMIC64_GET_OR_RLX

V_DEFINE_FUNC(vatomic32, get_xor, mo_seq)
#define VATOMIC32_GET_XOR
V_DEFINE_FUNC(vatomic32, get_xor, mo_acq)
#define VATOMIC32_GET_XOR_ACQ
V_DEFINE_FUNC(vatomic32, get_xor, mo_rel)
#define VATOMIC32_GET_XOR_REL
V_DEFINE_FUNC(vatomic32, get_xor, mo_rlx)
#define VATOMIC32_GET_XOR_RLX
V_DEFINE_FUNC(vatomic64, get_xor, mo_seq)
#define VATOMIC64_GET_XOR
V_DEFINE_FUNC(vatomic64, get_xor, mo_acq)
#define VATOMIC64_GET_XOR_ACQ
V_DEFINE_FUNC(vatomic64, get_xor, mo_rel)
#define VATOMIC64_GET_XOR_REL
V_DEFINE_FUNC(vatomic64, get_xor, mo_rlx)
#define VATOMIC64_GET_XOR_RLX

V_DEFINE_FUNC(vatomic32, get_add, mo_seq)
#define VATOMIC32_GET_ADD
V_DEFINE_FUNC(vatomic32, get_add, mo_acq)
#define VATOMIC32_GET_ADD_ACQ
V_DEFINE_FUNC(vatomic32, get_add, mo_rel)
#define VATOMIC32_GET_ADD_REL
V_DEFINE_FUNC(vatomic32, get_add, mo_rlx)
#define VATOMIC32_GET_ADD_RLX
V_DEFINE_FUNC(vatomic64, get_add, mo_seq)
#define VATOMIC64_GET_ADD
V_DEFINE_FUNC(vatomic64, get_add, mo_acq)
#define VATOMIC64_GET_ADD_ACQ
V_DEFINE_FUNC(vatomic64, get_add, mo_rel)
#define VATOMIC64_GET_ADD_REL
V_DEFINE_FUNC(vatomic64, get_add, mo_rlx)
#define VATOMIC64_GET_ADD_RLX

V_DEFINE_FUNC(vatomic32, get_sub, mo_seq)
#define VATOMIC32_GET_SUB
V_DEFINE_FUNC(vatomic32, get_sub, mo_acq)
#define VATOMIC32_GET_SUB_ACQ
V_DEFINE_FUNC(vatomic32, get_sub, mo_rel)
#define VATOMIC32_GET_SUB_REL
V_DEFINE_FUNC(vatomic32, get_sub, mo_rlx)
#define VATOMIC32_GET_SUB_RLX
V_DEFINE_FUNC(vatomic64, get_sub, mo_seq)
#define VATOMIC64_GET_SUB
V_DEFINE_FUNC(vatomic64, get_sub, mo_acq)
#define VATOMIC64_GET_SUB_ACQ
V_DEFINE_FUNC(vatomic64, get_sub, mo_rel)
#define VATOMIC64_GET_SUB_REL
V_DEFINE_FUNC(vatomic64, get_sub, mo_rlx)
#define VATOMIC64_GET_SUB_RLX

#endif /* VATOMIC_ARM32_H */
