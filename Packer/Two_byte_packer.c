#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "seq_packer.h"
#include "seq_packer_commons.h"
#define VERBOSE false
#include "common_tools.h"
#define START_CODES_SIZE 2

/* RLE simple packer */

//global variables used in compressor
static FILE* infil, * utfil;
static unsigned char code;
static unsigned long long buffer_endpos, buffer_startpos, buffer_min, buffer_size = 2048;
static unsigned char* buffer;
static bool code_occurred = false;

static unsigned long long char_freq[256] = { 0 };
static const char* base_dir;
static bool separate;
static long two_byte_freq_table[65536] = { 0 };
static uint8_t pair_table[2048] = { 0xee }, master_code;
static unsigned int pair_table_pos;

static struct value_freq {
	unsigned long value;
	unsigned long freq;
};



static void move_buffer(unsigned int steps) {
	buffer_startpos += steps;
	if (buffer_endpos == buffer_size) {
		unsigned long long buffer_left = buffer_size - buffer_startpos;
		if (buffer_left < buffer_min) {
			//load new buffer!!			
			debug("Load new buffer of: %d", buffer_size);

			unsigned char* new_buf = (unsigned char*)malloc(buffer_size * sizeof(unsigned char));
			for (unsigned long i = 0; i < (buffer_size - buffer_startpos); i++) {
				new_buf[i] = buffer[i + buffer_startpos];
			}
			unsigned long long res = fread(&new_buf[buffer_size - buffer_startpos], 1, buffer_startpos, infil);
			buffer_endpos = res + (buffer_size - buffer_startpos);
			free(buffer);
			buffer = new_buf;
			buffer_startpos = 0;
		}
	}
}


static struct value_freq find_best_code() {
	unsigned char best_code;
	unsigned long freq = ULONG_MAX;
	for (unsigned int i = 0; i < 256; i++) {
		if (char_freq[i] < freq) {
			freq = char_freq[i];
			best_code = i;
		}
	}//end for



	char_freq[best_code] = ULONG_MAX; // mark it as used!
	struct value_freq res;
	res.value = best_code;
	res.freq = freq;
	return res;
}



static void inc_char_freq(unsigned char c) {
	if (char_freq[c] < LONG_MAX) {
		char_freq[c]++;
	}
}

void add_to_two_byte_table(unsigned int c) {
	pair_table[pair_table_pos++] = (unsigned char)c;
	//printf("%d,", c);
}


struct value_freq find_best_two_byte() {
	long best = 0;
	unsigned int two_byte;
	for (unsigned int i = 0; i < 65536; i++) {
		if (two_byte_freq_table[i] > best) {
			best = two_byte_freq_table[i];
			two_byte = i;
		}
	}
	two_byte_freq_table[two_byte] = 0; // mark as used
	struct value_freq res;
	res.value = two_byte;
	res.freq = best;
	return res;
}

void create_two_byte_table() {
	//printf("\n creating two_byte_table \n");
	struct value_freq master = find_best_code();
	master_code = master.value;

	bool found_code = false;
	pair_table_pos = START_CODES_SIZE;
	do {
		struct value_freq two_byte = find_best_two_byte();
		struct value_freq code = find_best_code();
		found_code = (code.freq + 102 < two_byte.freq);
		if (found_code) {
			//worthwile
			add_to_two_byte_table(code.value);
			add_to_two_byte_table(two_byte.value % 256);
			add_to_two_byte_table(two_byte.value / 256);
			//printf("\n code %d for '%c%c' with freq:(%d,%d)", code.value, two_byte.value % 256, two_byte.value / 256, two_byte.freq, code.freq);
		}
	} while (found_code);

	assert(pair_table_pos > 0, "two_byte_table_pos = 0 !!!");
	pair_table[0] = (uint8_t)((pair_table_pos - START_CODES_SIZE) / 3);
	pair_table[1] = master_code;
	printf("\nCreated two_byte table size: %d", pair_table_pos);
}

unsigned int find_code_for_pair(unsigned char ch1, unsigned char ch2) {
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

bool is_code(unsigned char ch) {
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



void two_byte_pack_internal(const char* src, const char* dest, int pass) {

	printf("\nTwo-byte pack pass=%d", pass);

	unsigned long long offset, max_seq_len = (code_occurred ? 257 : 258),
		min_seq_len = 3;

	unsigned long char_freq[256] = { 0 }, best_seq_len;
	buffer_startpos = 0;
	buffer_min = max_seq_len * 2;
	unsigned int min_runlength = 3;

	infil = fopen(src, "rb");
	if (!infil) {
		printf("Hittade inte infil: %s", src);
		getchar();
		exit(1);
	}
	unsigned long total_size = get_file_size(infil);

	if (pass == 2) {
		fopen_s(&utfil, dest, "wb");
		if (!utfil) {
			puts("Hittade inte utfil!%s", dest); getchar(); exit(1);
		}
		// start compression!

		//write the metadata table
		for (int i = 0; i < pair_table_pos; i++) {

			fwrite(&pair_table[i], 1, 1, utfil);
		}
		printf("\n pair_table_pos: %d", pair_table_pos);
	}
	else {  // pass = 1
		for (int i = 0; i < 65536; i++) {
			two_byte_freq_table[i] = 0;
		}
	}

	/* start compression */

	buffer_endpos = fread(buffer, 1, buffer_size, infil);

	unsigned long long offset_max;
	unsigned int seq_len = 1;

	while (buffer_startpos < buffer_endpos) {

		unsigned char ch1 = buffer[buffer_startpos];
		unsigned char ch2 = buffer[buffer_startpos + 1];

		if (pass == 2) {
			unsigned int code = find_code_for_pair(ch1, ch2);
			if (code == 256) {
				// not found
				if (is_code(ch1)) {
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
		inc_char_freq(ch1);
		move_buffer(1);
	}
}//end while


if (pass == 1) {

	create_two_byte_table();
}
else {
	fclose(utfil);

}
fclose(infil);
}


void two_byte_pack(const char* src, const char* dest)
{
	buffer = (unsigned char*)malloc(buffer_size * sizeof(unsigned char));
	two_byte_pack_internal(src, dest, 1); //analyse and build meta-data
	two_byte_pack_internal(src, dest, 2); //pack
}



