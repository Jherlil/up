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

#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>
#include <immintrin.h>
double rnd();
unsigned long rndl();
void rseed(unsigned long seed);

extern thread_local uint64_t xs_state[4];
uint64_t xoshiro256pp();
#ifdef __AVX2__
struct Xoshiro8 {
    __m256i s0,s1,s2,s3;
    void seed(uint64_t base);
    void next8(uint64_t out[8]);
};
#endif

#endif
