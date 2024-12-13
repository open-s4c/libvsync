/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 * Author: Huawei Dresden Research Center
 * Create: 2022-10-19
 */
#ifndef VSYNC_VFREESTANDING_H
#define VSYNC_VFREESTANDING_H

#define vbool_t   _Bool
#define vuint8_t  unsigned char
#define vuint16_t unsigned short int
#define vuint32_t unsigned int

#ifndef NULL
#define NULL ((void *)0)
#endif
#ifndef __SIZEOF_LONG__
    #error "__SIZEOF_LONG__ not defined"
#endif /* __SIZEOF_LONG__ */

#ifndef __SIZE_TYPE__
    #error "__SIZE_TYPE__ not defined"
#endif /* __SIZE_TYPE__ */

#ifndef __SIZEOF_POINTER__
    #error "__SIZEOF_POINTER__ not defined"
#endif /* __SIZEOF_POINTER */

#if 8 == __SIZEOF_LONG__
    #define vuint64_t unsigned long int
#else
    #if 8 == __SIZEOF_LONG_LONG__
        #define vuint64_t unsigned long long int
    #else
        #error "long long must be 64 bits on this architecture"
    #endif /* 8 == __SIZEOF_LONG_LONG__ */
#endif     /* 8 == __SIZEOF_LONG__ */

#if 8 == __SIZEOF_POINTER__
    #define vuintptr_t vuint64_t
#else
    #define vuintptr_t vuint32_t
#endif /* 8 == __SIZEOF_POINTER__ */

typedef __SIZE_TYPE__ vsize_t;

#endif
