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
static   long two_byte_freq_table[65536] = { 0 };
static   uint8_t pair_table[2048] = { 0 }, master_code;
static   uint64_t char_freq[256];
static packProfile_t profile;

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


static value_freq_t find_best_code() {
	unsigned char best_code;
	uint64_t freq = ULONG_MAX;
	for (unsigned int i = 0; i < 256; i++) {
		if (char_freq[i] < freq) {
			freq = char_freq[i];
			best_code = i;
		}
	}
	char_freq[best_code] = ULONG_MAX; // mark it as used!
	value_freq_t res;
	res.value = best_code;
	res.freq = freq;
	return res;
}

value_freq_t find_best_two_byte() {
	long best = 0;
	int two_byte = -1;
	for (unsigned int i = 0; i < 65536; i++) {
		if (two_byte_freq_table[i] > best) {
			best = two_byte_freq_table[i];
			two_byte = i;
		}
	}
	if (two_byte >= 0) {
		two_byte_freq_table[two_byte] = 0; // mark as used
	}
	value_freq_t res;
	res.value = two_byte;
	res.freq = best;
	return res;
}

int get_gain_threshhold() {

	uint64_t res = source_size / profile.twobyte_threshold_divide;

	if (res < profile.twobyte_threshold_min) {
		res = profile.twobyte_threshold_min;
	}
	if (res > profile.twobyte_threshold_max) {
		res = profile.twobyte_threshold_max;
	}
	return res;
}

int create_two_byte_table() {
	debug("\n creating two_byte_table \n");

	// extract one code to use with seqpack later
	if (profile.twobyte_threshold_max > 0) {
		find_best_code();
	}

	value_freq_t master = find_best_code();
	master_code = master.value;
	int threshhold = get_gain_threshhold();
	printf("\n Two byte packer gain threshhold %d", threshhold);
	bool found_twobyte = false;
	int pair_table_pos = START_CODES_SIZE;
	do {
		value_freq_t two_byte = find_best_two_byte(); //ineffecient to search whole 65k table every time
		if (two_byte.value == -1) {
			break;
		}
		value_freq_t code = find_best_code();
		found_twobyte = (code.freq + threshhold < two_byte.freq);
		if (found_twobyte) {
			//worthwile
			pair_table[pair_table_pos++] = (uint8_t)code.value;
			pair_table[pair_table_pos++] = (uint8_t)(two_byte.value % 256);
			pair_table[pair_table_pos++] = (uint8_t)(two_byte.value / 256);
			debug("\n Code %d for '%c%c' with freq:(%d,%d)", code.value, two_byte.value % 256, two_byte.value / 256, two_byte.freq, code.freq);
		}
	} while (found_twobyte);

	assert(pair_table_pos > 0, "two_byte_table_pos = 0 !!!");
	pair_table[0] = (uint8_t)((pair_table_pos - START_CODES_SIZE) / 3);
	pair_table[1] = master_code;
	printf("\nCreated two_byte table size: %d\n", pair_table_pos);
	return pair_table_pos;
}

unsigned int find_code_for_pair(unsigned char ch1, unsigned char ch2, int pair_table_pos) {
	unsigned int val = ch1 + 256 * ch2;
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

	errno_t err = _wfopen_s(&infil, src, L"rb");
	if (err != 0) {
		wprintf(L"\nTwo byte pack: hittade inte infil: %s", src);getchar();exit(1);
	}
	source_size = get_file_size(infil);

	uint64_t total_size = get_file_size(infil);
	int pair_table_pos;
	if (pass == 2) {		
		err = _wfopen_s(&utfil, dest, L"wb");
		if (err != 0) {
			wprintf(L"\nTwo byte pack: Hittade inte utfil!%s", dest); getchar(); exit(1);
		}
		// start compression!

		pair_table_pos = create_two_byte_table();


		//write the metadata table
		for (int i = 0; i < pair_table_pos; i++) {

			putc(pair_table[i], utfil);
			debug(" %d", pair_table[i]);
		}
	}
	else {  // pass = 1
		for (int i = 0; i < 65536; i++) {
			two_byte_freq_table[i] = 0;
		}
		for (int i = 0; i < 256; i++) {
			char_freq[i] = 0;
		}
	}

	/* start compression */

	buffer_endpos = fread(buffer, 1, buffer_size, infil);

	unsigned int seq_len = 1;

	while (buffer_startpos < buffer_endpos) {

		unsigned char ch1 = buffer[buffer_startpos];
		unsigned char ch2 = buffer[buffer_startpos + 1];

		if (pass == 2) {
			unsigned int code = find_code_for_pair(ch1, ch2, pair_table_pos);
			if (code == 256) {
				// not found
				if (is_code(ch1, pair_table_pos)) {
					WRITE(utfil, master_code);
				}
				WRITE(utfil, ch1);
				move_buffer(1);

			}
			else { // write the code for the pair
				WRITE(utfil, (unsigned char)code);
				move_buffer(2);
			}
		}
		else { // pass == 1

			long val = ch1 + 256 * ch2; // always low first
			if (two_byte_freq_table[val] < LONG_MAX) {
				two_byte_freq_table[val]++;
			}
			if (char_freq[ch1] < LONG_MAX) {
				char_freq[ch1]++;
			}
			move_buffer(1);
		}
	}//end while

	if (pass == 2) {
		fclose(utfil);

	}
	fclose(infil);
}


void two_byte_packw(const wchar_t* src, const wchar_t* dest, packProfile_t prof)
{
	profile = prof;
	buffer = (unsigned char*)malloc(buffer_size * sizeof(unsigned char));
	two_byte_pack_internal(src, dest, 1); //analyse and build meta-data
	two_byte_pack_internal(src, dest, 2); //pack
	free(buffer);
}


void two_byte_pack(const char* src, const char* dest, packProfile_t prof) {
	wchar_t d[500], s[500];
	toUni(d, dest);
	toUni(s, src);
	two_byte_packw(s, d, prof);
}



