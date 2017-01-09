/*
 * RefDB.cc
 *
 *  Created on: May 30, 2013
 *      Author: hxin
 */
#include "RefDB.h"
#include "shift.h"
#include <x86intrin.h>
#include <iostream>
#include <cstring>

#define BLANK_SPACE 1048576

RefDB::RefDB() {
	chromo_array = NULL;
	length_gen = 0;
	bit0_gen = NULL;
	bit1_gen = NULL;
}

RefDB::~RefDB() {
	if (chromo_array != NULL) {
		delete [] chromo_array;
		chromo_array = NULL;
	}
	if (bit0_gen != NULL) {
		delete [] bit0_gen;
		bit0_gen = NULL;
	}
	if (bit1_gen != NULL) {
		delete [] bit1_gen;
		bit1_gen = NULL;
	}

	if (temp_file.is_open() )
		temp_file.close();
	if (db_file.is_open() )
		db_file.close();
}

void RefDB::init_generate() {
	if (!temp_file.is_open() ) {
		temp_file.open("temp.refDB", ofstream::out);
	}
	else {
		temp_file.close();
		temp_file.open("temp.refDB", ofstream::out);
	}
	pos_array.push_back(temp_file.tellp() );
}

void RefDB::add_chromo(char *chromo_string, uint64_t length) {
	char aligned_buff [AVX_BIT_LENGTH] __aligned__;
	uint8_t bit0_buff [AVX_BYTE_LENGTH] __aligned__;
	uint8_t bit1_buff [AVX_BYTE_LENGTH] __aligned__;

	int byte_length = (length - 1) / 8 + 1;

	if (length_gen < byte_length) {
		if (bit0_gen != NULL) {
			delete [] bit0_gen;
			bit0_gen = NULL;
		}
		if (bit1_gen != NULL) {
			delete [] bit1_gen;
			bit1_gen = NULL;
		}
		bit0_gen = new uint8_t [byte_length];
		bit1_gen = new uint8_t [byte_length];
		length_gen = byte_length;
	}

	length_array.push_back(length);

	for (int i = 0; i < byte_length; i += AVX_BIT_LENGTH) {
		int corrected_length;
		if (i + AVX_BIT_LENGTH > byte_length)
			corrected_length = byte_length - i;
		else
			corrected_length = AVX_BIT_LENGTH;

		memcpy(aligned_buff, chromo_string + i, corrected_length * sizeof(uint8_t));
		avx_convert2bit(aligned_buff, bit0_buff, bit1_buff);
		memcpy(bit0_gen + i / AVX_BYTE_LENGTH, bit0_buff, (corrected_length - 1) / 8 + 1);
		memcpy(bit1_gen + i / AVX_BYTE_LENGTH, bit1_buff, (corrected_length - 1) / 8 + 1);
	}

	temp_file.write((char*) bit0_gen, byte_length);
	temp_file << endl;
	temp_file.write((char*) bit1_gen, byte_length);
	temp_file << endl;

	pos_array.push_back(temp_file.tellp() );
}

void RefDB::finish_and_store(string db_name) {
	if (temp_file.is_open())
		temp_file.close();

	ifstream temp_file_reader("temp.ref_DB", ifstream::in);

	ofstream write_file;
	write_file.open((db_name + ".ref_DB").c_str(), ofstream::out);

	write_file << length_array.size() << endl;

	for (int i = 0; i < length_array.size(); i++) {
		write_file << length_array[i] << " " << (int) pos_array[i] << endl;
	}

	if (write_file.tellp() > BLANK_SPACE) {
		cerr << "too many meta data" << endl;
		exit(1);
	}

	write_file.seekp(BLANK_SPACE + 1);

	for (int i = 0; i < length_array.size(); i++) {
		string str_buff;
		// write bit0
		temp_file_reader >> str_buff;
		write_file << str_buff << endl;
		// write bit1
		temp_file_reader >> str_buff;
		write_file << str_buff << endl;
	}

	temp_file_reader.close();
	write_file.close();
	remove("temp.ref_DB");
}

void RefDB::init_load(string db_name) {
	if (db_file.is_open() )
		db_file.close();
	if (chromo_array != NULL)
		delete [] chromo_array;

	db_file.open(db_name + ".ref_DB");

	db_file >> chromo_total;

	chromo_array = new chromoMeta [chromo_total];

	for (int i = 0; i < chromo_total; i++) {
		db_file >> chromo_array[i].length;
		int converter;
		db_file >> converter;
		chromo_array[i].pos = converter;
	}
}

bool RefDB::load_chromo(int chromo_num) {
	if (chromo_num > chromo_total)
		return false;
	
	if (chromo_array[chromo_num].loaded)
		return true;

	chromo_array[chromo_num].loaded = true;
	db_file.seekg(chromo_array[chromo_num].pos);
	int byte_size = (chromo_array[chromo_num].length - 1) / 8 + 1;
	chromo_array[chromo_num].bit0 = new uint8_t [byte_size];
	chromo_array[chromo_num].bit1 = new uint8_t [byte_size];
	db_file.read((char*) chromo_array[chromo_num].bit0, byte_size);
	db_file.seekg((int) db_file.tellg() + 1);
	db_file.read((char*) chromo_array[chromo_num].bit1, byte_size);

	return true;
}

void RefDB::unload_chromo(int chromo_num) {
	if (chromo_array[chromo_num].loaded) {
		chromo_array[chromo_num].loaded = false;
		delete [] chromo_array[chromo_num].bit0;
		delete [] chromo_array[chromo_num].bit1;
		chromo_array[chromo_num].bit0 = NULL;
		chromo_array[chromo_num].bit1 = NULL;
	}
}

void RefDB::unload_all() {
	for (int i = 0; i < chromo_total; i++)
		unload_chromo(i);
}

bool RefDB::query(int chromo_num, int chromo_pos, int query_length, __m256i& bit0, __m256i& bit1) {
	if (!chromo_array[chromo_num].loaded || chromo_pos + query_length > chromo_array[chromo_num].length)
		return false;

	int byte_pos = chromo_pos / 8;
	bit0 = _mm256_loadu_si256((__m256i*) (chromo_array[chromo_pos].bit0 + byte_pos) );
	bit1 = _mm256_loadu_si256((__m256i*) (chromo_array[chromo_pos].bit1 + byte_pos) );

	bit0 = shift_left_avx(bit0, chromo_pos % 8);
	bit1 = shift_left_avx(bit1, chromo_pos % 8);

	return true;
}
