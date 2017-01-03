/*
 * vector_ed.c
 *
 *  Created on: Nov 8, 2013
 *      Author: hxin
 */

#include "print.h"
#include "SHD.h"
#include <stdio.h>
#include <string.h>
#include "popcount.h"
#include "bit_convert.h"
#include "mask.h"
#include "shift.h"

using namespace std;

/*
 * By little endians, left shift should actually be right shift in x86 convention
 */

void flip_false_zero(__m128i& vec) {

//	printf("vec: \t\t");
//	print128_bit(vec);

	//Due to the special effect of shuffle_epi8, we have to erase the high order bit.
	char temp_char = (char) 0x7f;
	__m128i boundary= _mm_set1_epi8(0x7f);
//	printf("MASK_7F: \t");
//	print128_bit(*boundary);

	__m128i shift = _mm_and_si128(boundary, vec);
	
//	printf("After and: \t");
//	print128_bit(shift);

	__m128i *mask = (__m128i *) MASK_SRS;

	shift = _mm_shuffle_epi8(*mask, shift);
	vec = _mm_or_si128(vec, shift);
	
//	printf("Last cases %d: \t", 0);
//	print128_bit(vec);

	int i;
	for (i = 1; i < 4; i++) {
		shift = _mm_srli_epi16(vec, i);
		shift = _mm_and_si128(boundary, shift);
//		printf("shift %d: \t", i);
//		print128_bit(shift);
		shift = _mm_shuffle_epi8(*mask, shift);
//		printf("shuffle %d: \t", i);
//		print128_bit(shift);
		shift = _mm_slli_epi16(shift, i);
		vec = _mm_or_si128(vec, shift);
//		printf("Last cases %d: \t", i);
//		print128_bit(vec);
	}

	//For the crossing bits
	__m128i shifted_vec = shift_right_sse(vec, 4);
//	printf("shifted_vec: \t");
//	print128_bit(shifted_vec);

	shift = _mm_and_si128(boundary, shifted_vec);
//	printf("After and: \t");
//	print128_bit(shift);
	
	shift = _mm_shuffle_epi8(*mask, shift);
	shifted_vec = _mm_or_si128(shifted_vec, shift);
//	printf("Cross cases %d: \t", 0);
//	print128_bit(shifted_vec);

	for (i = 1; i < 4; i++) {
		shift = _mm_srli_epi16(shifted_vec, i);
		shift = _mm_and_si128(boundary, shift);
		shift = _mm_shuffle_epi8(*mask, shift);
		shift = _mm_slli_epi16(shift, i);
		shifted_vec = _mm_or_si128(shifted_vec, shift);
//		printf("Cross cases %d: \t", i);
//		print128_bit(shifted_vec);
	}

	shifted_vec = shift_left_sse(shifted_vec, 4);
	vec = _mm_or_si128(shifted_vec, vec);
//	printf("Final case: \t");
//	print128_bit(vec);
}

void flip_false_zero(__m256i& vec) {

//	printf("vec: \t\t");
//	print128_bit(vec);

	//Due to the special effect of shuffle_epi8, we have to erase the high order bit.
	__m256i boundary= _mm256_set1_epi8(0x7f);
//	printf("MASK_7F: \t");
//	print128_bit(*boundary);

	__m256i shift = _mm256_and_si256(boundary, vec);
	
//	printf("After and: \t");
//	print128_bit(shift);

	__m256i *mask = (__m256i *) MASK_SRS;

	shift = _mm256_shuffle_epi8(*mask, shift);
	vec = _mm256_or_si256(vec, shift);
	
//	printf("Last cases %d: \t", 0);
//	print128_bit(vec);

	int i;
	for (i = 1; i < 4; i++) {
		shift = _mm256_srli_epi16(vec, i);
		shift = _mm256_and_si256(boundary, shift);
//		printf("shift %d: \t", i);
//		print128_bit(shift);
		shift = _mm256_shuffle_epi8(*mask, shift);
//		printf("shuffle %d: \t", i);
//		print128_bit(shift);
		shift = _mm256_slli_epi16(shift, i);
		vec = _mm256_or_si256(vec, shift);
//		printf("Last cases %d: \t", i);
//		print128_bit(vec);
	}

	__m256i shifted_vec = shift_right_avx(vec, 4);
//	printf("shifted_vec: \t");
//	print128_bit(shifted_vec);

	shift = _mm256_and_si256(boundary, shifted_vec);
//	printf("After and: \t");
//	print128_bit(shift);
	
	shift = _mm256_shuffle_epi8(*mask, shift);
	shifted_vec = _mm256_or_si256(shifted_vec, shift);
//	printf("Cross cases %d: \t", 0);
//	print128_bit(shifted_vec);

	for (i = 1; i < 4; i++) {
		shift = _mm256_srli_epi16(shifted_vec, i);
		shift = _mm256_and_si256(boundary, shift);
		shift = _mm256_shuffle_epi8(*mask, shift);
		shift = _mm256_slli_epi16(shift, i);
		shifted_vec = _mm256_or_si256(shifted_vec, shift);
//		printf("Cross cases %d: \t", i);
//		print128_bit(shifted_vec);
	}

	shifted_vec = shift_left_avx(shifted_vec, 4);
	vec = _mm256_or_si256(shifted_vec, vec);
//	printf("Final case: \t");
//	print128_bit(vec);
}

int bit_vec_filter_sse(__m128i read_XMM0, __m128i read_XMM1,
		__m128i ref_XMM0, __m128i ref_XMM1, int length, int max_error) {
	
	__m128i mask;
	if (length >= SHD_SSE_LENGTH)
		mask = _mm_set1_epi8(0xff);
	else
		mask = _mm_load_si128( (__m128i *) (MASK_SSE_END + (length *
										SSE_BYTE_NUM)));

	int total_difference = 0;

	//Start iteration
	int j;

	__m128i shift_XMM;
	__m128i diff_XMM;
	__m128i temp_diff_XMM;
	__m128i temp_shift_XMM;
	__m128i temp_mask;

	diff_XMM = _mm_xor_si128(read_XMM0, ref_XMM0);
	temp_diff_XMM = _mm_xor_si128(read_XMM1, ref_XMM1);
	diff_XMM = _mm_or_si128(diff_XMM, temp_diff_XMM);

	flip_false_zero(diff_XMM);

//	printf("diff_XMM: \t");
//	print128_bit(diff_XMM);

	for (j = 1; j <= max_error; j++) {
		temp_mask = _mm_load_si128( (__m128i *) (MASK_SSE_BEG + (j - 1) *
								SSE_BYTE_NUM));
		temp_mask = _mm_and_si128(temp_mask, mask);
		
		//Right shift read
		shift_XMM = shift_right_sse(read_XMM0, j);
		temp_diff_XMM = _mm_xor_si128(shift_XMM, ref_XMM0);
		shift_XMM = shift_right_sse(read_XMM1, j);
		temp_shift_XMM = _mm_xor_si128(shift_XMM, ref_XMM1);
		temp_diff_XMM = _mm_or_si128(temp_shift_XMM, temp_diff_XMM);
		temp_diff_XMM = _mm_and_si128(temp_diff_XMM, temp_mask);
//		printf("Before flip: \t");
//		print128_bit(temp_diff_XMM);
		flip_false_zero(temp_diff_XMM);
//		printf("After flip: \t");
//		print128_bit(temp_diff_XMM);
		diff_XMM = _mm_and_si128(diff_XMM, temp_diff_XMM);

//		printf("diff_XMM: \t");
//		print128_bit(diff_XMM);
//Right shift ref
		shift_XMM = shift_right_sse(ref_XMM0, j);
		temp_diff_XMM = _mm_xor_si128(shift_XMM, read_XMM0);
		shift_XMM = shift_right_sse(ref_XMM1, j);
		temp_shift_XMM = _mm_xor_si128(shift_XMM, read_XMM1);
		temp_diff_XMM = _mm_or_si128(temp_shift_XMM, temp_diff_XMM);
		temp_diff_XMM = _mm_and_si128(temp_diff_XMM, temp_mask);
//		printf("Before flip: \t");
//		print128_bit(temp_diff_XMM);
		flip_false_zero(temp_diff_XMM);
//		printf("After flip: \t");
//		print128_bit(temp_diff_XMM);
		diff_XMM = _mm_and_si128(diff_XMM, temp_diff_XMM);
		
//		printf("diff_XMM: \t");
//		print128_bit(diff_XMM);
	}

	total_difference = popcount_SHD_sse(diff_XMM);

//	printf("total_difference: %d\n", total_difference);

	if (total_difference > (max_error) )
		return 0;
	else
		return 1;
}

int bit_vec_filter_avx(__m256i read_XMM0, __m256i read_XMM1,
		__m256i ref_XMM0, __m256i ref_XMM1, int length, int max_error) {
	
	__m256i mask;
	if (length >= SHD_AVX_LENGTH)
		mask = _mm256_set1_epi8(0xff);
	else
		mask = _mm256_load_si256( (__m256i *) (MASK_AVX_END + (length *
										AVX_BYTE_NUM)));

	int total_difference = 0;

	//Start iteration
	int j;

	__m256i shift_XMM;
	__m256i diff_XMM;
	__m256i temp_diff_XMM;
	__m256i temp_shift_XMM;
	__m256i temp_mask;

	diff_XMM = _mm256_xor_si256(read_XMM0, ref_XMM0);
	temp_diff_XMM = _mm256_xor_si256(read_XMM1, ref_XMM1);
	diff_XMM = _mm256_or_si256(diff_XMM, temp_diff_XMM);

	flip_false_zero(diff_XMM);

//	printf("diff_XMM: \t");
//	print128_bit(diff_XMM);

	for (j = 1; j <= max_error; j++) {
		temp_mask = _mm256_load_si256( (__m256i *) (MASK_AVX_BEG + (j - 1) *
								AVX_BYTE_NUM));
		temp_mask = _mm256_and_si256(temp_mask, mask);
		
		//Right shift read
		shift_XMM = shift_right_avx(read_XMM0, j);
		temp_diff_XMM = _mm256_xor_si256(shift_XMM, ref_XMM0);
		shift_XMM = shift_right_avx(read_XMM1, j);
		temp_shift_XMM = _mm256_xor_si256(shift_XMM, ref_XMM1);
		temp_diff_XMM = _mm256_or_si256(temp_shift_XMM, temp_diff_XMM);
		temp_diff_XMM = _mm256_and_si256(temp_diff_XMM, temp_mask);
//		printf("Before flip: \t");
//		print128_bit(temp_diff_XMM);
		flip_false_zero(temp_diff_XMM);
//		printf("After flip: \t");
//		print128_bit(temp_diff_XMM);
		diff_XMM = _mm256_and_si256(diff_XMM, temp_diff_XMM);

//		printf("diff_XMM: \t");
//		print128_bit(diff_XMM);

		//Right shift ref
		shift_XMM = shift_right_avx(ref_XMM0, j);
		temp_diff_XMM = _mm256_xor_si256(shift_XMM, read_XMM0);
		shift_XMM = shift_right_avx(ref_XMM1, j);
		temp_shift_XMM = _mm256_xor_si256(shift_XMM, read_XMM1);
		temp_diff_XMM = _mm256_or_si256(temp_shift_XMM, temp_diff_XMM);
		temp_diff_XMM = _mm256_and_si256(temp_diff_XMM, temp_mask);
//		printf("Before flip: \t");
//		print128_bit(temp_diff_XMM);
		flip_false_zero(temp_diff_XMM);
//		printf("After flip: \t");
//		print128_bit(temp_diff_XMM);
		diff_XMM = _mm256_and_si256(diff_XMM, temp_diff_XMM);
		
//		printf("diff_XMM: \t");
//		print128_bit(diff_XMM);
	}

	total_difference = popcount_SHD_avx(diff_XMM);

//	printf("total_difference: %d\n", total_difference);

	if (total_difference > (max_error) )
		return 0;
	else
		return 1;
}
