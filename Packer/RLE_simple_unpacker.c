#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common_tools.h"
#include "RLE_simple_packer_commons.h"

// RLE simple  - unpacker



unsigned char read_runlength(FILE* infil, FILE* runlengths_file, bool separate) {
	if (separate) {
		return fgetc(runlengths_file);
	}
	else {
		unsigned char c;
		fread(&c, 1, 1, infil);
		return c;
	}
}

//------------------------------------------------------------------------------
void RLE_simple_unpack_internal(const char* source_filename, const char* dest_filename, const char* base_dir, bool separate)
{

	FILE* infil, * utfil, * runlengths_file = NULL;

	if (separate) {
		const char name[100] = { 0 };
		concat(name, base_dir, "runlengths");
		runlengths_file = fopen(name, "rb");
	}

	//printf("\n\n Unpacking %s", source_filename);
	unsigned long i;


	fopen_s(&infil, source_filename, "rb");
	if (!infil) {
		printf("\nHittade inte utfil: %s", source_filename);
		getchar();
		exit(1);
	}
	fopen_s(&utfil, dest_filename, "wb");
	if (!utfil) {
		puts("Hittade inte utfil!");
		getchar();
		exit(1);
	}

	//code_occurred = read_runlength(infil, runlengths_file);
	unsigned char code;
	fread(&code, 1, 1, infil);

	unsigned char cc;
	while (fread(&cc, 1, 1, infil) == 1) {

		if (cc == code) {

			unsigned int runlength = read_runlength(infil, runlengths_file, separate);

			if (runlength == 255) {
				//occurrence of code in original
				putc(code, utfil);
			}
			else {
				int br = fread(&cc, 1, 1, infil);
				assert(br == 1, "br == 1  in RLE_simple_unpacker.RLE_simple_unpack");
				for (i = 0; i < (runlength + MIN_RUNLENGTH); i++) {
					putc(cc, utfil);
				}
			}
		}
		else {
			putc(cc, utfil);
		}
	}
	fclose(infil);
	fclose(utfil);
	if (separate) {
		fclose(runlengths_file);
	}
}

void RLE_simple_unpack(const char* src, const char* dest) {
	RLE_simple_unpack_internal(src, dest, "", false);
}

void RLE_simple_unpack_separate(const char* src, const char* dest, const char* base_dir) {
	RLE_simple_unpack_internal(src, dest, base_dir, true);
}
