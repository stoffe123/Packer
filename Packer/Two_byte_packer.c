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



__declspec(thread) static  unsigned char buffer[BLOCK_SIZE];

__declspec(thread) static  unsigned long two_byte_freq_table[65536] = { 0 };
__declspec(thread) static  uint8_t pair_table[2048] = { 0 }, master_code;
__declspec(thread) static  unsigned long char_freq[256];
__declspec(thread) static  packProfile profile;

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

uint64_t getGainThreshold(uint64_t source_size) {

	uint64_t res = source_size / profile.twobyte_threshold_divide;

	if (res < profile.twobyte_threshold_min) {
		return profile.twobyte_threshold_min;
	}
	if (res > profile.twobyte_threshold_max) {
		return profile.twobyte_threshold_max;
	}
	return res;
}

int create_two_byte_table(uint64_t source_size) {
	debug("\n creating two_byte_table \n");

	value_freq_t master = find_best_code(char_freq);
	master_code = master.value;
	uint64_t threshold = getGainThreshold(source_size);
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

	FILE* infil, * utfil = NULL;

	debug("\nTwo-byte pack pass=%d", pass);

	uint64_t buffer_startpos = 0, source_size;

	infil = openRead(src);
	source_size = get_file_size(infil);

	uint64_t total_size = get_file_size(infil);
	int pair_table_pos;
	if (pass >= 2) {
		pair_table_pos = create_two_byte_table(source_size);

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

	uint64_t buffer_endpos = fread(buffer, 1, BLOCK_SIZE, infil);
	if (buffer_endpos == BLOCK_SIZE) {
		printf("\n too large file in two_byte packer!");
		exit(1);
	}

	while (buffer_startpos < buffer_endpos) {
		bool lastChar = false;
		if (buffer_startpos == buffer_endpos - 1) {
			lastChar = true;
		}
		unsigned char ch1 = buffer[buffer_startpos];
		unsigned char ch2 = buffer[buffer_startpos + 1];
		unsigned int val = ch1 + 256 * ch2;
		if (pass >= 2) {
			unsigned int code = (lastChar ? 256 :
				find_code_for_pair(val, pair_table_pos));


			if (code == 256) {
				// not found
				if (pass == 2) {
					if (char_freq[ch1] < LONG_MAX) {
						char_freq[ch1]++;
					}
				}
				else {
					if (is_code(ch1, pair_table_pos)) {
						fputc(master_code, utfil);
					}
					fputc(ch1, utfil);
				}
				buffer_startpos++;

			}
			else { // write the code for the pair
				if (pass == 2) {
					if (two_byte_freq_table[val] < LONG_MAX) {
						two_byte_freq_table[val]++;
					}
				}
				else {
					fputc((unsigned char)code, utfil);
				}
				buffer_startpos += 2;
			}
		}
		else { // pass == 1
			if (two_byte_freq_table[val] < LONG_MAX) {
				two_byte_freq_table[val]++;
			}
			if (char_freq[ch1] < LONG_MAX) {
				char_freq[ch1]++;
			}
			buffer_startpos++;
		}
	}//end while

	if (pass == 3) {
		fclose(utfil);
	}
	fclose(infil);
}


void twoBytePack(const wchar_t* src, const wchar_t* dest, packProfile prof)
{
	profile = prof;
	two_byte_pack_internal(src, dest, 1); //analyse and build metadata
	two_byte_pack_internal(src, dest, 2); //simulate pack and adjust metadata
	two_byte_pack_internal(src, dest, 3); //pack
}


void two_byte_pack(const char* src, const char* dest, packProfile prof) {
	wchar_t d[500], s[500];
	toUni(d, dest);
	toUni(s, src);
	twoBytePack(s, d, prof);
}



