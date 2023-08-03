/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef VSYNC_COMMON_ASSERT_H
#define VSYNC_COMMON_ASSERT_H

#ifndef ASSERT
	/* TODO: on `VSYNC_VERIFICATION` for dat3m we might need to call a special
	 * thing */
	#include <assert.h>
	#define ASSERT(V) assert(V)
#endif

#ifndef BUG_ON
	#define BUG_ON(V) ASSERT(!(V))
#endif

#define V_STATIC_ASSERT(_cond_, _str_) _Static_assert((_cond_), _str_)
#endif
