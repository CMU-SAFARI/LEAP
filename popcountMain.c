/*
 * popcountMain.c
 *
 *  Created on: Nov 7, 2013
 *      Author: hxin
 */

#include "popcount.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_CHUNKS 2048

uint8_t buffer[16 * MAX_CHUNKS] __aligned__;

#define OPT_COUNT 5

char* functions[OPT_COUNT] = { "verify", "popcount", 
		"builtin_popcount", 
		"popcount_m128i_sse",
		"popcount_m256i_sse" };

void help(const char* progname) {
	int i;
	printf("%s ", progname);

	printf("%s", functions[0]);
	for (i = 1; i < OPT_COUNT; i++)
		printf("|%s", functions[i]);

	printf(" 16-byte-chunks repeat-count\n");
	exit(1);
}

#define VERIFY(index) \
			printf("%15s -> %d %s\n", \
			functions[index], \
			popcount, \
			(popcount_ref != popcount) ? "FAILED!!!" : "" \
		); \
		if (popcount_ref != popcount) \
			failed = 1;

char verify(int index, uint32_t (*func)(uint8_t*, int), uint8_t *buffer,
		int chunks16, unsigned popcount_ref) {
	char failed;
	int popcount = (*func)(buffer, chunks16);
	VERIFY(index);
	return failed;
}

char verify_m128i(int index, uint32_t (*func)(__m128i), uint8_t *buffer,
		int chunks16, unsigned popcount_ref) {
	char failed;
	int popcount = 0;
	__m128i reg;
	int i;
	for (i = 0; i < chunks16; i++) {
		reg = *((__m128i *) (buffer + i * 16));
		popcount += (*func)(reg);
	}
	VERIFY(index);
	return failed;
}

char verify_m256i(int index, uint32_t (*func)(__m256i), uint8_t *buffer,
		int chunks16, unsigned popcount_ref) {
	char failed;
	int popcount = 0;
	__m256i reg;
	int i;
	for (i = 0; i < chunks16 / 2; i++) {
		reg = *((__m256i *) (buffer + i * 32));
		popcount += (*func)(reg);
	}
	VERIFY(index);
	return failed;
}

int main(int argc, char* argv[]) {
	// prog parametrs
	int result;
	int function;
	int chunks_count;
	unsigned long long  repeat_count;
	int default_chunks_count = 100;
	int default_repeat_count = 100000;

	double time_beg = 0;
	double time_end = 0;

	int i;

	// parse arguments
	if (argc == 1)
		help(argv[0]);

	// - function
	for (function = 0; function < OPT_COUNT; function++)
		if (strcasecmp(argv[1], functions[function]) == 0)
			break;

	if (function == OPT_COUNT)
		help(argv[0]);

	// - 16-byte chunks
	if (argc >= 3) {
		chunks_count = atoi(argv[2]);
		if (chunks_count <= 0 || chunks_count > MAX_CHUNKS)
			help(argv[0]);
	} else
		chunks_count = default_chunks_count;

	// - repeat count
	if (argc >= 4) {
		repeat_count = strtoull(argv[3], NULL, 10);
		if (repeat_count <= 0)
			help(argv[0]);
	} else
		repeat_count = default_repeat_count;

	// fill buffer with random data
	srand(time(NULL));
	printf("Data: ");
	for (i = 0; i < sizeof(buffer); i++)
		buffer[i] = rand() % 256;

	for (i = 0; i < chunks_count * 16; i++)
		printf("%X", buffer[i]);
	printf("\n");

	//Result data
	uint32_t popcount_ref, popcount_ref11;
	char failed = 0;

	// run
	printf("chunks=%d, repeat count=%llu\n", chunks_count, repeat_count);

	time_beg = clock();

	switch (function) {
	case 0:
		popcount_ref = popcount(buffer, chunks_count);

		// lookup result is reference
		printf("%10s -> %d\n", functions[1], popcount_ref);

		failed |= verify(2, &builtin_popcount, buffer, chunks_count,
				popcount_ref);
		failed |= verify_m128i(3, &popcount_m128i_sse, buffer, chunks_count,
				popcount_ref);
		failed |= verify_m256i(4, &popcount_m256i_avx, buffer, chunks_count,
				popcount_ref);

		if (failed)
			return EXIT_FAILURE;

		break;

	case 1:
		while (repeat_count--)
			popcount(buffer, chunks_count);
		break;

	case 2:
		while (repeat_count--)
			builtin_popcount(buffer, chunks_count);
		break;

	case 3:
		while (repeat_count--) {
			result = 0;
			for (i = 0; i < chunks_count; i++) {
				__m128i reg = *((__m128i *) (buffer + i * 16));
				result += popcount_m128i_sse(reg);
			}
		}
		break;

	}

	time_end = clock();

	printf("Total time: %f\n", (time_end - time_beg) / CLOCKS_PER_SEC);

	return EXIT_SUCCESS;
}

// eof
