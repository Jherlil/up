/*
 * This file is part of the BSGS distribution (https://github.com/JeanLucPons/BSGS).
 * Copyright (c) 2020 Jean Luc PONS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/


#include "Random.h"

#if defined(_WIN64) && !defined(__CYGWIN__)
#else
#include <sys/random.h>
#endif

#ifdef __unix__
#ifdef __CYGWIN__
#else
#include <linux/random.h>
#endif
#endif

static inline uint64_t rotl64(uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

static inline uint64_t splitmix64(uint64_t &state) {
    uint64_t z = (state += 0x9E3779B97f4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

thread_local uint64_t xs_state[4];

uint64_t xoshiro256pp() {
    uint64_t result = rotl64(xs_state[0] + xs_state[3], 23) + xs_state[0];
    uint64_t t = xs_state[1] << 17;

    xs_state[2] ^= xs_state[0];
    xs_state[3] ^= xs_state[1];
    xs_state[1] ^= xs_state[2];
    xs_state[0] ^= xs_state[3];

    xs_state[2] ^= t;
    xs_state[3] = rotl64(xs_state[3], 45);

    return result;
}


// Initialise the random generator with the specified seed
void rseed(unsigned long seed) {
    uint64_t sm = seed;
    xs_state[0] = splitmix64(sm);
    xs_state[1] = splitmix64(sm);
    xs_state[2] = splitmix64(sm);
    xs_state[3] = splitmix64(sm);
}

#if defined(_WIN64) && !defined(__CYGWIN__)
unsigned long rndl() {
    return (unsigned long)xoshiro256pp();
}
#else
unsigned long rndl() {
    unsigned long r;
    int bytes_read = getrandom(&r, sizeof(unsigned long), GRND_NONBLOCK );
    if (bytes_read > 0) {
        return r;
    }
    return (unsigned long)xoshiro256pp();
}
#endif

// Returns a uniform distributed double value in the interval ]0,1[
double rnd() {
    return (xoshiro256pp() >> 11) * (1.0/9007199254740992.0);
}

#ifdef __AVX2__
static inline __m256i rotl256(__m256i x, int k) {
    return _mm256_or_si256(_mm256_slli_epi64(x, k), _mm256_srli_epi64(x, 64 - k));
}

void Xoshiro8::seed(uint64_t base) {
    uint64_t tmp[16];
    uint64_t state = base;
    for (int i = 0; i < 16; ++i) {
        tmp[i] = splitmix64(state);
    }
    s0 = _mm256_loadu_si256((__m256i*)(tmp));
    s1 = _mm256_loadu_si256((__m256i*)(tmp + 4));
    s2 = _mm256_loadu_si256((__m256i*)(tmp + 8));
    s3 = _mm256_loadu_si256((__m256i*)(tmp + 12));
}

void Xoshiro8::next8(uint64_t out[8]) {
    __m256i r0 = rotl256(_mm256_add_epi64(s0, s3), 23);
    r0 = _mm256_add_epi64(r0, s0);
    __m256i t0 = _mm256_slli_epi64(s1, 17);
    s2 = _mm256_xor_si256(s2, s0);
    s3 = _mm256_xor_si256(s3, s1);
    s1 = _mm256_xor_si256(s1, s2);
    s0 = _mm256_xor_si256(s0, s3);
    s2 = _mm256_xor_si256(s2, t0);
    s3 = rotl256(s3, 45);
    _mm256_storeu_si256((__m256i*)out, r0);

    __m256i r1 = rotl256(_mm256_add_epi64(s0, s3), 23);
    r1 = _mm256_add_epi64(r1, s0);
    t0 = _mm256_slli_epi64(s1, 17);
    s2 = _mm256_xor_si256(s2, s0);
    s3 = _mm256_xor_si256(s3, s1);
    s1 = _mm256_xor_si256(s1, s2);
    s0 = _mm256_xor_si256(s0, s3);
    s2 = _mm256_xor_si256(s2, t0);
    s3 = rotl256(s3, 45);
    _mm256_storeu_si256((__m256i*)(out + 4), r1);
}
#endif
