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
	if (shift_num <= 0)
		return vec;
	if (shift_num >= 256)
		return _mm256_setzero_si256();

	__m128i low = _mm256_extracti128_si256(vec, 0);
	__m128i high = _mm256_extracti128_si256(vec, 1);

	if (shift_num >= 128) {
		high = shift_right_sse(low, shift_num - 128);
		low = _mm_setzero_si128();
	}
	else {
		__m128i carryover = shift_left_sse(low, 128 - shift_num);
		low = shift_right_sse(low, shift_num);
		high = _mm_or_si128(shift_right_sse(high, shift_num), carryover);
	}

	return _mm256_inserti128_si256(_mm256_castsi128_si256(low), high, 1);
}

__m256i shift_left_avx(__m256i vec, int shift_num) {
	if (shift_num <= 0)
		return vec;
	if (shift_num >= 256)
		return _mm256_setzero_si256();

	__m128i low = _mm256_extracti128_si256(vec, 0);
	__m128i high = _mm256_extracti128_si256(vec, 1);

	if (shift_num >= 128) {
		low = shift_left_sse(high, shift_num - 128);
		high = _mm_setzero_si128();
	}
	else {
		__m128i carryover = shift_right_sse(high, 128 - shift_num);
		high = shift_left_sse(high, shift_num);
		low = _mm_or_si128(shift_left_sse(low, shift_num), carryover);
	}

	return _mm256_inserti128_si256(_mm256_castsi128_si256(low), high, 1);
}

#ifdef USE_AVX512
__m512i shift_right_avx512(__m512i vec, int shift_num) {
	if (shift_num >= 256) {
		vec = _mm512_maskz_permutexvar_epi64(0xf0,
				_mm512_set_epi64(3, 2, 1, 0, 0, 0, 0, 0), vec);
		shift_num = shift_num % 256;
	}
	if (shift_num >= 128) {
		vec = _mm512_maskz_permutexvar_epi64(0xfc,
				_mm512_set_epi64(5, 4, 3, 2, 1, 0, 0, 0), vec);
		shift_num = shift_num % 128;
	}
	if (shift_num >= 64) {
		vec = _mm512_maskz_permutexvar_epi64(0xfe,
				_mm512_set_epi64(6, 5, 4, 3, 2, 1, 0, 0), vec);
		shift_num = shift_num % 64;
	}

	__m512i carryover = _mm512_maskz_permutexvar_epi64(0xfe,
			_mm512_set_epi64(6, 5, 4, 3, 2, 1, 0, 0), vec);
	carryover = _mm512_srli_epi64(carryover, 64 - shift_num);
	vec = _mm512_slli_epi64(vec, shift_num);
	return _mm512_or_si512(vec, carryover);
}

__m512i shift_left_avx512(__m512i vec, int shift_num) {
	if (shift_num >= 256) {
		vec = _mm512_maskz_permutexvar_epi64(0x0f,
				_mm512_set_epi64(11, 10, 9, 8, 7, 6, 5, 4), vec);
		shift_num = shift_num % 256;
	}
	if (shift_num >= 128) {
		vec = _mm512_maskz_permutexvar_epi64(0x3f,
				_mm512_set_epi64(9, 8, 7, 6, 5, 4, 3, 2), vec);
		shift_num = shift_num % 128;
	}
	if (shift_num >= 64) {
		vec = _mm512_maskz_permutexvar_epi64(0x7f,
				_mm512_set_epi64(8, 7, 6, 5, 4, 3, 2, 1), vec);
		shift_num = shift_num % 64;
	}

	__m512i carryover = _mm512_maskz_permutexvar_epi64(0x7f,
			_mm512_set_epi64(8, 7, 6, 5, 4, 3, 2, 1), vec);
	carryover = _mm512_slli_epi64(carryover, 64 - shift_num);
	vec = _mm512_srli_epi64(vec, shift_num);
	return _mm512_or_si512(vec, carryover);
}
#endif
