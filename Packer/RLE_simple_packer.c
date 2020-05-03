#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include "seq_packer.h"
#include "seq_packer_commons.h"
#define VERBOSE false
#include "common_tools.h"

/* RLE simple packer */

//global variables used in compressor
static FILE* infil, * utfil;
static unsigned char code;
static unsigned long long buffer_endpos, buffer_startpos, buffer_min, buffer_size = 2048;
static unsigned char* buffer;
static bool code_occurred = false;
static FILE* runlengths_file;
static unsigned long long char_freq[256] = { 0 };
static const char* base_dir;
static bool separate;


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

static unsigned char find_best_code() {
	unsigned char best_code;
	unsigned long freq = ULONG_MAX;
	for (unsigned int i = 0; i < 256; i++) {
		if (char_freq[i] < freq) {
			freq = char_freq[i];
			best_code = i;
		}
	}//end for

	printf("\n Found code: %d that occured: %d times.", best_code, freq);

	// char_freq[best] = ULONG_MAX; // mark it as used!
	code_occurred = (freq > 0);
	return best_code;
}



void write_runlength(unsigned char c) {
	if (separate) {
		fwrite(&c, 1, 1, runlengths_file);
	}
	else {
		WRITE(utfil, c);
	}
}

void inc_char_freq(unsigned char c) {
	if (char_freq[c] < ULONG_MAX) {
		char_freq[c]++;
	}
}


//--------------------------------------------------------------------------------------------------------



void RLE_pack_internal(const char* src, const char* dest, int pass) {

	printf("\nRLE_simple_pack pass=%d" , pass);
	if (separate) {
		runlengths_file = fopen(concat(base_dir, "runlengths"), "wb");
	}
	unsigned long long offset, max_runlength = (code_occurred ? 257 : 258);
		
	unsigned long char_freq[256] = { 0 }, best_runlength;
	buffer_startpos = 0;
	buffer_min = max_runlength*2;
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
		write_runlength(code_occurred ? 1 : 0);
		WRITE(utfil, code);
	} else { // pass = 1
		code_occurred = false;
	}

	/* start compression */

	buffer_endpos = fread(buffer, 1, buffer_size, infil);

	unsigned long long offset_max;
	unsigned int runlength = 1;

	while (buffer_startpos < buffer_endpos) {

		best_runlength = 2;
		unsigned char ch = buffer[buffer_startpos];
		if (pass == 2) {

			runlength = 1;
			while (runlength < max_runlength && 
				buffer_startpos + runlength < buffer_endpos && 
				ch == buffer[buffer_startpos + runlength]) {
				runlength++;
			}
		}
		/* now we found the longest runlength in the window! */

		assert(runlength > 0, "runlength > 0 in RLE_simple_packer.RLE_pack_internal");
		if (runlength < min_runlength) {
			if (pass == 1) {
				inc_char_freq(buffer[buffer_startpos]);
				if (runlength == 2) {
					inc_char_freq(buffer[buffer_startpos + 1]);
				}
			}
			else { // pass = 2
				for (int i = 0; i < runlength; i++) {
					unsigned char c = buffer[buffer_startpos + i];
					if (c == code) {
						WRITE(utfil, code);
						WRITE(utfil, 255);
						assert(code_occurred, "code_occured in RLE_simple_packer.RLE_pack_internal");
					}
					else {
						WRITE(utfil, c);
					}
				}
			}
			move_buffer(runlength);
		}
		else { // Runlength fond!
			if (pass == 2) {
				WRITE(utfil, code);
				write_runlength(runlength - min_runlength);
				WRITE(utfil, ch);
			}
			move_buffer(runlength);
		}
	}//end while


	if (pass == 1) {
		code = find_best_code();
	}
	else {
		fclose(utfil);
		if (separate) {
			fclose(runlengths_file);
		}
	}
	fclose(infil);
}


void RLE_simple_pack_internal(const char* src, const char* dest)
{
	buffer = (unsigned char*)malloc(buffer_size * sizeof(unsigned char));
	RLE_pack_internal(src, dest, 1); //find code
	RLE_pack_internal(src, dest, 2); //pack
}

void RLE_simple_pack(const char* src, const char* dest) {
	separate = false;
	RLE_simple_pack_internal(src, dest);
}


void RLE_simple_pack_separate(const char* src, const char* dest, const char* bd) {
	base_dir = bd;
	separate = true;
	RLE_simple_pack_internal(src, dest);
}
