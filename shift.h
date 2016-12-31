/*
 * shift.h
 *
 *  Created on: Dec 25 2016
 *      Author: Hongyi Xin
 */

#ifndef __SHIFT_H_
#define __SHIFT_H_

#ifndef __aligned__
#define __aligned__ __attribute__((aligned(32)))
#endif

#include <stdint.h>
#include <nmmintrin.h>

// read and ref need to be 16 aligned
__m128i shift_right_sse(__m128i vec, int shift_num);
__m128i shift_left_sse(__m128i vec, int shift_num);
__m256i shift_right_avx(__m256i vec, int shift_num);
__m256i shift_left_avx(__m256i vec, int shift_num);

#endif /* __SHIFT_H_ */
