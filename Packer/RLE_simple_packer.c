#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

#include "common_tools.h"
#include "packer_commons.h"
#include "RLE_simple_packer_commons.h"

#define VERBOSE false

/* RLE simple packer */


void write_runlength(unsigned char c, FILE* utfil, FILE* runlengths_file, bool separate) {
	if (separate) {
		fwrite(&c, 1, 1, runlengths_file);
	}
	else {
		fputc(c, utfil);
	}
}

//--------------------------------------------------------------------------------------------------------

value_freq_t  RLE_pack_internal(const char* src, const char* dest, int pass, value_freq_t code_struct, const char* base_dir, bool separate) {

	unsigned char code = 0;

	if (pass == 2) {
		code = code_struct.value;
		//code_occurred = code_struct.freq > 0;
	}

	FILE* infil = NULL, * utfil = NULL, * runlengths_file = NULL;
	unsigned long char_freq[256] = { 0 };
	debug("\nRLE_simple_pack pass=%d", pass);
	if (separate) {
		const char name[100] = { 0 };
		concat(name, base_dir, "runlengths");
		runlengths_file = fopen(name, "wb");
	}
	unsigned long long max_runlength = 254 + MIN_RUNLENGTH;

	infil = fopen(src, "rb");
	if (!infil) {
		printf("Hittade inte infil: %s", src);
		getchar();
		exit(1);
	}

	if (pass == 2) {
		fopen_s(&utfil, dest, "wb");
		if (!utfil) {
			printf("Hittade inte utfil!%s", dest); getchar(); exit(1);
		}
		// start compression!
		//write_runlength(code_occurred ? 1 : 0, utfil, runlengths_file);
		fputc(code, utfil);
	}
	else { // pass = 1
		//code_occurred = false;
	}

	/* start compression */

	unsigned int runlength = 1;

	int read_char = fgetc(infil);
	while (read_char != EOF) {

		if (pass == 1) {
			if (char_freq[read_char] < ULONG_MAX - runlength) {
				char_freq[read_char]++;
			}
		}
		unsigned char first_char = read_char;
		runlength = 1;
		while ((read_char = fgetc(infil)) != EOF && runlength < max_runlength && read_char == first_char) {
			runlength++;
		}

		/* now we found the longest runlength in the window! */

		assert(runlength > 0, "runlength > 0 in RLE_simple_packer.RLE_pack_internal");
		if (runlength < MIN_RUNLENGTH) {
			if (pass == 1) {
				if (char_freq[first_char] < ULONG_MAX - runlength) {
					char_freq[first_char]++;
				}
			}
			else { // pass = 2
				for (int i = 0; i < runlength; i++) {
					if (first_char == code) {
						fputc(code, utfil);
						fputc(255, utfil);
						//assert(code_occurred, "code_occured in RLE_simple_packer.RLE_pack_internal");
					}
					else {
						fputc(first_char, utfil);
					}
				}
			}
		}
		else { // Runlength fond!
			if (pass == 2) {
				fputc(code, utfil);
				write_runlength(runlength - MIN_RUNLENGTH, utfil, runlengths_file, separate);
				fputc(first_char, utfil);
			}
			else {
				//since the code appeared in a runlength no penalty needed!
				if (char_freq[first_char] > 0) {
					char_freq[first_char]--;
				}
			}
		}
	}//end while

	fclose(infil);

	if (pass == 1) {
		return find_best_code(char_freq);
	}
	else {
		fclose(utfil);
		if (separate) {
			fclose(runlengths_file);
		}
	}

}

void RLE_simple_pack_internal(const char* src, const char* dest, const char* base_dir, bool separate)
{
	value_freq_t dummy;
	dummy.freq = 0;
	dummy.value = 0;
	value_freq_t res = RLE_pack_internal(src, dest, 1, dummy, base_dir, separate); //find code
	RLE_pack_internal(src, dest, 2, res, base_dir, separate); //pack
}

void RLE_simple_pack(const char* src, const char* dest) {
	RLE_simple_pack_internal(src, dest, "", false);
}


void RLE_simple_pack_separate(const char* src, const char* dest, const char* base_dir) {
	RLE_simple_pack_internal(src, dest, base_dir, true);
}