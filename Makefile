EXECUTABLE = popcount bit_convert vectorED vectorLEAP testNW testLV testLV_BAG vectorSHD_ED testRefDB# countPassFilter vector_filter string_cp shift test_SIMD_ED vectorED vectorLV sse.o diffED #ssse3_popcount test_modifier

CXX = g++-5

LD = ld

LDFLAGS = -r

NW_PATH = ./needleman_wunsch-0.3.5
LIBS_PATH = $(NW_PATH)/libs
UTILITY_LIB_PATH := $(LIBS_PATH)/utility_lib
STRING_BUF_PATH := $(LIBS_PATH)/string_buffer
BIOINF_LIB_PATH := $(LIBS_PATH)/bioinf
SCORING_PATH := $(LIBS_PATH)/alignment_scoring

#CFLAGS = -g --std=c++11 -mbmi -mavx2 -msse4.2 -I . -Ddebug \
#CFLAGS = -O3 --std=c++11 -mbmi -mavx2 -msse4.2 -I . -DNO_BIT_VEC\

CFLAGS = -O3 --std=c++11 -mbmi -mavx2 -msse4.2 -I . \
         -I $(UTILITY_LIB_PATH) \
         -I $(STRING_BUF_PATH) -I $(BIOINF_LIB_PATH) -I $(SCORING_PATH) \
         -I $(NW_PATH) -DCOMPILE_TIME='"$(shell date)"' -DSCORE_TYPE='int' \
         -fpermissive
AVX512_FLAGS = $(CFLAGS) -DUSE_AVX512 -mavx512f -mavx512bw -mavx512vl
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS += -I/opt/local/include
endif

#CFLAGS = -O3 -march=native -P -E

all: $(EXECUTABLE)

LV_BAG.o: LV_BAG.cc LV_BAG.h
	$(CXX) -O3 -c $< -o $@

LV.o: LV.cc LV.h
	$(CXX) -O3 -c $< -o $@

SIMD_ED.o: SIMD_ED.cc SIMD_ED.h
	$(CXX) $(CFLAGS) -c $< -o $@

print.o: print.c print.h
	$(CXX) $(CFLAGS) -c $< -o $@

popcount.o: popcount.c popcount.h
	$(CXX) $(CFLAGS) -c $< -o $@
	
popcount: popcount.o print.o popcountMain.c
	$(CXX) $(CFLAGS) $^ -o $@

bit_convert.o: bit_convert.c bit_convert.h
	$(CXX) $(CFLAGS) -c $< -o $@
	
bit_convert: print.o bit_convert.o bit_convertMain.c
	$(CXX) $(CFLAGS) $^ -o $@

shift.o: shift.c shift.h
	$(CXX) $(CFLAGS) -c $< -o $@

SHD.o: SHD.cc SHD.h
	$(CXX) $(CFLAGS) -c $< -o $@

mask.o: mask.c mask.h
	$(CXX) $(CFLAGS) -c $< -o $@

sse.o: mask.o print.o bit_convert.o popcount.o vector_filter.o
	$(LD) $(LDFLAGS) $^ -o $@

vector_filter: mask.o print.o bit_convert.o popcount.o vector_filter.o vector_filterMain.c
	$(CXX) $(CFLAGS) $^ -o $@

countPassFilter: mask.o print.o bit_convert.o popcount.o vector_filter.o countPassFilter.cc
	$(CXX) $(CFLAGS) $^ -o $@

timeSSE: timeSSE.c
	$(CXX) $(CFLAGS) $< -o $@

needleman_wunsch.o: $(NW_PATH)/needleman_wunsch.c $(NW_PATH)/needleman_wunsch.h 
	$(CXX) $(CFLAGS) -c $< -o $@

vectorED: SIMD_ED.o print.o bit_convert.o vectorED.cc shift.o SHD.o mask.o popcount.o needleman_wunsch.o $(wildcard $(SCORING_PATH)/*.c) $(UTILITY_LIB_PATH)/utility_lib.c $(BIOINF_LIB_PATH)/bioinf.c $(STRING_BUF_PATH)/string_buffer.c $(NW_PATH)/nw_cmdline.c 
	$(CXX) $(CFLAGS) $^ -o $@ -lz 

vectorLEAP: SIMD_ED.o print.o bit_convert.o vectorED.cc shift.o SHD.o mask.o popcount.o needleman_wunsch.o $(wildcard $(SCORING_PATH)/*.c) $(UTILITY_LIB_PATH)/utility_lib.c $(BIOINF_LIB_PATH)/bioinf.c $(STRING_BUF_PATH)/string_buffer.c $(NW_PATH)/nw_cmdline.c 
	$(CXX) $(CFLAGS) $^ -o $@ -lz 

SIMD_ED_avx512.o: SIMD_ED.cc SIMD_ED.h
	$(CXX) $(AVX512_FLAGS) -c $< -o $@

bit_convert_avx512.o: bit_convert.c bit_convert.h
	$(CXX) $(AVX512_FLAGS) -c $< -o $@

shift_avx512.o: shift.c shift.h
	$(CXX) $(AVX512_FLAGS) -c $< -o $@

SHD_avx512.o: SHD.cc SHD.h
	$(CXX) $(AVX512_FLAGS) -c $< -o $@

mask_avx512.o: mask.c mask.h
	$(CXX) $(AVX512_FLAGS) -c $< -o $@

popcount_avx512.o: popcount.c popcount.h
	$(CXX) $(AVX512_FLAGS) -c $< -o $@

vectorED_avx512: SIMD_ED_avx512.o print.o bit_convert_avx512.o vectorED.cc shift_avx512.o SHD_avx512.o mask_avx512.o popcount_avx512.o needleman_wunsch.o $(wildcard $(SCORING_PATH)/*.c) $(UTILITY_LIB_PATH)/utility_lib.c $(BIOINF_LIB_PATH)/bioinf.c $(STRING_BUF_PATH)/string_buffer.c $(NW_PATH)/nw_cmdline.c
	$(CXX) $(AVX512_FLAGS) $^ -o $@ -lz

testNW: SIMD_ED.o print.o bit_convert.o testNW.cc shift.o SHD.o mask.o popcount.o needleman_wunsch.o $(wildcard $(SCORING_PATH)/*.c) $(UTILITY_LIB_PATH)/utility_lib.c $(BIOINF_LIB_PATH)/bioinf.c $(STRING_BUF_PATH)/string_buffer.c $(NW_PATH)/nw_cmdline.c 
	$(CXX) $(CFLAGS) $^ -o $@ -L/usr/local/lib -lz -lparasail

testLV: LV.o testLV.cc
	$(CXX) -O3 $^ -o $@

testLV_BAG: LV_BAG.o testLV_BAG.cc
	$(CXX) -O3 $^ -o $@

vectorSHD_ED: SIMD_ED.o SHD.o mask.o print.o bit_convert.o shift.o popcount.o vectorSHD_ED.cc
	$(CXX) $(CFLAGS) $^ -o $@

vectorSHD_ED_avx512: SIMD_ED_avx512.o SHD_avx512.o mask_avx512.o print.o bit_convert_avx512.o shift_avx512.o popcount_avx512.o vectorSHD_ED.cc
	$(CXX) $(AVX512_FLAGS) $^ -o $@

tests/test_avx512_bit_convert: tests/test_avx512_bit_convert.cpp bit_convert_avx512.o
	$(CXX) $(AVX512_FLAGS) $^ -o $@

tests/test_avx512_shift: tests/test_avx512_shift.cpp shift_avx512.o
	$(CXX) $(AVX512_FLAGS) $^ -o $@

tests/test_avx512_lane_masks: tests/test_avx512_lane_masks.cpp SIMD_ED_avx512.o SHD_avx512.o mask_avx512.o print.o bit_convert_avx512.o shift_avx512.o popcount_avx512.o
	$(CXX) $(AVX512_FLAGS) $^ -o $@

tests/test_avx512_vth: tests/test_avx512_vth.cpp SIMD_ED_avx512.o SHD_avx512.o mask_avx512.o print.o bit_convert_avx512.o shift_avx512.o popcount_avx512.o
	$(CXX) $(AVX512_FLAGS) $^ -o $@

tests/test_avx512_full_correctness: tests/test_avx512_full_correctness.cpp SIMD_ED_avx512.o SHD_avx512.o mask_avx512.o print.o bit_convert_avx512.o shift_avx512.o popcount_avx512.o
	$(CXX) $(AVX512_FLAGS) $^ -o $@

test_avx512_bit_convert: tests/test_avx512_bit_convert
	./tests/test_avx512_bit_convert

test_avx512_shift: tests/test_avx512_shift
	./tests/test_avx512_shift

test_avx512_lane_masks: tests/test_avx512_lane_masks
	./tests/test_avx512_lane_masks

test_avx512_vth: tests/test_avx512_vth
	./tests/test_avx512_vth

test_avx512_full_correctness: tests/test_avx512_full_correctness
	./tests/test_avx512_full_correctness

test_avx512_correctness: test_avx512_bit_convert test_avx512_shift test_avx512_lane_masks test_avx512_vth test_avx512_full_correctness

backend_runner_sse: tests/backend_runner_sse

backend_runner_avx2: tests/backend_runner_avx2

backend_runner_avx512: tests/backend_runner_avx512

tests/backend_runner_sse: tests/backend_runner.cpp bit_convert.o shift.o
	$(CXX) $(CFLAGS) -DBACKEND_SSE $^ -o $@

tests/backend_runner_avx2: tests/backend_runner.cpp SIMD_ED.o SHD.o mask.o print.o bit_convert.o shift.o popcount.o
	$(CXX) $(CFLAGS) -DBACKEND_AVX2 $^ -o $@

tests/backend_runner_avx512: tests/backend_runner.cpp SIMD_ED_avx512.o SHD_avx512.o mask_avx512.o print.o bit_convert_avx512.o shift_avx512.o popcount_avx512.o
	$(CXX) $(AVX512_FLAGS) -DBACKEND_AVX512 $^ -o $@

test_backend_consistency: backend_runner_sse backend_runner_avx2 backend_runner_avx512
	python3 tests/test_backend_consistency.py

test_all_backends_small: test_backend_consistency

test_SIMD_ED: SIMD_ED.o vector_filter.o bit_convert.o mask.o popcount.o print.o test_ED.cc
	$(CXX) $(CFLAGS) $^ -o $@
		
diffED: LV.o SIMD_ED.o mask.o print.o bit_convert.o popcount.o vector_filter.o diffED.cc
	$(CXX) $(CFLAGS) $^ -o $@

RefDB.o: RefDB.cc RefDB.h
	$(CXX) $(CFLAGS) -c $< -o $@

testRefDB: RefDB.o bit_convert.o shift.o print.o RefDBMain.cc
	$(CXX) $(CFLAGS) $^ -o $@

#ssse3_popcount: ssse3_popcount.c
#	$(CXX) $(CFLAGS) $< -o $@
	

	
.PHONY : clean test_avx512_bit_convert test_avx512_shift test_avx512_lane_masks test_avx512_vth test_avx512_full_correctness test_avx512_correctness backend_runner_sse backend_runner_avx2 backend_runner_avx512 test_backend_consistency test_all_backends_small

clean:
	rm -f $(EXECUTABLE) *.o tests/test_avx512_bit_convert tests/test_avx512_shift tests/test_avx512_lane_masks tests/test_avx512_vth tests/test_avx512_full_correctness tests/backend_runner_sse tests/backend_runner_avx2 tests/backend_runner_avx512 vectorED_avx512 vectorSHD_ED_avx512
