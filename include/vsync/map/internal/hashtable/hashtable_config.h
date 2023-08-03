/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef VHASH_CONFIG_H
#define VHASH_CONFIG_H

#ifdef VSYNC_VERIFICATION
	#define VHASHTABLE_BUCKET_COUNT 2
	#define VHASHTABLE_BUCKET_LEN	2
#else
	#ifndef VHASHTABLE_BUCKET_COUNT
		#define VHASHTABLE_BUCKET_COUNT 1024
	#endif
	#define VHASHTABLE_BUCKET_LEN 4
#endif

#endif
