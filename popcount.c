/*
 * popcount.c
 *
 *  Created on: Nov 7, 2013
 *      Author: hxin
 */
#include "popcount.h"
#include <stdio.h>
#include "print.h"

#ifdef DEBUG
#include <assert.h>
#endif

uint8_t POPCOUNT[32] __aligned__ = {
/* 0 */0,
/* 1 */1,
/* 2 */1,
/* 3 */2,
/* 4 */1,
/* 5 */2,
/* 6 */2,
/* 7 */3,
/* 8 */1,
/* 9 */2,
/* a */2,
/* b */3,
/* c */2,
/* d */3,
/* e */3,
/* f */4,
/* 0 */0,
/* 1 */1,
/* 2 */1,
/* 3 */2,
/* 4 */1,
/* 5 */2,
/* 6 */2,
/* 7 */3,
/* 8 */1,
/* 9 */2,
/* a */2,
/* b */3,
/* c */2,
/* d */3,
/* e */3,
/* f */4 };

uint8_t POPCOUNT_SHD[32] __aligned__ = {
/* 0 */0,
/* 1 */1,
/* 2 */1,
/* 3 */1,
/* 4 */1,
/* 5 */2,
/* 6 */2,
/* 7 */1,
/* 8 */1,
/* 9 */2,
/* a */2,
/* b */2,
/* c */1,
/* d */2,
/* e */1,
/* f */1,
/* 0 */0,
/* 1 */1,
/* 2 */1,
/* 3 */1,
/* 4 */1,
/* 5 */2,
/* 6 */2,
/* 7 */1,
/* 8 */1,
/* 9 */2,
/* a */2,
/* b */2,
/* c */1,
/* d */2,
/* e */1,
/* f */1 };


uint8_t __MASK_0F_[32] __aligned__ = {
			0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf,
			0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 
			0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf,
			0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf
			};

uint32_t ssse3_popcount_m256_core(__m256i reg, uint8_t *map) {

	uint32_t result;

	__m256i clear_mask = _mm256_loadu_si256( (__m256i*) __MASK_0F_);
	__m256i count_mask = _mm256_loadu_si256( (__m256i*) map);

	__m256i upper_bits = _mm256_srli_epi16(reg, 4);
	upper_bits = _mm256_and_si256(upper_bits, clear_mask);
	__m256i lower_bits = _mm256_and_si256(reg, clear_mask);

	__m256i lower_sum = _mm256_shuffle_epi8(count_mask, lower_bits);
	__m256i upper_sum = _mm256_shuffle_epi8(count_mask, upper_bits);

	__m256i zero_vec = _mm256_setzero_si256();

	__m256i packed_sum_8 = _mm256_add_epi8(lower_sum, upper_sum);
	__m256i packed_sum_64 = _mm256_sad_epu8(packed_sum_8, zero_vec);

	uint64_t packed_sum[4];
	_mm256_store_si256((__m256i*)packed_sum, packed_sum_64);
	result = packed_sum[0] + packed_sum[1] + packed_sum[2] + packed_sum[3];

	return result;

/*
	__m256i shuffled_packed_sum_64 = _mm256_permute4x64_epi64(packed_sum_64, 15);
	__m256i packed_sum_128 = _mm256_add_epi64(packed_sum_64, shuffled_packed_sum_64);
	__m256i shuffled_packed_sum_128 = _mm256_permute4x64_epi64(packed_sum_128, 2);
	__m256i total_sum = _mm256_add_epi64(packed_sum_128, shuffled_packed_sum_128);

	result = _mm256_extract_epi32(total_sum, 0);
	*/
}

uint32_t ssse3_popcount_m128_core(__m128i reg, uint8_t *map) {

	uint32_t result;

	__m128i clear_mask = _mm_loadu_si128( (__m128i*) __MASK_0F_);
	__m128i count_mask = _mm_loadu_si128( (__m128i*) map);

	__m128i upper_bits = _mm_srli_epi16(reg, 4);
	upper_bits = _mm_and_si128(upper_bits, clear_mask);
	__m128i lower_bits = _mm_and_si128(reg, clear_mask);

	__m128i lower_sum = _mm_shuffle_epi8(count_mask, lower_bits);
	__m128i upper_sum = _mm_shuffle_epi8(count_mask, upper_bits);

	__m128i zero_vec = _mm_setzero_si128();

	__m128i packed_sum_8 = _mm_add_epi8(lower_sum, upper_sum);
	__m128i packed_sum_64 = _mm_sad_epu8(packed_sum_8, zero_vec);

	uint64_t packed_sum[2];
	_mm_store_si128((__m128i*)packed_sum, packed_sum_64);
	result = packed_sum[0] + packed_sum[1];

	return result;
/*	
	printf("reg: \n");
	print128_bit(reg);
	printf("upper_bits: \n");
	print128_bit(upper_bits);
	printf("lower_bits: \n");
	print128_bit(lower_bits);

	printf("upper_sum: \n");
	print128_hex(upper_sum);
	printf("lower_sum: \n");
	print128_hex(lower_sum);

	printf("packed_sum: ");
	print128_hex(packed_sum);
	printf("partial_sum: ");
	print128_hex(packed_sum);
	printf("shuffled_partial_sum: ");
	print128_hex(packed_sum);


/*
	__asm__ volatile ("movdqu (%%rax), %%xmm7" : : "a" (map));
	__asm__ volatile ("movdqu (%%rax), %%xmm6" : : "a" (MASK_0F));
	// xmm5 -- global accumulator

	result = 0;

	__asm__ volatile ("pxor %xmm4, %xmm4");
	// xmm4 -- local accumulator

	__asm__ volatile(
			"movdqa        %1, %%xmm0	\n"
			"movdqa    %%xmm0, %%xmm1	\n"

			"psrlw         $4, %%xmm1	\n"
			"pand      %%xmm6, %%xmm0	\n"// xmm0 := lower nibbles
			"pand      %%xmm6, %%xmm1	\n"// xmm1 := higher nibbles

			"movdqa    %%xmm7, %%xmm2	\n"
			"movdqa    %%xmm7, %%xmm3	\n"// get popcount
			"pshufb    %%xmm0, %%xmm2	\n"// for all nibbles
			"pshufb    %%xmm1, %%xmm3	\n"// using PSHUFB

			"paddb     %%xmm2, %%xmm4	\n"// update local
			"paddb     %%xmm3, %%xmm4	\n"// accumulator

			"pxor	%%xmm0, %%xmm0		\n"
			"psadbw	%%xmm0, %%xmm4		\n"
			"movhlps   %%xmm4, %%xmm0	\n"
			"paddd     %%xmm4, %%xmm0	\n"
			"movd   %%xmm0, %%rax		\n"
			: "=a" (result)
			: "x" (reg)
	);

	return result;
	*/
}

uint32_t popcount_m128i_sse(__m128i reg) {
	return ssse3_popcount_m128_core(reg, POPCOUNT);
}

uint32_t popcount_m256i_sse(__m256i reg) {
	return ssse3_popcount_m256_core(reg, POPCOUNT);
}


uint32_t builtin_popcount(uint8_t* buffer, int chunks16) {
	uint32_t result = 0;

	int i;
	for (i = 0; i < chunks16; i++) {
		uint64_t *cast_int64 = (uint64_t*) (buffer + i * 16);
		result += _mm_popcnt_u64(*cast_int64);
		cast_int64 = (uint64_t*) (buffer + i * 16 + 8);
		result += _mm_popcnt_u64(*cast_int64);
	}

	return result;
}

uint32_t popcount(uint8_t *buffer, int chunks16) {
	uint32_t result = 0;

	int i;
	for (i = 0; i < chunks16 * 16; i++) {
		int j;
		uint8_t mask = 1;
		for (j = 0; j < 8; j++) {
			if (buffer[i] & mask)
				result++;
			mask = mask << 1;
		}
	}

	return result;
}

