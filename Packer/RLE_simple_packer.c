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

/* Global variables used in compressor */
static const char* base_dir;
static bool separate;

static value_freq_t find_best_code2(unsigned long* char_freq) {
	unsigned char best_code;
	unsigned long freq = ULONG_MAX;
	for (unsigned int i = 0; i < 256; i++) {
		if (char_freq[i] < freq) {
			freq = char_freq[i];
			best_code = i;
		}
	}//end for

	printf("\n Found code: %d that occured: %d times.", best_code, freq);

	char_freq[best_code] = ULONG_MAX; // mark it as used!
	value_freq_t res;
	res.value = best_code;
	res.freq = freq;
	return res;
}



void write_runlength(unsigned char c, FILE* utfil, FILE* runlengths_file) {
	if (separate) {
		fwrite(&c, 1, 1, runlengths_file);
	}
	else {
		WRITE(utfil, c);
	}
}


//--------------------------------------------------------------------------------------------------------

value_freq_t  RLE_pack_internal(const char* src, const char* dest, int pass, value_freq_t code_struct, const char* base_dir) {

	unsigned char code = 0;
	bool code_occurred = true;
	if (pass == 2) {
		code = code_struct.value;
		code_occurred = code_struct.freq > 0;
	}

	FILE* infil = NULL, * utfil = NULL, * runlengths_file = NULL;
	unsigned long char_freq[256] = { 0 };
	debug("\nRLE_simple_pack pass=%d", pass);
	if (separate) {
		const char name[100] = { 0 };
		concat(name, base_dir, "runlengths");
		runlengths_file = fopen(name, "wb");
	}
	unsigned long long max_runlength = (code_occurred ? 254 + MIN_RUNLENGTH : 255 + MIN_RUNLENGTH);

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
		write_runlength(code_occurred ? 1 : 0, utfil, runlengths_file);
		WRITE(utfil, code);
	}
	else { // pass = 1
		code_occurred = false;
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
						WRITE(utfil, code);
						WRITE(utfil, 255);
						assert(code_occurred, "code_occured in RLE_simple_packer.RLE_pack_internal");
					}
					else {
						WRITE(utfil, first_char);
					}
				}
			}
		}
		else { // Runlength fond!
			if (pass == 2) {
				WRITE(utfil, code);
				write_runlength(runlength - MIN_RUNLENGTH, utfil, runlengths_file);
				WRITE(utfil, first_char);
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
		//skip best code, save that for seqlen packer
		find_best_code2(char_freq);
		
		return find_best_code2(char_freq);
	}
	else {
		fclose(utfil);
		if (separate) {
			fclose(runlengths_file);
		}
	}

}


void RLE_simple_pack_internal(const char* src, const char* dest, const char* base_dir)
{
	value_freq_t dummy;
	dummy.freq = 0;
	dummy.value = 0;
	value_freq_t res = RLE_pack_internal(src, dest, 1, dummy, base_dir); //find code
	RLE_pack_internal(src, dest, 2, res, base_dir); //pack
}

void RLE_simple_pack(const char* src, const char* dest) {
	separate = false;
	RLE_simple_pack_internal(src, dest, "");
}


void RLE_simple_pack_separate(const char* src, const char* dest, const char* base_dir) {	
	separate = true;
	RLE_simple_pack_internal(src, dest, base_dir);
}


/*
DWORD WINAPI thread_RLE_simple_pack(LPVOID lpParam)
{
	char test_filenames[16][100] = { "bad.cdg","repeatchar.txt", "onechar.txt", "empty.txt",  "oneseq.txt", "book_med.txt","book.txt",
			 "amb.dll",
			 "rel.pdf",
			 "nex.doc",
			"did.csh",
			 "aft.htm",
			 "tob.pdf",
		 "pazera.exe",
		"voc.wav",
		 "bad.mp3"



	};
	uint32_t i = *(DWORD*)lpParam;

	printf("OOOOOOOOOOOOOOOOO The parameter: %d.\n", i);

	printf("filename=%s", test_filenames[i]);

	const char* src = concat("c:/test/testsuite/", test_filenames[i]);

	RLE_simple_pack(src, concat(src, "_packed"));

}
*/