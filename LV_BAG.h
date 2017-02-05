#include <iostream>
#include <x86intrin.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <cstring>
#include <string>
#include <cstdlib>

#ifndef _MAX_LENGTH_
#define _MAX_LENGTH_ 256
#endif

using namespace std;

#ifndef __ED_INFO_H_
#define __ED_INFO_H_

enum ED_TYPE {MISMATCH, A_INS, B_INS};

struct ED_INFO {
	ED_TYPE type;
	int id_length;	
};

#endif

#ifndef __LV_H_
#define __LV_H_

enum ED_modes {ED_LOCAL, ED_GLOBAL, ED_SEMI_FREE_BEGIN, ED_SEMI_FREE_END};

class LV {
public:
	LV();
	~LV();

	void init(int gap_threshold, int af_threshold, ED_modes mode, int ms_penalty, int gap_open_penalty, int gap_ext_penalty);

	void load_reads(char *read, char *ref, int length);

	void reset();	
	void run();
	bool check_pass();
	void backtrack();
	int get_ED();
	string get_CIGAR();
private:
	int count_ID_length(int lane_idx, int start_pos);

	// information of each lane
	int **start;
	int **end;
    int gap_threshold;
    int af_threshold;
    int ms_penalty;
    int gap_open_penalty;
    int gap_ext_penalty;
	int **I_pos;
	int **D_pos;
	int ED_count;

	// backtracking data
	ED_modes mode;
	bool ED_pass;
	int final_lane_idx;
	int final_ED;
	ED_INFO *ED_info;
	int converge_ED;
	int converge_final_lane;

	// serrogates
	int mid_lane;
	int total_lanes;
	
	// buffers
	int buffer_length;
	
	char A[_MAX_LENGTH_];
	char B[_MAX_LENGTH_];
};

#endif

