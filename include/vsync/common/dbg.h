/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_UTILS_DBG
#define VSYNC_UTILS_DBG

#include <vsync/common/macros.h>

#ifdef VSYNC_ENABLE_DEBUG
    #include <vsync/utils/string.h>
    #include <vsync/vtypes.h>
    #include <stdio.h>

    #define VDBG_FILENAME                                                      \
        (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

    #if defined(DBG_ENABLE_COL_ALL)
        #define DBG_ENABLE_COL_WHITE
        #define DBG_ENABLE_COL_MAGENTA
        #define DBG_ENABLE_COL_YELLOW
        #define DBG_ENABLE_COL_GREEN
        #define DBG_ENABLE_COL_GRAY
        #define DBG_ENABLE_COL_CYAN
        #define DBG_ENABLE_COL_BLUE
        #define DBG_ENABLE_COL_RED
        #define DBG_ENABLE_COL_L_MAGENTA
        #define DBG_ENABLE_COL_L_YELLOW
        #define DBG_ENABLE_COL_L_GREEN
        #define DBG_ENABLE_COL_L_GRAY
        #define DBG_ENABLE_COL_L_CYAN
        #define DBG_ENABLE_COL_L_BLUE
        #define DBG_ENABLE_COL_L_RED
    #endif

    /* DBG_IS_ENABLED(X) determines whether color X is enabled or not in
     * runtime (eg, X == COL_RED). If DBG_ENABLE_X is NOT defined then
     * DBG_IS_ENABLED_ generates the string "DBG_ENABLE_X". If DBG_ENABLE_X is
     * defined, then DBG_IS_ENABLED_ generates the string "1" or "" depending on
     * whether DBG_ENABLE_X was defined as a -D argument to the compiler or
     * inside the source code. Therefore, DBG_IS_ENABLED(X) returns TRUE for "1"
     * and "". */

    #define DBG_IS_ENABLED(COLOR)                                              \
        (strcmp(DBG_IS_ENABLED_(COLOR), "1") == 0 ||                           \
         strcmp(DBG_IS_ENABLED_(COLOR), "") == 0)

    #define DBG_IS_ENABLED_(COLOR) V_STRING(COLOR)

    /******************************************************************************/
    #define COL_RED       "31"
    #define COL_GREEN     "32"
    #define COL_YELLOW    "33"
    #define COL_BLUE      "34"
    #define COL_MAGENTA   "35"
    #define COL_CYAN      "36"
    #define COL_GRAY      "90"
    #define COL_WHITE     "97"
    #define COL_L_GRAY    "37"
    #define COL_L_RED     "91"
    #define COL_L_GREEN   "92"
    #define COL_L_YELLOW  "93"
    #define COL_L_BLUE    "94"
    #define COL_L_MAGENTA "95"
    #define COL_L_CYAN    "96"
    /******************************************************************************/
    #define DBG_COL_PREFIX(_color_) "\033[1;" _color_ "m"
    #define DBG_COL_SUFFIX          "\033[0m"
    #define DBG_COL_STRING(_s_, _color_)                                       \
        DBG_COL_PREFIX(_color_) _s_ DBG_COL_SUFFIX

    #define DBG_COLOR(COLOR, ...)                                              \
        if (DBG_IS_ENABLED(DBG_ENABLE_##COLOR)) {                              \
            DBG_COLOR_(COLOR, __VA_ARGS__);                                    \
        }

    #ifdef VSYNC_VERIFICATION
        /* #genmcbug https://github.com/MPI-SWS/genmc/issues/47 The current
         * version of genmc crashes when stderr if used, for now we use printf
         */
        #define DBG_COLOR_(COLOR, _format_, ...)                               \
            (void)printf("[%s:%d:%s]\033[1;" COLOR "m:" _format_ "\033[0m\n",  \
                         __FILE__, __LINE__, __func__, ##__VA_ARGS__)
    #else
        #define DBG_COLOR_(COLOR, _format_, ...)                               \
            (void)fprintf(stderr,                                              \
                          "[%s:%d:%s]\033[1;" COLOR "m:" _format_ "\033[0m\n", \
                          __FILE__, __LINE__, __func__, ##__VA_ARGS__)
    #endif
#else
    #define DBG_COLOR(color, _format_, ...) V_UNUSED(__VA_ARGS__)
    #define DBG_COL_STRING(s, ...)          s
#endif

#define DBG_MAGENTA(_format_, ...)                                             \
    DBG_COLOR(COL_MAGENTA, _format_, ##__VA_ARGS__)
#define DBG_YELLOW(_format_, ...) DBG_COLOR(COL_YELLOW, _format_, ##__VA_ARGS__)
#define DBG_GREEN(_format_, ...)  DBG_COLOR(COL_GREEN, _format_, ##__VA_ARGS__)
#define DBG_BLUE(_format_, ...)   DBG_COLOR(COL_BLUE, _format_, ##__VA_ARGS__)
#define DBG_GRAY(_format_, ...)   DBG_COLOR(COL_GRAY, _format_, ##__VA_ARGS__)
#define DBG_RED(_format_, ...)    DBG_COLOR(COL_RED, _format_, ##__VA_ARGS__)
#define DBG(_format_, ...)        DBG_COLOR(COL_WHITE, _format_, ##__VA_ARGS__)

#endif
