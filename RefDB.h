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

using namespace std;

struct chromoMeta{
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
	void add_chromo(char* chromo_string, uint64_t length);
	void finish_and_store(string db_name);

	// loading functions
	void init_load();
	void unload_all();
	void load_chromo(int32_t chromo_num);
	void unload_chromo(int32_t chromo_num);

	// query supports only up to 249 length at max.
	bool query(uint32_t chromo_num, uint32_t chromo_pos, __m256i& bit0, __m256i& bit1);

	uint32_t get_chromo_length();
private:
	// variables for loading and query
	chromoMeta * chromo_array;

	// variables for generate
	int length_gen;
	uint8_t *bit0_gen;
	uint8_t *bit1_gen;

	// file for generate
	ofstream temp_file;

	// file for loading
	ifstream db_file;
};

#endif /* REFDB_H_ */
