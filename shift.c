/*
 * vector_ed.c
 *
 *  Created on: Nov 8, 2013
 *      Author: hxin
 */

#include "shift.h"
#include <stdio.h>
#include <string.h>

__m128i shift_right_sse(__m128i vec, int shift_num) {
	if (shift_num >= 64) {
		vec = _mm_slli_si128(vec, 8);
		shift_num = shift_num % 64;
	}
	__m128i carryover = _mm_slli_si128(vec, 8);
	carryover = _mm_srli_epi64(carryover, 64 - shift_num);
	vec = _mm_slli_epi64(vec, shift_num);
	return _mm_or_si128(vec, carryover);
}

__m128i shift_left_sse(__m128i vec, int shift_num) {
	if (shift_num >= 64) {
		vec = _mm_srli_si128(vec, 8);
		shift_num = shift_num % 64;
	}
	__m128i carryover = _mm_srli_si128(vec, 8);
	carryover = _mm_slli_epi64(carryover, 64 - shift_num);
	vec = _mm_srli_epi64(vec, shift_num);
	return _mm_or_si128(vec, carryover);
}

__m256i shift_right_avx(__m256i vec, int shift_num) {
	if (shift_num >= 128) {
		vec = _mm256_inserti128_si256(_mm256_setzero_si256(), _mm256_extracti128_si256(vec, 0), 1);
		shift_num = shift_num % 128;
	}
	if (shift_num >= 64) {
		vec = _mm256_slli_si256(vec, 8);
		shift_num = shift_num % 64;
	}
	__m256i carryover = _mm256_slli_si256(vec, 8);
	carryover = _mm256_srli_epi64(carryover, 64 - shift_num);
	vec = _mm256_slli_epi64(vec, shift_num);
	return _mm256_or_si256(vec, carryover);
}

__m256i shift_left_avx(__m256i vec, int shift_num) {
	if (shift_num >= 128) {
		vec = _mm256_inserti128_si256(_mm256_setzero_si256(), _mm256_extracti128_si256(vec, 1), 0);
		shift_num = shift_num % 128;
	}
	if (shift_num >= 64) {
		vec = _mm256_srli_si256(vec, 8);
		shift_num = shift_num % 64;
	}
	__m256i carryover = _mm256_srli_si256(vec, 8);
	carryover = _mm256_slli_epi64(carryover, 64 - shift_num);
	vec = _mm256_srli_epi64(vec, shift_num);
	return _mm256_or_si256(vec, carryover);
}

