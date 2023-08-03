/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef VSYNC_ALLOC_H
#define VSYNC_ALLOC_H

#include <vsync/vtypes.h>

typedef void (*vmem_free_fun_t)(void *ptr, void *arg);
typedef void *(*vmem_malloc_fun_t)(vsize_t sz, void *arg);

typedef struct vmem_lib_s {
	vmem_free_fun_t free_fun;
	vmem_malloc_fun_t malloc_fun;
	void *arg;
} vmem_lib_t;

static inline vbool_t
vmem_lib_not_null(vmem_lib_t *vmem)
{
	if (!vmem->free_fun) {
		return false;
	}
	if (!vmem->malloc_fun) {
		return false;
	}
	return true;
}

static inline void
vmem_lib_copy(vmem_lib_t *des, vmem_lib_t *src)
{
#if defined(VSYNC_VERIFICATION)
	des->free_fun	= src->free_fun;
	des->malloc_fun = src->malloc_fun;
	des->arg		= src->arg;
#else
	*des = *src;
#endif
}

#endif
