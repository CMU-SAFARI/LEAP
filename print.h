/*
 * print.h
 *
 *  Created on: Nov 12, 2013
 *      Author: hxin
 */

#ifndef PRINT_H_
#define PRINT_H_
#include <stdint.h>
#include <x86intrin.h>

void printbytevector(uint8_t *data, int length);
void print128_bit(__m128i var);
void print256_bit(__m256i var);
void print128_hex(__m128i var);

#endif /* PRINT_H_ */
