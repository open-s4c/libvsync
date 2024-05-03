/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_SMR_INTERFACE_H
#define VSYNC_SMR_INTERFACE_H

#if defined(VSYNC_SMR_NOT_AVAILABLE) || defined(SMR_NONE)
    #include <test/smr/ismr_none.h>
#elif defined(SMR_CLEANUP)
    #include <test/smr/ismr_cleanup.h>
#elif defined(SMR_KCLEANUP)
    #include <test/smr/ismr_kcleanup.h>
#elif defined(SMR_EBR)
    #include <test/smr/ismr_ebr.h>
#elif defined(SMR_GUS)
    #include <test/smr/ismr_gus.h>
#else
    // can be multiple
    #include <test/smr/ismr_gdump.h>
#endif

#endif
