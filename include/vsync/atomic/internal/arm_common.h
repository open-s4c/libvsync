/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef VATOMIC_ARM_COMMON_H
#define VATOMIC_ARM_COMMON_H

/*******************************************************************************
 * assembly instructions and helpers
 ******************************************************************************/

#define V_ASM(...) __asm__ volatile(__VA_ARGS__)

/* register helpers */
#define V_R(T, r)					 " %" T "[" #r "]"
#define V_R2(T1, r1, T2, r2)		 V_R(T1, r1) "," V_R(T2, r2)
#define V_R3(T1, r1, T2, r2, T3, r3) V_R2(T1, r1, T2, r2) "," V_R(T3, r3)

/* the exact fence instruction depends on the architecture */
#define V_FENCE(mo) V_CONCAT(V_FENCE_, mo)

#define V_BEQ(l) "beq " #l "\n"
#define V_BNE(l) "bne " #l "\n"
#define V_BHS(l) "bhs " #l "\n"

/*******************************************************************************
 * vatomic_fence
 ******************************************************************************/

#define V_DEFINE_fence(P, mo)                                                  \
	static inline void V_FUNC(P, fence, mo)(void)                              \
	{                                                                          \
		V_ASM(V_FENCE(mo)::: "memory");                                        \
	}

#if !defined(VATOMIC_DISABLE_POLITE_AWAIT)
	#define vatomic_cpu_pause() __asm__ volatile("yield\n" ::: "memory")
#endif

#endif /* VATOMIC_ARM_COMMON_H */
