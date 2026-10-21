/*
 * vector_filter.h
 *
 *  Created on: Nov 12, 2013
 *      Author: hxin
 */

#ifndef VECTOR_FILTER_H_
#define VECTOR_FILTER_H_

#include <stdint.h>
#include <x86intrin.h>

#ifndef __aligned__
#define __aligned__ __attribute__((aligned(64)))
#endif

// read and ref need to be 16 aligned
int bit_vec_filter_sse(__m128i read_XMM0, __m128i read_XMM1,
		__m128i ref_XMM0, __m128i ref_XMM1, int length, int max_error);

int bit_vec_filter_avx(__m256i read_YMM0, __m256i read_YMM1,
		__m256i ref_YMM0, __m256i ref_YMM1, int length, int max_error);

int bit_vec_filter_avx(__m256i *xor_masks, int length, int max_error);

#ifdef USE_AVX512
int bit_vec_filter_avx512(__m512i read_ZMM0, __m512i read_ZMM1,
		__m512i ref_ZMM0, __m512i ref_ZMM1, int length, int max_error);

int bit_vec_filter_avx512(__m512i *xor_masks, int length, int max_error);
#endif

#endif /* VECTOR_FILTER_H_ */
