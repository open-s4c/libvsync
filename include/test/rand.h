/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_UTILS_RAND
#define VSYNC_UTILS_RAND

#include <vsync/common/assert.h>
#include <vsync/utils/math.h>
#include <stdlib.h>
#include <time.h>
#include <vsync/vtypes.h>
#include <vsync/common/verify.h>
#include <sys/random.h>

#if defined(VSYNC_VERIFICATION)
static inline vuint64_t
rand_gen_seed(void)
{
    return verification_rand();
}
#else

    #include <time.h>

static inline vuint64_t
rand_gen_seed(void)
{
    return (vuint64_t)time(0);
}

#endif

/**
 * rand is not thread-safe. If you need to use rand in a concurrent algo
 * pre-generate the randoms, save them and then use them.
 * @see random_gen_values
 */

static vbool_t g_rand_initialized          = false;
static __thread unsigned int g_thread_seed = 0;

/**
 * uses time(0) as the seed of srand
 *
 */
static inline void
random_init(void)
{
#if !defined(VSYNC_VERIFICATION)
    unsigned int seed = time(0);
    if (!g_rand_initialized) {
        srand(seed);
        g_rand_initialized = true;
    }
#endif
}
/**
 * Wraps rand
 * @return int what rand returns
 */
static inline int
random_rand(void)
{
#if defined(VSYNC_VERIFICATION)
    return verification_rand();
#elif __APPLE__
    char buf[sizeof(int)] = {0};
    if (getentropy(buf, sizeof(int)) == -1) {
        ASSERT(0 && "getentropy failed");
        return 0;
    } else {
        return *((int *)buf);
    }
#else
    char buf[sizeof(int)] = {0};
    if (getrandom(buf, sizeof(int), GRND_RANDOM) == -1) {
        ASSERT(0 && "getrandom failed");
        return 0;
    } else {
        return *((int *)buf);
    }
#endif
}

/**
 * Wraps rand_r
 * @return int what rand returns
 * @note it is thread safe
 * @note uses thread local storage
 */
static inline int
random_thread_safe_rand(void)
{
#if defined(VSYNC_VERIFICATION)
    return verification_rand();
#else
    if (g_thread_seed == 0) {
        g_thread_seed = time(NULL);
    }
    return rand_r(&g_thread_seed);
#endif
}

/**
 * Generates a random number in [`min`, `max`]
 * @note uses a thread safe random number generator
 * @param min minimum number that can to be generated
 * @param max maximum number that can to be generated
 * @return vuint32_t a number between [`min`, `max`]
 */
static inline vuint32_t
random_thread_safe_get_next(vuint32_t min, vuint32_t max)
{
    ASSERT(min <= max);
    ASSERT(max < VINT32_MAX);
    int r = random_thread_safe_rand();
    if (r < 0) {
        r *= -1;
    }
    return (((vuint32_t)r) % (max - min + 1U)) + min;
}

/**
 * Uses the given seed to initialize srand
 *
 * @param seed
 */
static inline void
random_init_seed(unsigned int seed)
{
#if defined(VSYNC_VERIFICATION)
    (void)seed;
#else
    ASSERT(!g_rand_initialized && "was initialized before");
    srand(seed);
    g_rand_initialized = true;
#endif
}
/**
 * Wraps rand to generate a more of evenly distributed random number
 * @param range
 * @pre range <= 0x0FFFFFF
 * @return vuint32_t
 */
static inline vuint32_t
_random(vuint32_t range)
{
#if defined(VSYNC_VERIFICATION)
    (void)range;
    return verification_rand();
#else
    vuint32_t r        = 0;
    if (!g_rand_initialized) {
        random_init_seed(0);
    }
    ASSERT(g_rand_initialized &&
           "the module needs to be initialized first. call "
           "random_init_seed/random_init");

    ASSERT(range <= 0x0FFFFFFF && "Value is not supported");

    do {
        r = (vuint32_t)random_rand() % v_pow2_round_up(range);
    } while (r >= range);

    return r;
#endif
}
/**
 * Returns a vuint32_t random number in [min, max]
 *
 * @param min
 * @param max
 * @return vuint32_t
 */
static inline vuint32_t
random_next_int(vuint32_t min, vuint32_t max)
{
    vuint32_t random_number = 0;
    ASSERT(max >= min);
    random_number = _random(max - min + 1);
    return random_number + min;
}

/**
 * Returns a vuint64_t random number in [min, max]
 *
 * @param min
 * @param max
 * @param alignment
 * @return vuint64_t
 */
static inline vuint64_t
random_next_int64(vuint64_t min, vuint64_t max, vuint16_t alignment)
{
    vuint64_t random_number = 0;
#if defined(VSYNC_VERIFICATION)
    random_number = verification_rand();
    (void)min;
    (void)max;
    (void)alignment;
#else
    vuint32_t rand_msbits        = 0;
    vuint32_t rand_lsbits        = 0;
    vuint64_t range              = (max - min) + 1;
    const vuint32_t shift_to_msb = 32;
    ASSERT(max >= min);

    rand_lsbits = random_rand();
    rand_msbits = random_rand();

    random_number = ((vuint64_t)rand_msbits << shift_to_msb) | rand_lsbits;

    random_number = (random_number) % range + min;

    random_number = random_number & ~((1 << alignment) - 1);

    ASSERT(random_number >= min);
    ASSERT(random_number <= max);

#endif
    return random_number;
}
/**
 * random_with_probability
 * e.g. given an array of {20, 20, 60} the probabilities of the return value
 * will be as follows
 * 	~ 20%  0
 * 	~ 20%  1
 * 	~ 60%  2
 * @note the sum of the array elements should be exactly 100 (i.e. 100%)
 * @param probabilities an array of the probabilities one wants to achieve
 * @param length of the "probabilities" array
 * @return vuint32_t
 * 	an integer between [0, length-1]
 */
static inline vuint32_t
random_with_probability(vuint32_t *probabilities, vuint32_t length)
{
    vuint32_t r     = random_next_int(1, 100);
    vuint32_t i     = 0;
    vuint32_t start = 0;

    for (i = 0; i < length; i++) {
        if (r > start && r <= (start + probabilities[i])) {
            return i;
        }
        start += probabilities[i];
    }

    ASSERT(0 && "something is wrong this should not be reachable");
    return 0;
}
/**
 * random_next_string
 * creates a non-null-terminating string of the given len
 * @param len length of the desired string
 * @param str output parameter where the string will be stored
 */
static inline void
random_next_string(unsigned char *str, vsize_t len)
{
    vsize_t i = 0;

    for (i = 0; i < len; i++) {
        str[i] = (unsigned char)random_next_int(0, VUINT8_MAX);
    }
}
/**
 * random_next_string
 * creates a non-null-terminating string of the given len
 * @param len length of the desired string
 * @param str output parameter where the string will be stored
 */
static inline void
random_next_printable_string(unsigned char *str, vsize_t len)
{
    vsize_t i                = 0;
    const vuint32_t min_char = 33;
    const vuint32_t max_char = 126;

    for (i = 0; i < len - 1; i++) {
        // 33 = '!'
        // 126 = '~'
        str[i] = (unsigned char)random_next_int(min_char, max_char);
    }
    ASSERT(i == len - 1);
    str[i] = '\0';
}

/**
 * fills the given array with random numbers that fall in [min, max]
 *
 * @param arr vuint32_t array to be filled with random numbers
 * @param len the length of the array
 * @param min
 * @param max
 */
static inline void
random_gen_values(vuint32_t arr[], vsize_t len, vuint32_t min, vuint32_t max)
{
    vsize_t i = 0;

    for (i = 0; i < len; i++) {
        arr[i] = random_next_int(min, max);
    }
}


/* Deterministic random generator that allows multiple seeding,
 * to be used in testing.
 */

static inline void
deterministic_random_init(unsigned int seed)
{
    srand(seed);
}

static inline vuint32_t
deterministic_random(vuint32_t range)
{
    vuint32_t r = 0;
    do {
        r = rand() % v_pow2_round_up(range);
    } while (r >= range);
    return r;
}

static inline vuint32_t
deterministic_random_next_int(vuint32_t min, vuint32_t max)
{
    ASSERT(max >= min);
    return deterministic_random(max - min + 1) + min;
}

static inline void
deterministic_random_shuffle(vsize_t arr[], vsize_t len)
{
    for (vsize_t i = 1; i < len; ++i) {
        vsize_t index = deterministic_random_next_int(0, i - 1);
        vsize_t temp  = arr[i];
        arr[i]        = arr[index];
        arr[index]    = temp;
    }
}

#endif
