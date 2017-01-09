EXECUTABLE = popcount bit_convert vectorED vectorLV vectorSHD_ED testRefDB# countPassFilter vector_filter string_cp shift test_SIMD_ED vectorED vectorLV sse.o diffED #ssse3_popcount test_modifier

CXX = g++-5

LD = ld

LDFLAGS = -r

CFLAGS = -g --std=c++11 -mbmi -mavx2 -msse4.2 -I . -Ddebug
#CFLAGS = -O3 --std=c++11 -mbmi -mavx2 -msse4.2 -I .
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS += -I/opt/local/include
endif

#CFLAGS = -O3 -march=native -P -E

all: $(EXECUTABLE)

LV.o: LV.cc LV.h
	$(CXX) $(CFLAGS) -c $< -o $@

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

vectorED: SIMD_ED.o print.o bit_convert.o vectorED.cc shift.o
	$(CXX) $(CFLAGS) $^ -o $@

vectorLV: LV.o vectorLV.cc
	$(CXX) $(CFLAGS) $^ -o $@

vectorSHD_ED: SIMD_ED.o SHD.o mask.o print.o bit_convert.o shift.o popcount.o vectorSHD_ED.cc
	$(CXX) $(CFLAGS) $^ -o $@

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
	

	
.PHONY : clean

clean:
	rm $(EXECUTABLE) *.o
