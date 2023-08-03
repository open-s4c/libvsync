/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

/* memory order mappings follow the table from this link:
 *   https://www.cl.cam.ac.uk/~pes20/cpp/cpp0xmappings.html
 *
 * Define VSYNC_ENABLE_ARM64_LSE to enable ARM64v8.1 LSE support.
 *
 * We currently support the following instructions from the LSE:
 *    cas(a/al/l),
 *    swp(a/al/l),
 *    ldadd(a/al/l),
 *    ldclr(a/al/l),
 *    ldset(a/al/l),
 */
#ifndef VATOMIC_ARM64_H
#define VATOMIC_ARM64_H

#include "macros.h"
#include "arm_common.h"

/*******************************************************************************
 * options
 ******************************************************************************/

#if defined(__ARM_FEATURE_ATOMICS) && !defined(VATOMIC_DISABLE_ARM64_LSE)
	#define VATOMIC_ARM64_LSE
#endif

#if defined(VATOMIC_ENABLE_ARM64_LXE) && defined(VATOMIC_ARM64_LSE)
	#define VATOMIC_ARM64_LXE
#elif defined(VATOMIC_ENABLE_ARM64_LXE)
	#error "Platform does not support LSE instructions"
#endif

#if defined(VATOMIC_DISABLE_POLITE_AWAIT)
	#define V_SEVL
	#define V_WFE
	#define V_LD_WFE V_LD
#else
	#define V_SEVL	 "sevl\n"
	#define V_WFE	 "wfe\n"
	#define V_LD_WFE V_LDX
#endif

/*******************************************************************************
 * mappings
 ******************************************************************************/

/* V_MAP_R: maps a prefix P to a register type R, eg, vatomic32 to "w" */
#define V_MAP_R(P)		   V_CONCAT(V_MAP_R_, P)
#define V_MAP_R_vatomic32  "w"
#define V_MAP_R_vatomic64  "x"
#define V_MAP_R_vatomicptr "x"

/* fence mappings */
#define V_FENCE_mo_seq "dmb ish"
#define V_FENCE_mo_acq "dmb ishld"
#define V_FENCE_mo_rel "dmb ish"
#define V_FENCE_mo_rlx V_FENCE_RLX_INSTRUCTION

#define V_OP_LD(x, mo) "ld" V_CONCAT(V_OP_LD_, V_REMAP(mo)) #x "r"
#define V_OP_LD_mo_seq "a"
#define V_OP_LD_mo_acq "a"
#define V_OP_LD_mo_rel
#define V_OP_LD_mo_rlx

#define V_OP_ST(x, mo) "st" V_CONCAT(V_OP_ST_, V_REMAP(mo)) #x "r"
#define V_OP_ST_mo_seq "l"
#define V_OP_ST_mo_acq
#define V_OP_ST_mo_rel "l"
#define V_OP_ST_mo_rlx

/* valid for cas, swp */
#define V_OP_LSE(op, mo) op V_CONCAT(V_OP_LSE_, V_REMAP(mo))
#define V_OP_LSE_mo_seq	 "al"
#define V_OP_LSE_mo_acq	 "a"
#define V_OP_LSE_mo_rel	 "l"
#define V_OP_LSE_mo_rlx

/* ldadd, ldclr, ldset */
#define V_OP_LDLSE(op, mo) "ld" op V_CONCAT(V_OP_LSE_, V_REMAP(mo))
#define V_OP_LDLSE_mo_seq  "al"
#define V_OP_LDLSE_mo_acq  "a"
#define V_OP_LDLSE_mo_rel  "l"
#define V_OP_LDLSE_mo_rlx

/* valid for stadd, stclr, stset */
#define V_OP_STLSE(op, mo) "st" op V_CONCAT(V_OP_STLSE_, V_REMAP(mo))
#define V_OP_STLSE_mo_seq  "l"
#define V_OP_STLSE_mo_acq
#define V_OP_STLSE_mo_rel "l"
#define V_OP_STLSE_mo_rlx

/*******************************************************************************
 * assembly instructions
 ******************************************************************************/

/* Prefetch Memory (prfm) for store (pst) into L1; data is used only once
 * (strm). We're using it to prefetch for store before ldx so that stx doesn't
 * suffer the cost of changing a cacheline from shared to exclusive state. See
 * https://lore.kernel.org/linux-arm-kernel/1436779519-2232-16/
 */
#define V_PRFM(a)		   "prfm pstl1strm," V_R(, a) "\n"
#define V_MOV(T, dst, src) "mov" V_R2(T, dst, T, src) "\n"
#define V_DEC(T, n, o)	   "sub" V_R2(T, n, T, o) "#1\n"
#define V_CMP(T, v, e)	   "cmp" V_R2(T, v, T, e) "\n"
#define V_CBZ(T, r, l)	   "cbz" V_R(T, r) "," #l "\n"
#define V_CBNZ(T, r, l)	   "cbnz" V_R(T, r) "," #l "\n"

#define V_LD(mo, T, v, a)	  V_OP_LD(, mo) V_R2(T, v, , a) "\n"
#define V_ST(mo, T, v, a)	  V_OP_ST(, mo) V_R2(T, v, , a) "\n"
#define V_LDX(mo, T, r, a)	  V_OP_LD(x, mo) V_R2(T, r, , a) "\n"
#define V_STX(mo, T, t, n, a) V_OP_ST(x, mo) V_R3("w", t, T, n, , a) "\n"
#define V_CAS(mo, T, o, n, a) V_OP_LSE("cas", mo) V_R3(T, o, T, n, , a) "\n"
#define V_SWP(mo, T, n, o, a) V_OP_LSE("swp", mo) V_R3(T, n, T, o, , a) "\n"
#define V_OP3(op, T, n, o, v) op V_R3(T, n, T, o, T, v) "\n"

#define V_LDLSE(op, mo, T, n, o, a)                                            \
	V_OP_LDLSE(op, mo) V_R3(T, n, T, o, , a) "\n"
#define V_STLSE(op, mo, T, n, o, a) V_OP_STLSE(op, mo) V_R2(T, n, , a) "\n"

#define V_MVN(T, v)						"mvn" V_R2(T, v, T, v) "\n"
#define V_MVN_LDSE(arg, mo, T, v, o, a) V_MVN(T, v) V_LDLSE(arg, mo, T, v, o, a)
#define V_MVN_STLSE(arg, mo, T, v, o, a)                                       \
	V_MVN(T, v) V_STLSE(arg, mo, T, v, o, a)

#define V_NEG(T, v) "neg" V_R2(T, v, T, v) "\n"
#define V_NEG_LDLSE(arg, mo, T, v, o, a)                                       \
	V_NEG(T, v) V_LDLSE(arg, mo, T, v, o, a)
#define V_NEG_STLSE(arg, mo, T, v, o, a)                                       \
	V_NEG(T, v) V_STLSE(arg, mo, T, v, o, a)

/*******************************************************************************
 * assembly region selector
 ******************************************************************************/

#if defined(VATOMIC_ARM64_LSE)
	#define V_WITH_LSE(X) X
	#define V_WITH_LLSC_LXE(X, COND_LLSC, COND_LXE)
#elif defined(VATOMIC_ARM64_LXE)
	#define V_WITH_LSE(X)							X
	#define V_WITH_LLSC_LXE(X, COND_LLSC, COND_LXE) X COND_LXE
#else
	#define V_WITH_LSE(X)
	#define V_WITH_LLSC_LXE(X, COND_LLSC, COND_LXE) X COND_LLSC
#endif

/*******************************************************************************
 * vatomic_{read, write}
 ******************************************************************************/

#define V_DEFINE_read(P, mo)                                                   \
	static inline V_MAP_T(P) V_FUNC(P, read, mo)(V_MAP_A(P) *a)                \
	{                                                                          \
		V_MAP_T(P) val;                                                        \
		V_ASM(V_LD(mo, V_MAP_R(P), v, a)                                       \
			  : [v] "=&r"(val)                                                 \
			  : [a] "Q"(a->_v)                                                 \
			  : "memory");                                                     \
		return val;                                                            \
	}

#define V_DEFINE_write(P, mo)                                                  \
	static inline void V_FUNC(P, write, mo)(V_MAP_A(P) *a, V_MAP_T(P) v)       \
	{                                                                          \
		V_ASM(V_ST(mo, V_MAP_R(P), v, a)                                       \
			  :                                                                \
			  : [v] "r"(v), [a] "Q"(a->_v)                                     \
			  : "memory");                                                     \
	}

/*******************************************************************************
 * vatomic_xchg
 *
 * LSE supports: SWP (word/double word), SWPB (byte), SWPH (halfword)
 * 32-bit: swp <Ws>, <Wt>, [<Xn|SP>]
 * 64-bit: swp <Xs>, <Xt>, [<Xn|SP>]
 *    <Ws> | <Xs> register to be stored (Rs)
 *    <Wt> | <Xt> register to be loaded (Rt)
 *    <Xn|SP> base register or stack pointer (Rn)
 *
 ******************************************************************************/

#define V_DEFINE_xchg(P, mo)                                                   \
	static inline V_MAP_T(P) V_FUNC(P, xchg, mo)(V_MAP_A(P) *a, V_MAP_T(P) v)  \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		vuint32_t tmp;                                                         \
		V_ASM(V_WITH_LLSC_LXE(V_PRFM(a) "1:\n" V_LDX(mo, V_MAP_R(P), oldv, a)  \
								  V_STX(mo, V_MAP_R(P), tmp, newv, a),         \
							  V_CBNZ("w", tmp, 1b), V_CBZ("w", tmp, 2f))       \
				  V_WITH_LSE(V_SWP(mo, V_MAP_R(P), newv, oldv, a) "2:\n")      \
			  : [oldv] "=&r"(oldv), [tmp] "=&r"(tmp)                           \
			  : [newv] "r"(v), [a] "Q"(a->_v)                                  \
			  : "memory");                                                     \
		return oldv;                                                           \
	}

/*******************************************************************************
 * vatomic_cmpxchg
 *
 * LSE supports: CAS (word/double word), CASB (byte), CASH
 * (halfword), CASP (Pair of words/double words ) 32-bit: cas
 * <Ws>, <Wt>, [<Xn|SP>{,#0}] 64-bit: cas <Xs>, <Xt>,
 * [<Xn|SP>{,#0}] <Ws> | <Xs> register to be compared and
 * loaded (Rs) <Wt> | <Xt> register to be conditionally
 * stored (Rt) <Xn|SP> base register or stack pointer (Rn)
 *
 ******************************************************************************/

#define V_DEFINE_cmpxchg(P, mo)                                                \
	static inline V_MAP_T(P) V_FUNC(P, cmpxchg, mo)(                           \
		V_MAP_A(P) *a, V_MAP_T(P) e, V_MAP_T(P) v)                             \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		vuint32_t tmp;                                                         \
		V_ASM(V_WITH_LLSC_LXE(V_PRFM(a) "1:\n" V_LDX(mo, V_MAP_R(P), oldv, a)  \
								  V_CMP(V_MAP_R(P), oldv, exp) V_BNE(2f)       \
									  V_STX(mo, V_MAP_R(P), tmp, newv, a),     \
							  V_CBNZ("w", tmp, 1b), V_CBZ("w", tmp, 2f))       \
				  V_WITH_LSE(V_MOV(V_MAP_R(P), oldv, exp)                      \
								 V_CAS(mo, V_MAP_R(P), oldv, newv, a)) "2:\n"  \
			  : [oldv] "=&r"(oldv), [tmp] "=&r"(tmp)                           \
			  : [newv] "r"(v), [exp] "r"(e), [a] "Q"(a->_v)                    \
			  : "memory", "cc");                                               \
		return oldv;                                                           \
	}

/*******************************************************************************
 * assembly helpers
 ******************************************************************************/

#define V_OP_RMW(_OP1, arg1, _OP2, arg2, mo, T, vtyp)                          \
	({                                                                         \
		vuint32_t tmp;                                                         \
		vtyp newv;                                                             \
		V_ASM(V_WITH_LLSC_LXE(V_PRFM(a) "1:\n" V_LDX(mo, T, oldv, a)           \
								  _OP1(arg1, T, newv, oldv, v)                 \
									  V_STX(mo, T, tmp, newv, a),              \
							  V_CBNZ("w", tmp, 1b), V_CBZ("w", tmp, 2f))       \
				  V_WITH_LSE(_OP2(arg2, mo, T, v, oldv, a)) "2:\n"             \
			  : [oldv] "=&r"(oldv), [newv] "=&r"(newv), [tmp] "=&r"(tmp),      \
				[v] "+&r"(v)                                                   \
			  : [a] "Q"(a->_v)                                                 \
			  : "memory", "cc");                                               \
	})

/*******************************************************************************
 * vatomic_{get_max, max, max_get}
 ******************************************************************************/

#define V_MAX_CMP(target, T, newv, oldv, v)                                    \
	V_CMP(T, oldv, v) V_BHS(target) V_MOV(T, newv, v)

#define V_DEFINE_get_max(P, mo)                                                \
	static inline V_MAP_T(P) V_FUNC(P, get_max, mo)(V_MAP_A(P) *a,             \
													V_MAP_T(P) v)              \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(V_MAX_CMP, 2f, V_LDLSE, "umax", mo, V_MAP_R(P), V_MAP_T(P));  \
		return oldv;                                                           \
	}

#define V_DEFINE_max(P, mo)                                                    \
	static inline void V_FUNC(P, max, mo)(V_MAP_A(P) *a, V_MAP_T(P) v)         \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(V_MAX_CMP, 2f, V_STLSE, "umax", mo, V_MAP_R(P), V_MAP_T(P));  \
	}

/*******************************************************************************
 * vatomic_{get_add, add, add_get}
 ******************************************************************************/

#define V_DEFINE_get_add(P, mo)                                                \
	static inline V_MAP_T(P) V_FUNC(P, get_add, mo)(V_MAP_A(P) *a,             \
													V_MAP_T(P) v)              \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(V_OP3, "add", V_LDLSE, "add", mo, V_MAP_R(P), V_MAP_T(P));    \
		return oldv;                                                           \
	}

#define V_DEFINE_add(P, mo)                                                    \
	static inline void V_FUNC(P, add, mo)(V_MAP_A(P) *a, V_MAP_T(P) v)         \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(V_OP3, "add", V_STLSE, "add", mo, V_MAP_R(P), V_MAP_T(P));    \
	}

/*******************************************************************************
 * vatomic_{get_or, or, or_get}
 *
 * LSE supports: LDSET (word/double word), LDSETB (byte),
 * LDSETH (halfword) 32-bit: ldset <Ws>, <Wt>, [<Xn|SP>]
 * 64-bit: ldset <Xs>, <Xt>, [<Xn|SP>]
 *    <Ws> | <Xs> register holding the data value to be
 * operated on with the memory contents of the memory
 * location (Rs) <Wt> | <Xt> register to be loaded (Rt)
 *    <Xn|SP> base register or stack pointer (Rn)
 *    oldv = *a
 *    *a = *a | v
 *
 ******************************************************************************/

#define V_DEFINE_get_or(P, mo)                                                 \
	static inline V_MAP_T(P) V_FUNC(P, get_or, mo)(V_MAP_A(P) *a,              \
												   V_MAP_T(P) v)               \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(V_OP3, "orr", V_LDLSE, "set", mo, V_MAP_R(P), V_MAP_T(P));    \
		return oldv;                                                           \
	}

#define V_DEFINE_or(P, mo)                                                     \
	static inline void V_FUNC(P, or, mo)(V_MAP_A(P) *a, V_MAP_T(P) v)          \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(V_OP3, "orr", V_STLSE, "set", mo, V_MAP_R(P), V_MAP_T(P));    \
	}

/*******************************************************************************
 * vatomic_{get_xor, xor, xor_get}
 *
 * LSE supports: LDEOR (word/double word), LDSETB (byte),
 * LDSETH (halfword) 32-bit: ldeor <Ws>, <Wt>, [<Xn|SP>]
 * 64-bit: ldeor <Xs>, <Xt>, [<Xn|SP>]
 *    <Ws> | <Xs> register holding the data value to be
 * operated on with the memory contents of the memory
 * location (Rs) <Wt> | <Xt> register to be loaded (Rt)
 *    <Xn|SP> base register or stack pointer (Rn)
 *    oldv = *a
 *    *a = *a ^ v
 *
 ******************************************************************************/

#define V_DEFINE_get_xor(P, mo)                                                \
	static inline V_MAP_T(P) V_FUNC(P, get_xor, mo)(V_MAP_A(P) *a,             \
													V_MAP_T(P) v)              \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(V_OP3, "eor", V_LDLSE, "eor", mo, V_MAP_R(P), V_MAP_T(P));    \
		return oldv;                                                           \
	}

#define V_DEFINE_xor(P, mo)                                                    \
	static inline void V_FUNC(P, xor, mo)(V_MAP_A(P) *a, V_MAP_T(P) v)         \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(V_OP3, "eor", V_STLSE, "eor", mo, V_MAP_R(P), V_MAP_T(P));    \
	}

/*******************************************************************************
 * vatomic_{get_and, and}
 *
 * LSE supports: LDCLR (word/double word), LDCLRB (byte),
 * LDCLRH (halfword) 32-bit: ldset <Ws>, <Wt>, [<Xn|SP>]
 * 64-bit: ldset <Xs>, <Xt>, [<Xn|SP>]
 *    <Ws> | <Xs> register holding the data value to be
 * operated on with the memory contents of the memory
 * location (Rs) <Wt> | <Xt> register to be loaded (Rt)
 *    <Xn|SP> base register or stack pointer (Rn)
 *    oldv = *a
 *    *a = *a & ~v
 *
 ******************************************************************************/

#define V_DEFINE_get_and(P, mo)                                                \
	static inline V_MAP_T(P) V_FUNC(P, get_and, mo)(V_MAP_A(P) *a,             \
													V_MAP_T(P) v)              \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(V_OP3, "and", V_MVN_LDSE, "clr", mo, V_MAP_R(P), V_MAP_T(P)); \
		return oldv;                                                           \
	}

#define V_DEFINE_and(P, mo)                                                    \
	static inline void V_FUNC(P, and, mo)(V_MAP_A(P) *a, V_MAP_T(P) v)         \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(V_OP3, "and", V_MVN_STLSE, "clr", mo, V_MAP_R(P),             \
				 V_MAP_T(P));                                                  \
	}

/*******************************************************************************
 * vatomic_{get_sub, sub, sub_get}
 *
 * TODO: for sub we need to send -v not v and use add, use the sub instruction!
 *
 * Since LSE does not have an LDSUB instruction we are going
 * to use LDADD instead! But first we need to make v to -v,
 * for that purpose we first do tmp = 0 - v using the sub
 * instruction, we do that on asm-level and not C-level, to
 * avoid an overflow possiblity with an undefined behaviour
 *
 ******************************************************************************/

#define V_DEFINE_get_sub(P, mo)                                                \
	static inline V_MAP_T(P) V_FUNC(P, get_sub, mo)(V_MAP_A(P) *a,             \
													V_MAP_T(P) v)              \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(V_OP3, "sub", V_NEG_LDLSE, "add", mo, V_MAP_R(P),             \
				 V_MAP_T(P));                                                  \
		return oldv;                                                           \
	}

#define V_DEFINE_sub(P, mo)                                                    \
	static inline void V_FUNC(P, sub, mo)(V_MAP_A(P) *a, V_MAP_T(P) v)         \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_OP_RMW(V_OP3, "sub", V_NEG_STLSE, "add", mo, V_MAP_R(P),             \
				 V_MAP_T(P));                                                  \
	}

/*******************************************************************************
 * vatomic_await functions (binary variants)
 ******************************************************************************/

/* according experiments with Kunpeng 920, aligning the
 * branch targets at double quadwords can greatly improve
 * performance
 */
#define V_CODE_ALIGN ".align 5\n"

#define V_MAP_await_b_lt  "b.lo"
#define V_MAP_await_b_le  "b.ls"
#define V_MAP_await_b_gt  "b.hi"
#define V_MAP_await_b_ge  "b.hs"
#define V_MAP_await_b_eq  "b.eq"
#define V_MAP_await_b_neq "b.ne"

#define V_AWAIT_B(cond, l) V_CONCAT(V_MAP_await_b_, cond) " " #l "\n"

#define V_MAP_await_b_not_lt  "b.hs"
#define V_MAP_await_b_not_le  "b.hi"
#define V_MAP_await_b_not_gt  "b.ls"
#define V_MAP_await_b_not_ge  "b.lo"
#define V_MAP_await_b_not_eq  "b.ne"
#define V_MAP_await_b_not_neq "b.eq"

#define V_AWAIT_BNOT(cond, l) V_CONCAT(V_MAP_await_b_not_, cond) " " #l "\n"

#define V_DEFINE_await(P, mo, cond)                                            \
	static inline V_MAP_T(P) V_FUNC(P, await, cond, mo)(V_MAP_A(P) *a,         \
														V_MAP_T(P) v)          \
	{                                                                          \
		V_MAP_T(P) val;                                                        \
		V_ASM(V_LD(mo, V_MAP_R(P), val, a) V_CMP(V_MAP_R(P), val, exp)         \
				  V_AWAIT_B(cond, 2f) V_SEVL V_CODE_ALIGN                      \
			  "1:\n" V_WFE V_LD_WFE(mo, V_MAP_R(P), val, a)                    \
				  V_CMP(V_MAP_R(P), val, exp) V_AWAIT_BNOT(cond, 1b) "2:\n"    \
			  : [val] "=&r"(val)                                               \
			  : [exp] "r"(v), [a] "Q"(a->_v)                                   \
			  : "memory", "cc");                                               \
		return val;                                                            \
	}

#define V_DEFINE_await_eq(P, mo)                                               \
	static inline V_MAP_T(P) V_FUNC(P, await_eq, mo)(V_MAP_A(P) *a,            \
													 V_MAP_T(P) v)             \
	{                                                                          \
		V_MAP_T(P) val;                                                        \
		V_ASM(V_LD(mo, V_MAP_R(P), val, a) V_CMP(V_MAP_R(P), val, exp)         \
				  V_AWAIT_B(eq, 2f) V_SEVL V_CODE_ALIGN                        \
			  "1:\n" V_WFE V_LD_WFE(mo, V_MAP_R(P), val, a)                    \
				  V_CMP(V_MAP_R(P), val, exp) V_AWAIT_BNOT(eq, 1b) "2:\n"      \
			  : [val] "=&r"(val)                                               \
			  : [exp] "r"(v), [a] "Q"(a->_v)                                   \
			  : "memory", "cc");                                               \
		return val;                                                            \
	}

/*******************************************************************************
 * vatomic_await functions (ternary variants)
 ******************************************************************************/

#define V_AWAIT_OP(op, T, newv, oldv, v)                                       \
	V_CONCAT(V_MAP_await_op_, op)(T, newv, oldv, v) "\n"
#define V_MAP_await_op_add(T, newv, oldv, v) "add" V_R3(T, newv, T, oldv, T, v)
#define V_MAP_await_op_sub(T, newv, oldv, v) "sub" V_R3(T, newv, T, oldv, T, v)
#define V_MAP_await_op_set(T, newv, oldv, v) "mov" V_R2(T, newv, T, v)

#define V_DEFINE_await_COND_OP(P, mo, cond, op)                                \
	static inline V_MAP_T(P) V_FUNC(P, await, cond, op, mo)(                   \
		V_MAP_A(P) *a, V_MAP_T(P) c, V_MAP_T(P) v)                             \
	{                                                                          \
		V_MAP_T(P) oldv;                                                       \
		V_MAP_T(P) newv;                                                       \
		vuint32_t tmp;                                                         \
		V_ASM(                                                                 \
			V_PRFM(a) V_LDX(mo, V_MAP_R(P), oldv, a)                           \
				V_CMP(V_MAP_R(P), oldv, cmpv) V_AWAIT_B(cond, 3f) V_CODE_ALIGN \
			"1:\n" V_WFE "2:\n" V_LDX(mo, V_MAP_R(P), oldv, a)                 \
				V_CMP(V_MAP_R(P), oldv, cmpv) V_AWAIT_BNOT(                    \
					cond, 1b) "3:\n" V_AWAIT_OP(op, V_MAP_R(P), newv, oldv, v) \
					V_STX(mo, V_MAP_R(P), tmp, newv, a) V_CBNZ("w", tmp, 2b)   \
			: [oldv] "=&r"(oldv), [newv] "=&r"(newv), [tmp] "=&r"(tmp)         \
			: [cmpv] "r"(c), [v] "r"(v), [a] "Q"(a->_v)                        \
			: "memory", "cc");                                                 \
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

V_DEFINE_FUNC(vatomic32, get_max, mo_seq)
#define VATOMIC32_GET_MAX
V_DEFINE_FUNC(vatomic32, get_max, mo_acq)
#define VATOMIC32_GET_MAX_ACQ
V_DEFINE_FUNC(vatomic32, get_max, mo_rel)
#define VATOMIC32_GET_MAX_REL
V_DEFINE_FUNC(vatomic32, get_max, mo_rlx)
#define VATOMIC32_GET_MAX_RLX
V_DEFINE_FUNC(vatomic64, get_max, mo_seq)
#define VATOMIC64_GET_MAX
V_DEFINE_FUNC(vatomic64, get_max, mo_acq)
#define VATOMIC64_GET_MAX_ACQ
V_DEFINE_FUNC(vatomic64, get_max, mo_rel)
#define VATOMIC64_GET_MAX_REL
V_DEFINE_FUNC(vatomic64, get_max, mo_rlx)
#define VATOMIC64_GET_MAX_RLX

V_DEFINE_FUNC(vatomic32, max, mo_seq)
#define VATOMIC32_MAX
V_DEFINE_FUNC(vatomic32, max, mo_rel)
#define VATOMIC32_MAX_REL
V_DEFINE_FUNC(vatomic32, max, mo_rlx)
#define VATOMIC32_MAX_RLX
V_DEFINE_FUNC(vatomic64, max, mo_seq)
#define VATOMIC64_MAX
V_DEFINE_FUNC(vatomic64, max, mo_rel)
#define VATOMIC64_MAX_REL
V_DEFINE_FUNC(vatomic64, max, mo_rlx)
#define VATOMIC64_MAX_RLX

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

V_DEFINE_FUNC(vatomic32, and, mo_seq)
#define VATOMIC32_AND
V_DEFINE_FUNC(vatomic32, and, mo_rel)
#define VATOMIC32_AND_REL
V_DEFINE_FUNC(vatomic32, and, mo_rlx)
#define VATOMIC32_AND_RLX
V_DEFINE_FUNC(vatomic64, and, mo_seq)
#define VATOMIC64_AND
V_DEFINE_FUNC(vatomic64, and, mo_rel)
#define VATOMIC64_AND_REL
V_DEFINE_FUNC(vatomic64, and, mo_rlx)
#define VATOMIC64_AND_RLX

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

V_DEFINE_FUNC(vatomic32, or, mo_seq)
#define VATOMIC32_OR
V_DEFINE_FUNC(vatomic32, or, mo_rel)
#define VATOMIC32_OR_REL
V_DEFINE_FUNC(vatomic32, or, mo_rlx)
#define VATOMIC32_OR_RLX
V_DEFINE_FUNC(vatomic64, or, mo_seq)
#define VATOMIC64_OR
V_DEFINE_FUNC(vatomic64, or, mo_rel)
#define VATOMIC64_OR_REL
V_DEFINE_FUNC(vatomic64, or, mo_rlx)
#define VATOMIC64_OR_RLX

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

V_DEFINE_FUNC(vatomic32, xor, mo_seq)
#define VATOMIC32_XOR
V_DEFINE_FUNC(vatomic32, xor, mo_rel)
#define VATOMIC32_XOR_REL
V_DEFINE_FUNC(vatomic32, xor, mo_rlx)
#define VATOMIC32_XOR_RLX
V_DEFINE_FUNC(vatomic64, xor, mo_seq)
#define VATOMIC64_XOR
V_DEFINE_FUNC(vatomic64, xor, mo_rel)
#define VATOMIC64_XOR_REL
V_DEFINE_FUNC(vatomic64, xor, mo_rlx)
#define VATOMIC64_XOR_RLX

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

V_DEFINE_FUNC(vatomic32, add, mo_seq)
#define VATOMIC32_ADD
V_DEFINE_FUNC(vatomic32, add, mo_rel)
#define VATOMIC32_ADD_REL
V_DEFINE_FUNC(vatomic32, add, mo_rlx)
#define VATOMIC32_ADD_RLX
V_DEFINE_FUNC(vatomic64, add, mo_seq)
#define VATOMIC64_ADD
V_DEFINE_FUNC(vatomic64, add, mo_rel)
#define VATOMIC64_ADD_REL
V_DEFINE_FUNC(vatomic64, add, mo_rlx)
#define VATOMIC64_ADD_RLX

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

V_DEFINE_FUNC(vatomic32, sub, mo_seq)
#define VATOMIC32_SUB
V_DEFINE_FUNC(vatomic32, sub, mo_rel)
#define VATOMIC32_SUB_REL
V_DEFINE_FUNC(vatomic32, sub, mo_rlx)
#define VATOMIC32_SUB_RLX
V_DEFINE_FUNC(vatomic64, sub, mo_seq)
#define VATOMIC64_SUB
V_DEFINE_FUNC(vatomic64, sub, mo_rel)
#define VATOMIC64_SUB_REL
V_DEFINE_FUNC(vatomic64, sub, mo_rlx)
#define VATOMIC64_SUB_RLX

V_DEFINE_FUNC(vatomic32, await, mo_seq, neq, lt, le, gt, ge)
#define VATOMIC32_AWAIT_NEQ
#define VATOMIC32_AWAIT_LT
#define VATOMIC32_AWAIT_LE
#define VATOMIC32_AWAIT_GT
#define VATOMIC32_AWAIT_GE
V_DEFINE_FUNC(vatomic32, await, mo_acq, neq, lt, le, gt, ge)
#define VATOMIC32_AWAIT_NEQ_ACQ
#define VATOMIC32_AWAIT_LT_ACQ
#define VATOMIC32_AWAIT_LE_ACQ
#define VATOMIC32_AWAIT_GT_ACQ
#define VATOMIC32_AWAIT_GE_ACQ
V_DEFINE_FUNC(vatomic32, await, mo_rlx, neq, lt, le, gt, ge)
#define VATOMIC32_AWAIT_NEQ_RLX
#define VATOMIC32_AWAIT_LT_RLX
#define VATOMIC32_AWAIT_LE_RLX
#define VATOMIC32_AWAIT_GT_RLX
#define VATOMIC32_AWAIT_GE_RLX
V_DEFINE_FUNC(vatomic64, await, mo_seq, neq, lt, le, gt, ge)
#define VATOMIC64_AWAIT_NEQ
#define VATOMIC64_AWAIT_LT
#define VATOMIC64_AWAIT_LE
#define VATOMIC64_AWAIT_GT
#define VATOMIC64_AWAIT_GE
V_DEFINE_FUNC(vatomic64, await, mo_acq, neq, lt, le, gt, ge)
#define VATOMIC64_AWAIT_NEQ_ACQ
#define VATOMIC64_AWAIT_LT_ACQ
#define VATOMIC64_AWAIT_LE_ACQ
#define VATOMIC64_AWAIT_GT_ACQ
#define VATOMIC64_AWAIT_GE_ACQ
V_DEFINE_FUNC(vatomic64, await, mo_rlx, neq, lt, le, gt, ge)
#define VATOMIC64_AWAIT_NEQ_RLX
#define VATOMIC64_AWAIT_LT_RLX
#define VATOMIC64_AWAIT_LE_RLX
#define VATOMIC64_AWAIT_GT_RLX
#define VATOMIC64_AWAIT_GE_RLX

V_DEFINE_FUNC(vatomic32, await_eq, mo_seq)
#define VATOMIC32_AWAIT_EQ
V_DEFINE_FUNC(vatomic32, await_eq, mo_acq)
#define VATOMIC32_AWAIT_EQ_ACQ
V_DEFINE_FUNC(vatomic32, await_eq, mo_rlx)
#define VATOMIC32_AWAIT_EQ_RLX
V_DEFINE_FUNC(vatomic64, await_eq, mo_seq)
#define VATOMIC64_AWAIT_EQ
V_DEFINE_FUNC(vatomic64, await_eq, mo_acq)
#define VATOMIC64_AWAIT_EQ_ACQ
V_DEFINE_FUNC(vatomic64, await_eq, mo_rlx)
#define VATOMIC64_AWAIT_EQ_RLX

V_DEFINE_FUNC(vatomicptr, await_eq, mo_seq)
#define VATOMICPTR_AWAIT_EQ
V_DEFINE_FUNC(vatomicptr, await_eq, mo_acq)
#define VATOMICPTR_AWAIT_EQ_ACQ
V_DEFINE_FUNC(vatomicptr, await_eq, mo_rlx)
#define VATOMICPTR_AWAIT_EQ_RLX

V_DEFINE_FUNC(vatomicptr, await, mo_seq, neq)
#define VATOMICPTR_AWAIT_NEQ
V_DEFINE_FUNC(vatomicptr, await, mo_acq, neq)
#define VATOMICPTR_AWAIT_NEQ_ACQ
V_DEFINE_FUNC(vatomicptr, await, mo_rlx, neq)
#define VATOMICPTR_AWAIT_NEQ_RLX

#endif /* VATOMIC_ARM64_H */
