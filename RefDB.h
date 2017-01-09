/*
 * refDB.h
 *
 *  Created on: May 30, 2013
 *      Author: hxin
 */

#ifndef REFDB_H_
#define REFDB_H_

#include <stdint.h>
#include <string>
#include <fstream>
#include <vector>
#include "bit_convert.h"
#include "shift.h"
#include "mask.h"

using namespace std;

struct chromoMeta{
	chromoMeta() {
		pos = 0;		
		loaded = false;
		length = 0;
		bit0 = NULL;
		bit1 = NULL;
	};

	~chromoMeta() {
		unload();
	};

	void unload() {
		if (loaded) {
			delete [] bit0;
			delete [] bit1;
			loaded = false;
			bit0 = NULL;
			bit1 = NULL;
		}
	}

	streampos pos;
	bool loaded;
	uint64_t length;
	uint8_t *bit0;
	uint8_t *bit1;
};

class RefDB {
public:
	RefDB();
	~RefDB();

	// Generate functions
	void init_generate();
	void add_chromo(char *chromo_string, uint64_t length);
	void finish_and_store(string db_name);

	// loading functions
	void init_load(string db_name);
	void unload_all();
	bool load_chromo(int chromo_num);
	void unload_chromo(int chromo_num);

	// query supports only up to 249 length at max.
	bool query(int chromo_num, int chromo_pos, int query_length, __m256i& bit0, __m256i& bit1);

	uint32_t get_chromo_length();
private:
	// variables for loading and query
	chromoMeta * chromo_array;
	int chromo_total;

	// variables for generate
	uint64_t length_gen;
	uint8_t *bit0_gen;
	uint8_t *bit1_gen;
	vector<uint64_t> length_array;
	vector<streampos> pos_array;

	// file for generate
	ofstream temp_file;

	// file for loading
	ifstream db_file;
};

#endif /* REFDB_H_ */
