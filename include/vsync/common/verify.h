/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_VERIFY_UTILS_H
#define VSYNC_VERIFY_UTILS_H

#include <vsync/vtypes.h>
#include <vsync/common/macros.h>

#if defined(VSYNC_VERIFICATION)
	#if defined(VSYNC_VERIFICATION_DAT3M)
	#else
		#include <genmc.h>
	#endif
static inline void
verification_ignore(void)
{
	__VERIFIER_assume(0);
}
static inline void
verification_assume(vbool_t cond)
{
	__VERIFIER_assume(cond);
}
static inline void
verification_loop_begin(void)
{
	__VERIFIER_loop_begin();
}
static inline void
verification_spin_start(void)
{
	__VERIFIER_spin_start();
}
static inline void
verification_spin_end(int v)
{
	__VERIFIER_spin_end(v);
}
static inline int
verification_rand(void)
{
	return __VERIFIER_nondet_int();
}
#else
static inline void
verification_ignore(void)
{
}
static inline void
verification_assume(vbool_t condition)
{
	V_UNUSED(condition);
}
static inline void
verification_loop_begin(void)
{
}
static inline void
verification_spin_start(void)
{
}
static inline void
verification_spin_end(int v)
{
	V_UNUSED(v);
}
static inline int
verification_rand(void)
{
	return 0;
}
#endif

#if defined(VSYNC_VERIFICATION) && defined(VSYNC_VERIFICATION_DAT3M)
static inline void
verification_loop_bound(vuint32_t bound)
{
	__VERIFIER_loop_bound(bound);
}
#else
static inline void
verification_loop_bound(vuint32_t bound)
{
	V_UNUSED(bound);
}
#endif

#endif
