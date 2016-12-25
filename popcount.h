/*
 * popcount.h
 *
 *  Created on: Nov 7, 2013
 *      Author: hxin
 */

#ifndef POPCOUNT_H_
#define POPCOUNT_H_

#ifndef __aligned__
	#define __aligned__ __attribute__((aligned(16)))
#endif

#include <stdint.h>
#include <nmmintrin.h>
#include <immintrin.h>

uint32_t popcount_m128i_sse(__m128i reg);

uint32_t popcount_m256i_sse(__m256i reg);

uint32_t builtin_popcount(uint8_t* buffer, int chunks16);

uint32_t popcount(uint8_t *buffer, int chunks16);

#endif /* POPCOUNT_H_ */
