/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TIME_MEASUREMENT
#define VSYNC_TIME_MEASUREMENT

#include <vsync/vtypes.h>
#include <vsync/common/compiler.h>
#include <stdio.h>
#include <time.h>

#define VTIME_MILLISEC_IN_SEC 1000U
#define VTIME_MICROSEC_IN_SEC (VTIME_MILLISEC_IN_SEC * 1000)
#define VTIME_NANOSEC_IN_SEC  (VTIME_MICROSEC_IN_SEC * 1000)

typedef struct timespec cpu_time_t;

/**
 * calls rdtsc instruction Read Time-Stamp Counter
 * @ref https://helloacm.com/the-rdtsc-performance-timer-written-in-c/
 * @ref https://github.com/google/benchmark/blob/v1.1.0/src/cycleclock.h#L102
 * @return the number of clock cycles since last reset
 */
static inline vuint64_t
read_time_stamp_counter(void)
{
    barrier();
#if defined(__x86_64__)
    unsigned int lo, hi;
    const vuint32_t shift_to_msb = 32;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    barrier();
    return ((vuint64_t)hi << shift_to_msb) | lo;
#elif defined(__aarch64__)
    int64_t virtual_timer_value;
    asm volatile("mrs %0, cntvct_el0" : "=r"(virtual_timer_value));
    barrier();
    return virtual_timer_value;
#else
    ASSERT(0 && "implement read_time_stamp_counter for the target arch");
#endif
}

static inline vuint64_t
calc_ticks_in_ms(void)
{
#if defined(VSYNC_VERIFICATION)
    return 0;
#else
    vuint64_t ticks_before  = 0;
    vuint64_t ticks_after   = 0;
    vuint64_t ticks_elapsed = 0;
    vuint64_t milliseconds  = 1000;
    // 1 millisecond is 1000 microseconds
    vuint64_t microseconds = 1000 * milliseconds;
    ticks_before           = read_time_stamp_counter();
    // sleeps for the given amount of microseconds
    usleep(microseconds);
    ticks_after   = read_time_stamp_counter();
    ticks_elapsed = ticks_after - ticks_before;
    // calculate how many ticks correlate to a millisecond
    vuint64_t ticks_in_millisecond = ticks_elapsed / milliseconds;
    return ticks_in_millisecond;
#endif
}

/**
 * start_recording_time
 *
 * @param _time
 */
static inline void
record_time(cpu_time_t *_time)
{
#if !defined(VSYNC_VERIFICATION)
    clock_gettime(CLOCK_REALTIME, _time);
#endif
}

/**
 *
 * @param start_time
 * @param finish_time
 * @return long the spent time in micro-seconds
 */
static inline long
calc_spent_time(cpu_time_t start_time, cpu_time_t finish_time)
{
#if defined(VSYNC_VERIFICATION)
    return 0;
#else
    return (finish_time.tv_sec - start_time.tv_sec) * VTIME_MICROSEC_IN_SEC +
           (finish_time.tv_nsec - start_time.tv_nsec) / VTIME_MILLISEC_IN_SEC;
#endif
}
/**
 *
 * @param start_time
 * @param finish_time
 * @return long eturns the spent time in nano-seconds
 */
static inline long
calc_spent_time_nano(cpu_time_t start_time, cpu_time_t finish_time)
{
#if defined(VSYNC_VERIFICATION)
    return 0;
#else
    return (finish_time.tv_sec - start_time.tv_sec) * VTIME_NANOSEC_IN_SEC +
           (finish_time.tv_nsec - start_time.tv_nsec);
#endif
}
/**
 * Returns a string representing the local time
 *
 * @return char* local time in string format
 * @ref: http://www.cplusplus.com/reference/ctime/localtime/
 */
static inline char *
get_local_time(void)
{
    time_t rawtime;
    struct tm *timeinfo;
    (void)time(&rawtime);
    timeinfo = localtime(&rawtime);
    return asctime(timeinfo);
}


#endif
