/*
 * shift.h
 *
 *  Created on: Dec 25 2016
 *      Author: Hongyi Xin
 */

#ifndef __SHIFT_H_
#define __SHIFT_H_

#include <stdint.h>
#include <x86intrin.h>

#ifndef __aligned__
	#define __aligned__ __attribute__((aligned(64)))
#endif

// read and ref need to be 16 aligned
__m128i shift_right_sse(__m128i vec, int shift_num);
__m128i shift_left_sse(__m128i vec, int shift_num);
__m256i shift_right_avx(__m256i vec, int shift_num);
__m256i shift_left_avx(__m256i vec, int shift_num);
#ifdef USE_AVX512
__m512i shift_right_avx512(__m512i vec, int shift_num);
__m512i shift_left_avx512(__m512i vec, int shift_num);
#endif

#endif /* __SHIFT_H_ */
