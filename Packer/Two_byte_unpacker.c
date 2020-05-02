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
static unsigned char two_byte_table[2048], master_code;
static unsigned int two_byte_table_pos;


int get_two_byte_for_code(unsigned char code) {
	for (int i = START_CODES_SIZE; i < two_byte_table_pos; i += 3) {
		if (code == two_byte_table[i]) {
			return two_byte_table[i + 1] + 256 * two_byte_table[i + 2];
	
		}
	}
	return -1;
}

//--------------------------------------------------------------------------------------------------------



void two_byte_unpack_internal(const char* src, const char* dest) {

	printf("\nTwo-byte unpack...");


	fopen_s(&infil, src, "rb");
	if (!infil) {
		printf("\nHittade inte utfil: %s", src);
		getchar();
		exit(1);
	}
	fopen_s(&utfil, dest, "wb");
	if (!utfil) {
		puts("Hittade inte utfil! %s", dest);
		getchar();
		exit(1);
	}
	fread(&two_byte_table_pos, 1, 1, infil);
	fread(&master_code, 1, 1, infil);
	two_byte_table_pos *= 3;
	
	fread(&two_byte_table[START_CODES_SIZE], 1, (size_t)two_byte_table_pos, infil);
	two_byte_table_pos += START_CODES_SIZE;

	unsigned char cc;
	while (fread(&cc, 1, 1, infil) == 1) {

		if (cc == master_code) {
			fread(&cc, 1, 1, infil);
			WRITE(utfil, cc);
		}
		else {
			int two_byte = get_two_byte_for_code(cc);
			if (two_byte == -1) {
				WRITE(utfil, cc);
			}
			else {
				WRITE(utfil, two_byte % 256);
				WRITE(utfil, two_byte / 256);
			}
		}
	}
	fclose(utfil);
	fclose(infil);
}


void two_byte_unpack(const char* src, const char* dest)
{	
	two_byte_unpack_internal(src, dest); //analyse and build meta-data	
}



