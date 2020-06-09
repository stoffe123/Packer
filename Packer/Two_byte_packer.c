#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "common_tools.h"
#include "packer_commons.h"

#define VERBOSE false
#define START_CODES_SIZE 2

/* Two-byte packer */

//Global variables used in compressor
static  FILE* infil, * utfil;

static   uint64_t buffer_endpos, buffer_startpos, buffer_min,
buffer_size = 2048, source_size;
static   unsigned char* buffer;

static   const char* base_dir;
static   unsigned long two_byte_freq_table[65536] = { 0 };
static   uint8_t pair_table[2048] = { 0 }, master_code;
static   unsigned long char_freq[256];
static packProfile profile;

static void move_buffer(unsigned int steps) {
	buffer_startpos += steps;
	if (buffer_endpos == buffer_size) {
		uint64_t buffer_left = buffer_size - buffer_startpos;
		if (buffer_left < buffer_min) {
			//load new buffer!!			
			//debug("Load new buffer of: %d", buffer_size);

			unsigned char* new_buf = (unsigned char*)malloc(buffer_size * sizeof(unsigned char));
			for (uint64_t i = 0; i < (buffer_size - buffer_startpos); i++) {
				new_buf[i] = buffer[i + buffer_startpos];
			}
			uint64_t res = fread(&new_buf[buffer_size - buffer_startpos], 1, buffer_startpos, infil);
			buffer_endpos = res + (buffer_size - buffer_startpos);
			free(buffer);
			buffer = new_buf;
			buffer_startpos = 0;
		}
	}
}

val_freq_t find_best_two_byte() {
	unsigned long best = 0;
	uint64_t two_byte = 0;
	for (unsigned int i = 0; i < 65536; i++) {
		if (two_byte_freq_table[i] > best) {
			best = two_byte_freq_table[i];
			two_byte = i;
		}
	}
	if (best > 0) {
		two_byte_freq_table[two_byte] = 0; // mark as used
	}
	val_freq_t res;
	res.value = two_byte;
	res.freq = best;
	return res;
}

uint64_t getGainThreshold() {

	uint64_t res = source_size / profile.twobyte_threshold_divide;

	if (res < profile.twobyte_threshold_min) {
		return profile.twobyte_threshold_min;
	}
	if (res > profile.twobyte_threshold_max) {
		return profile.twobyte_threshold_max;
	}
	return res;
}

int create_two_byte_table() {
	debug("\n creating two_byte_table \n");

	// extract one code to use with seqpack later
	if (profile.twobyte_ratio != 100) {
		find_best_code(char_freq);
	}
	
	value_freq_t master = find_best_code(char_freq);
	master_code = master.value;
	uint64_t threshold = getGainThreshold();
	debug("\n Two byte packer gain threshhold %lld", threshold);
	bool found_twobyte = false;
	int pair_table_pos = START_CODES_SIZE;
	do {
		val_freq_t two_byte = find_best_two_byte(); //ineffecient to search whole 65k table every time
		if (two_byte.freq < threshold) {
			break;
		}
		value_freq_t code = find_best_code(char_freq);
		found_twobyte = (code.freq + threshold < two_byte.freq);
		if (found_twobyte) {
			//worthwile
			pair_table[pair_table_pos++] = (uint8_t)code.value;
			pair_table[pair_table_pos++] = (uint8_t)(two_byte.value % 256);
			pair_table[pair_table_pos++] = (uint8_t)(two_byte.value / 256);
			debug("\n Code %lu for '%c%c' with freq:(%lld,%lld)", code.value, two_byte.value % 256, two_byte.value / 256, two_byte.freq, code.freq);
		}
	} while (found_twobyte);

	assert(pair_table_pos > 0, "two_byte_table_pos = 0 !!!");
	pair_table[0] = (uint8_t)((pair_table_pos - START_CODES_SIZE) / 3);
	pair_table[1] = master_code;
	debug("\nCreated two_byte table size: %d\n", pair_table_pos);
	return pair_table_pos;
}

unsigned int find_code_for_pair(unsigned int val, int pair_table_pos) {
	for (int i = START_CODES_SIZE; i < pair_table_pos; i += 3) {
		unsigned int table_val = pair_table[i + 1] +
			256 * pair_table[i + 2];
		if (table_val == val) {
			return pair_table[i];
		}
	}
	return 256;
}

bool is_code(unsigned char ch, int pair_table_pos) {
	if (ch == master_code) {
		return true;
	}
	for (int i = START_CODES_SIZE; i < pair_table_pos; i += 3) {
		if (pair_table[i] == ch) {
			return true;
		}
	}
	return false;
}


//--------------------------------------------------------------------------------------------------------


void two_byte_pack_internal(const wchar_t* src, const wchar_t* dest, int pass) {

	debug("\nTwo-byte pack pass=%d", pass);

	buffer_startpos = 0;
	buffer_min = 20;

	infil = openRead(src);	
	source_size = get_file_size(infil);

	uint64_t total_size = get_file_size(infil);
	int pair_table_pos;
	if (pass >= 2) {		
		pair_table_pos = create_two_byte_table();

		if (pass == 3) {
			utfil = openWrite(dest);

			//write the metadata table
			for (int i = 0; i < pair_table_pos; i++) {

				putc(pair_table[i], utfil);
				debug(" %d", pair_table[i]);
			}
		}
	}
	if (pass < 3) {
		for (int i = 0; i < 65536; i++) {
			two_byte_freq_table[i] = 0;
		}
		for (int i = 0; i < 256; i++) {
			char_freq[i] = 0;
		}
	}

	buffer_endpos = fread(buffer, 1, buffer_size, infil);

	unsigned int seq_len = 1;

	while (buffer_startpos < buffer_endpos) {

		unsigned char ch1 = buffer[buffer_startpos];
		unsigned char ch2 = buffer[buffer_startpos + 1];
		unsigned int val = ch1 + 256 * ch2;
		if (pass >= 2) {
			unsigned int code = find_code_for_pair(val, pair_table_pos);

			if (code == 256) {
				// not found
				if (pass == 2) {
					if (char_freq[ch1] < LONG_MAX) {
						char_freq[ch1]++;
					}
				}
				else {
					if (is_code(ch1, pair_table_pos)) {
						WRITE(utfil, master_code);
					}
					WRITE(utfil, ch1);
				}
				move_buffer(1);

			}
			else { // write the code for the pair
				if (pass == 2) {
					if (two_byte_freq_table[val] < LONG_MAX) {
						two_byte_freq_table[val]++;
					}
				}
				else {
					WRITE(utfil, (unsigned char)code);
				}
				move_buffer(2);
			}
		}
		else { // pass == 1
			if (two_byte_freq_table[val] < LONG_MAX) {
				two_byte_freq_table[val]++;
			}
			if (char_freq[ch1] < LONG_MAX) {
				char_freq[ch1]++;
			}
			move_buffer(1);
		}
	}//end while

	if (pass == 3) {
		fclose(utfil);
	}
	fclose(infil);
}


void two_byte_packw(const wchar_t* src, const wchar_t* dest, packProfile prof)
{
	profile = prof;
	buffer = (unsigned char*)malloc(buffer_size * sizeof(unsigned char));
	two_byte_pack_internal(src, dest, 1); //analyse and build metadata
	two_byte_pack_internal(src, dest, 2); //simulate pack and adjust metadata
	two_byte_pack_internal(src, dest, 3); //pack
	free(buffer);
}


void two_byte_pack(const char* src, const char* dest, packProfile prof) {
	wchar_t d[500], s[500];
	toUni(d, dest);
	toUni(s, src);
	two_byte_packw(s, d, prof);
}



