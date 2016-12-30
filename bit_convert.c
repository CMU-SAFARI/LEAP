/*
 * bit_convert.c
 *
 *  Created on: Nov 7, 2013
 *      Author: hxin
 */

#include "print.h"
#include "bit_convert.h"
#include <stdio.h>
#include <x86intrin.h>

// This is here only because gcc lacks some intrinsics!!
__m256i _mm256_loadu2_m128i(__m128i* hi, __m128i* lo) {
	return _mm256_inserti128_si256(_mm256_castsi128_si256(*lo), *hi, 1);
}

void _mm256_storeu2_m128i(__m128i* hi, __m128i* lo, __m256i target) {
	*hi = _mm256_extracti128_si256(target, 1);
	*lo = _mm256_extracti128_si256(target, 0);
}
// This is here only because gcc lacks some intrinsics!!

char MASK_A[32] __aligned__ = { 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
		'A', 'A', 'A', 'A', 'A', 'A', 'A' };

char MASK_C[32] __aligned__ = { 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C',
		'C', 'C', 'C', 'C', 'C', 'C', 'C' };

char MASK_G[32] __aligned__ = { 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G',
		'G', 'G', 'G', 'G', 'G', 'G', 'G' };

char MASK_T[32] __aligned__ = {
	'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T',
		'T', 'T', 'T', 'T', 'T', 'T', 'T' };

void c_convert2bit(char *str, int length, uint8_t *bits) {
	int i;
	int j;
	int k;

	for (j = 0; j < length * 2 / (8 * sizeof(bits[0])) + 1; j++)
		bits[j] = 0;

	for (i = 0; i < length; i++) {
		j = i * 2 / (sizeof(bits[0]) * 8);
		k = i * 2 % (sizeof(bits[0]) * 8);

		switch (str[i]) {
		case 'C':
			bits[j] += 1ULL << k;
			break;
		case 'G':
			bits[j] += 2ULL << k;
			break;
		case 'T':
			bits[j] += 3ULL << k;
			break;
		default:
			break;
		}
		/*
		 int m;
		 for (m = 63; m >= 0; m--) {
		 //			cout << "m:" << m << " ";
		 if (temp & (1ULL << m) )
		 printf("1");
		 else
		 printf("0");
		 }
		 */
	}
}

uint8_t BIT_FF[32] __aligned__ = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	};

uint8_t BASE_SHIFT1[32] __aligned__ = {
	0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15,
	0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15
	};

uint8_t BASE_SHIFT2[32] __aligned__ = {
	0, 1, 8, 9, 4, 5, 12, 13, 2, 3, 10, 11, 6, 7, 14, 15,
	0, 1, 8, 9, 4, 5, 12, 13, 2, 3, 10, 11, 6, 7, 14, 15 };

//Have to consider Intel's endians
uint8_t LOC_MASK_SSE[128] = { 0x01, 0x01, 0x01, 0x01, //1
		0x01, 0x01, 0x01, 0x01, //1
		0x01, 0x01, 0x01, 0x01, //1
		0x01, 0x01, 0x01, 0x01, //1
		0x04, 0x04, 0x04, 0x04, //3
		0x04, 0x04, 0x04, 0x04, //3
		0x04, 0x04, 0x04, 0x04, //3
		0x04, 0x04, 0x04, 0x04, //3
		0x02, 0x02, 0x02, 0x02, //2
		0x02, 0x02, 0x02, 0x02, //2
		0x02, 0x02, 0x02, 0x02, //2
		0x02, 0x02, 0x02, 0x02, //2
		0x08, 0x08, 0x08, 0x08, //4
		0x08, 0x08, 0x08, 0x08, //4
		0x08, 0x08, 0x08, 0x08, //4
		0x08, 0x08, 0x08, 0x08, //4
		0x10, 0x10, 0x10, 0x10, //5
		0x10, 0x10, 0x10, 0x10, //5
		0x10, 0x10, 0x10, 0x10, //5
		0x10, 0x10, 0x10, 0x10, //5
		0x40, 0x40, 0x40, 0x40, //7
		0x40, 0x40, 0x40, 0x40, //7
		0x40, 0x40, 0x40, 0x40, //7
		0x40, 0x40, 0x40, 0x40, //7
		0x20, 0x20, 0x20, 0x20, //6
		0x20, 0x20, 0x20, 0x20, //6
		0x20, 0x20, 0x20, 0x20, //6
		0x20, 0x20, 0x20, 0x20, //6
		0x80, 0x80, 0x80, 0x80, //8
		0x80, 0x80, 0x80, 0x80, //8
		0x80, 0x80, 0x80, 0x80, //8
		0x80, 0x80, 0x80, 0x80 //8
		};

uint8_t LOC_MASK_AVX[256] = {
		0x01, 0x01, 0x01, 0x01, //1
		0x01, 0x01, 0x01, 0x01, //1
		0x01, 0x01, 0x01, 0x01, //1
		0x01, 0x01, 0x01, 0x01, //1
		0x01, 0x01, 0x01, 0x01, //1
		0x01, 0x01, 0x01, 0x01, //1
		0x01, 0x01, 0x01, 0x01, //1
		0x01, 0x01, 0x01, 0x01, //1
		0x02, 0x02, 0x02, 0x02, //2
		0x02, 0x02, 0x02, 0x02, //2
		0x02, 0x02, 0x02, 0x02, //2
		0x02, 0x02, 0x02, 0x02, //2
		0x02, 0x02, 0x02, 0x02, //2
		0x02, 0x02, 0x02, 0x02, //2
		0x02, 0x02, 0x02, 0x02, //2
		0x02, 0x02, 0x02, 0x02, //2
		0x10, 0x10, 0x10, 0x10, //5
		0x10, 0x10, 0x10, 0x10, //5
		0x10, 0x10, 0x10, 0x10, //5
		0x10, 0x10, 0x10, 0x10, //5
		0x10, 0x10, 0x10, 0x10, //5
		0x10, 0x10, 0x10, 0x10, //5
		0x10, 0x10, 0x10, 0x10, //5
		0x10, 0x10, 0x10, 0x10, //5
		0x20, 0x20, 0x20, 0x20, //6
		0x20, 0x20, 0x20, 0x20, //6
		0x20, 0x20, 0x20, 0x20, //6
		0x20, 0x20, 0x20, 0x20, //6
		0x20, 0x20, 0x20, 0x20, //6
		0x20, 0x20, 0x20, 0x20, //6
		0x20, 0x20, 0x20, 0x20, //6
		0x20, 0x20, 0x20, 0x20, //6
		0x04, 0x04, 0x04, 0x04, //3
		0x04, 0x04, 0x04, 0x04, //3
		0x04, 0x04, 0x04, 0x04, //3
		0x04, 0x04, 0x04, 0x04, //3
		0x04, 0x04, 0x04, 0x04, //3
		0x04, 0x04, 0x04, 0x04, //3
		0x04, 0x04, 0x04, 0x04, //3
		0x04, 0x04, 0x04, 0x04, //3
		0x08, 0x08, 0x08, 0x08, //4
		0x08, 0x08, 0x08, 0x08, //4
		0x08, 0x08, 0x08, 0x08, //4
		0x08, 0x08, 0x08, 0x08, //4
		0x08, 0x08, 0x08, 0x08, //4
		0x08, 0x08, 0x08, 0x08, //4
		0x08, 0x08, 0x08, 0x08, //4
		0x08, 0x08, 0x08, 0x08, //4
		0x40, 0x40, 0x40, 0x40, //7
		0x40, 0x40, 0x40, 0x40, //7
		0x40, 0x40, 0x40, 0x40, //7
		0x40, 0x40, 0x40, 0x40, //7
		0x40, 0x40, 0x40, 0x40, //7
		0x40, 0x40, 0x40, 0x40, //7
		0x40, 0x40, 0x40, 0x40, //7
		0x40, 0x40, 0x40, 0x40, //7
		0x80, 0x80, 0x80, 0x80, //8
		0x80, 0x80, 0x80, 0x80, //8
		0x80, 0x80, 0x80, 0x80, //8
		0x80, 0x80, 0x80, 0x80, //8
		0x80, 0x80, 0x80, 0x80, //8
		0x80, 0x80, 0x80, 0x80, //8
		0x80, 0x80, 0x80, 0x80, //8
		0x80, 0x80, 0x80, 0x80  //8
		};

void sse_convert2bit(char *str, uint8_t *bits0, uint8_t *bits1) {

	__m128i *shift_hint = (__m128i *) BASE_SHIFT1;
	__m128i *cast_str;

	__m128i temp;
	__m128i result0, result1;

	//Mask for bit0 and bit1
	__m128i *maskA = (__m128i *) MASK_A;
	__m128i *maskC = (__m128i *) MASK_C;
	__m128i *maskG = (__m128i *) MASK_G;
	__m128i *maskT = (__m128i *) MASK_T;
	__m128i *mask;

	int i;

	for (i = 0; i < 128; i += 16) {
		cast_str = (__m128i *) (str + i);
		*cast_str = _mm_shuffle_epi8(*cast_str, *shift_hint);
	}

//	printf("After shifting 0: %s\n", str);

	for (i = 8; i < 128; i += 32) {
		cast_str = (__m128i *) (str + i);
		temp = _mm_loadu_si128(cast_str);
		temp = _mm_shuffle_epi32(temp, 0x4e);
		_mm_storeu_si128(cast_str, temp);
	}

//	printf("After shifting 1: %s\n", str);

	shift_hint = (__m128i *) BASE_SHIFT2;
	for (i = 0; i < 128; i += 16) {
		cast_str = (__m128i *) (str + i);
		*cast_str = _mm_shuffle_epi8(*cast_str, *shift_hint);
	}

//	printf("After shifting 2: %s\n", str);

	for (i = 16; i < 128; i += 64) {
		temp = *((__m128i *) (str + i));
		*((__m128i *) (str + i)) = *((__m128i *) (str + i + 16));
		*((__m128i *) (str + i + 16)) = temp;
	}

//	printf("After shifting 3: %s\n", str);

	for (i = 8; i < 128; i += 32) {
		cast_str = (__m128i *) (str + i);
		temp = _mm_loadu_si128(cast_str);
		temp = _mm_shuffle_epi32(temp, 0x4e);
		_mm_storeu_si128(cast_str, temp);
	}

//	printf("After shifting 4: %s\n", str);

	for (i = 0; i < 128; i += 16) {
		cast_str = (__m128i *) (str + i);
		temp = _mm_load_si128(cast_str);
		temp = _mm_shuffle_epi32(temp, 0xd8);
		_mm_store_si128(cast_str, temp);
	}

//	printf("After shifting 5: %s\n", str);

	for (i = 16; i < 64; i += 32) {
		temp = *((__m128i *) (str + i));
		*((__m128i *) (str + i)) = *((__m128i *) (str + i + 48));
		*((__m128i *) (str + i + 48)) = temp;
	}

//	printf("After shifting 6: %s\n", str);

	for (i = 8; i < 128; i += 32) {
		cast_str = (__m128i *) (str + i);
		temp = _mm_loadu_si128(cast_str);
		temp = _mm_shuffle_epi32(temp, 0x4e);
		_mm_storeu_si128(cast_str, temp);
	}

//	printf("After shifting 7: %s\n", str);

	result0 = _mm_set1_epi32(0);
	result1 = _mm_set1_epi32(0);
	__m128i* bit0_reg = (__m128i*) bits0;
	__m128i* bit1_reg = (__m128i*) bits1;

	*bit0_reg = _mm_set1_epi32(0);
	*bit1_reg = _mm_set1_epi32(0);

	for (i = 0; i < 128; i += 16) {
		cast_str = (__m128i *) (str + i);
		temp = _mm_cmpeq_epi8(*maskC, *cast_str);
		result0 = _mm_and_si128(temp, *((__m128i *) BIT_FF)); //C=01
		//print128_hex(result);

		temp = _mm_cmpeq_epi8(*maskG, *cast_str);
		temp = _mm_and_si128(temp, *((__m128i *) BIT_FF));
		result1 = _mm_or_si128(result1, temp); //G=10
		//print128_hex(result);

		temp = _mm_cmpeq_epi8(*maskT, *cast_str);
		temp = _mm_and_si128(temp, *((__m128i *) BIT_FF)); //T=11
		result0 = _mm_or_si128(result0, temp); //T=11
		result1 = _mm_or_si128(result1, temp); //T=11
		//print128_hex(result);

		mask = (__m128i *) (LOC_MASK_SSE + i);

		result0 = _mm_and_si128(*mask, result0);
		result1 = _mm_and_si128(*mask, result1);
		//print128_hex(result);
		*bit0_reg = _mm_or_si128(*bit0_reg, result0);
		*bit1_reg = _mm_or_si128(*bit1_reg, result1);
		//print128_bit(result);
//		print128_bit(*bit0_reg);
//		print128_bit(*bit1_reg);

	}
}

void avx_convert2bit(char *str, uint8_t *bits0, uint8_t *bits1) {

	__m256i *shift_hint = (__m256i *) BASE_SHIFT1;
	__m256i *cast_str;
	__m128i *cast_128_1, *cast_128_2;

	__m256i temp;
	__m256i result0, result1;

	//Mask for bit0 and bit1
	__m256i *maskA = (__m256i *) MASK_A;
	__m256i *maskC = (__m256i *) MASK_C;
	__m256i *maskG = (__m256i *) MASK_G;
	__m256i *maskT = (__m256i *) MASK_T;
	__m256i *mask;

	int i;

	for (i = 0; i < 256; i += 32) {
		cast_str = (__m256i *) (str + i);
		*cast_str = _mm256_shuffle_epi8(*cast_str, *shift_hint);
	}

//	printf("After shifting 0: %s\n", str);

	for (i = 8; i < 256; i += 64) {
		cast_128_1 = (__m128i *) (str + i);
		cast_128_2 = (__m128i *) (str + i + 32);
		temp = _mm256_loadu2_m128i(cast_128_1, cast_128_2);
		temp = _mm256_shuffle_epi32(temp, 0x4e);
		_mm256_storeu2_m128i(cast_128_1, cast_128_2, temp);
	}

//	printf("After shifting 1: %s\n", str);

	shift_hint = (__m256i *) BASE_SHIFT2;
	for (i = 0; i < 256; i += 32) {
		cast_str = (__m256i *) (str + i);
		*cast_str = _mm256_shuffle_epi8(*cast_str, *shift_hint);
	}

//	printf("After shifting 2: %s\n", str);

	for (i = 16; i < 256; i += 64) {
		cast_128_1 = (__m128i *) (str + i);
		cast_128_2 = (__m128i *) (str + i + 16);
		temp = _mm256_loadu2_m128i(cast_128_1, cast_128_2);
		_mm256_storeu2_m128i(cast_128_2, cast_128_1, temp);
	}

//	printf("After shifting 3: %s\n", str);

	for (i = 8; i < 256; i += 64) {
		cast_128_1 = (__m128i *) (str + i);
		cast_128_2 = (__m128i *) (str + i + 32);
		temp = _mm256_loadu2_m128i(cast_128_1, cast_128_2);
		temp = _mm256_shuffle_epi32(temp, 0x4e);
		_mm256_storeu2_m128i(cast_128_1, cast_128_2, temp);
	}

//	printf("After shifting 4: %s\n", str);

	for (i = 0; i < 256; i += 32) {
		cast_str = (__m256i *) (str + i);
		temp = _mm256_load_si256(cast_str);
		temp = _mm256_shuffle_epi32(temp, 0xd8);
		_mm256_store_si256(cast_str, temp);
	}

//	printf("After shifting 5: %s\n", str);

	for (i = 16; i < 64; i += 32) {
		cast_128_1 = (__m128i *) (str + i);
		cast_128_2 = (__m128i *) (str + i + 48);
		temp = _mm256_loadu2_m128i(cast_128_1, cast_128_2);
		_mm256_storeu2_m128i(cast_128_2, cast_128_1, temp);

		cast_128_1 = (__m128i *) (str + 128 + i);
		cast_128_2 = (__m128i *) (str + 128 + i + 48);
		temp = _mm256_loadu2_m128i(cast_128_1, cast_128_2);
		_mm256_storeu2_m128i(cast_128_2, cast_128_1, temp);
	}

//	printf("After shifting 6: %s\n", str);

	for (i = 8; i < 256; i += 64) {
		cast_128_1 = (__m128i *) (str + i);
		cast_128_2 = (__m128i *) (str + i + 32);
		temp = _mm256_loadu2_m128i(cast_128_1, cast_128_2);
		temp = _mm256_shuffle_epi32(temp, 0x4e);
		_mm256_storeu2_m128i(cast_128_1, cast_128_2, temp);
	}

//	printf("After shifting 7: %s\n", str);

	for (i = 16; i < 128; i += 32) {
		cast_128_1 = (__m128i *) (str + i);
		cast_128_2 = (__m128i *) (str + i + 112);
		temp = _mm256_loadu2_m128i(cast_128_1, cast_128_2);
		_mm256_storeu2_m128i(cast_128_2, cast_128_1, temp);
	}


//	printf("After shifting 8: %s\n", str);

	result0 = _mm256_setzero_si256();
	result1 = _mm256_setzero_si256();
	__m256i* bit0_reg = (__m256i*) bits0;
	__m256i* bit1_reg = (__m256i*) bits1;

	*bit0_reg = _mm256_setzero_si256();
	*bit1_reg = _mm256_setzero_si256();

	for (i = 0; i < 256; i += 16) {
		cast_str = (__m256i *) (str + i);
		temp = _mm256_cmpeq_epi8(*maskC, *cast_str);
		result0 = _mm256_and_si256(temp, *((__m256i *) BIT_FF)); //C=01
		//print256_hex(result);

		temp = _mm256_cmpeq_epi8(*maskG, *cast_str);
		temp = _mm256_and_si256(temp, *((__m256i *) BIT_FF)); //G=10
		result1 = _mm256_or_si256(result1, temp); //G=10
		//print256_hex(result);

		temp = _mm256_cmpeq_epi8(*maskT, *cast_str);
		temp = _mm256_and_si256(temp, *((__m256i *) BIT_FF)); //T=11
		result0 = _mm256_or_si256(result0, temp); //T=11
		result1 = _mm256_or_si256(result1, temp); //T=11
		//print256_hex(result);

		mask = (__m256i *) (LOC_MASK_AVX + i);

		result0 = _mm256_and_si256(*mask, result0);
		result1 = _mm256_and_si256(*mask, result1);
		//print256_hex(result);
		*bit0_reg = _mm256_or_si256(*bit0_reg, result0);
		*bit1_reg = _mm256_or_si256(*bit1_reg, result1);
		//print256_bit(result);
//		print256_bit(*bit0_reg);
//		print256_bit(*bit1_reg);

	}
}

