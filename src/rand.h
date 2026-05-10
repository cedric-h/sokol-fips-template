/* adapted from https://github.com/libsdl-org/SDL/blob/main/src/stdlib/SDL_random.c */

#ifndef __EAB_RAND_DEF
#define __EAB_RAND_DEF
void rand_seed(uint64_t seed);
int32_t rand_int(int32_t n);
float randf(void);
uint32_t rand_bits(void);
float gaussian_randf(float mean, float stddev);
int gaussian_rand(float mean, float stddev);

#ifdef __EAB_RAND_IMPL
#include <math.h>
static uint64_t rand_state;
static bool rand_initialized = false;

static uint32_t __rand_bits_r(uint64_t *state) {
    if (!state) {
        return 0;
    }

    // The C and A parameters of this LCG have been chosen based on hundreds
    // of core-hours of testing with PractRand and TestU01's Crush.
    // Using a 32-bit A improves performance on 32-bit architectures.
    // C can be any odd number, but < 256 generates smaller code on ARM32
    // These values perform as well as a full 64-bit implementation against
    // Crush and PractRand. Plus, their worst-case performance is better
    // than common 64-bit constants when tested against PractRand using seeds
    // with only a single bit set.

    // We tested all 32-bit and 33-bit A with all C < 256 from a v2 of:
    // Steele GL, Vigna S. Computationally easy, spectrally good multipliers
    // for congruential pseudorandom number generators.
    // Softw Pract Exper. 2022;52(2):443-458. doi: 10.1002/spe.3030
    // https://arxiv.org/abs/2001.05304v2

    *state = *state * 0xff1cd035ul + 0x05;

    // Only return top 32 bits because they have a longer period
    return (uint64_t)(*state >> 32);
}

static int32_t __rand_r(uint64_t *state, int32_t n) {
    // Algorithm: get 32 bits from rand_bits() and treat it as a 0.32 bit
    // fixed point number. Multiply by the 31.0 bit n to get a 31.32 bit
    // result. Shift right by 32 to get the 31 bit integer that we want.

    if (n < 0) {
        // The algorithm looks like it works for numbers < 0 but it has an
        // infinitesimal chance of returning a value out of range.
        // Returning -rand(abs(n)) blows up at INT_MIN instead.
        // It's easier to just say no.
        return 0;
    }

    // On 32-bit arch, the compiler will optimize to a single 32-bit multiply
    uint64_t val = (uint64_t)__rand_bits_r(state) * n;
    return (int32_t)(val >> 32);
}

static float __randf_r(uint64_t *state) {
    // Note: its using 24 bits because float has 23 bits significand + 1 implicit bit
#if (defined(_MSC_VER) && (_MSC_VER < 1913)) || (!defined(_MSC_VER) && (!defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)))
    // no hexidecimal float notation, do it the hard way. MSVC before 15.6 (2017), etc, needs this.
    const union { uint32_t u32; float f; } float_union = { 0x33800000U };
    return (__rand_bits_r(state) >> (32 - 24)) * float_union.f;
#else
    return (__rand_bits_r(state) >> (32 - 24)) * 0x1p-24f;
#endif
}

void rand_seed(uint64_t seed) {
    rand_state = seed;
    rand_initialized = true;
}

int32_t rand_int(int32_t n) {
    if (!rand_initialized) {
        rand_seed(0);
    }

    return __rand_r(&rand_state, n);
}

float randf(void) {
    if (!rand_initialized) {
        rand_seed(0);
    }

    return __randf_r(&rand_state);
}

uint32_t rand_bits(void) {
    if (!rand_initialized) {
        rand_seed(0);
    }

    return __rand_bits_r(&rand_state);
}

float gaussian_randf(float mean, float stddev) {
	float u1 = randf();
	float u2 = randf();

	/* avoid log(0) */
	if (u1 == 0) {
		u1 = 0.00000000001;
	}

	float z0 = sqrtf(-2*logf(u1)) * cosf(2*M_PI*u2);
	return z0*stddev + mean;
}
int gaussian_rand(float mean, float stddev) {
    return roundf(gaussian_randf(mean, stddev));
}

#endif
#endif
