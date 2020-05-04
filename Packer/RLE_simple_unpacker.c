#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "seq_unpacker.h"
#include "common_tools.h"
#include "RLE_simple_packer_commons.h"
#include "seq_packer_commons.h"

// RLE simple  - unpacker

//Global vars
static unsigned char code;
static FILE* infil, * utfil, * seq_lens_file, * runlengths_file;
static long long read_packedfile_pos,	
	runlengths_file_pos;
static const char* base_dir;
static bool separate;

static void write_byte_to_file(unsigned char cc) {
	putc(cc, utfil);
}

static void put_buf(unsigned char cc) {
	putc(cc, utfil);
}

static unsigned char read_runlength() {
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
void RLE_simple_unpack_internal(const char* source_filename, const char* dest_filename)
{
	bool code_occurred = 1;

	if (separate) {
		runlengths_file = fopen(concat(base_dir, "runlengths"), "rb");	
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

	code_occurred = read_runlength();
	fread(&code, 1, 1, infil);

	unsigned char cc;
	while (fread(&cc, 1,1, infil) == 1) {
		
		if (cc == code) {
			
			unsigned int runlength = read_runlength();
			
			if (runlength == 255 && code_occurred) {
				//occurrence of code in original
				put_buf(code);
			}
			else {
				int br = fread(&cc, 1, 1, infil);
				assert(br == 1, "br == 1  in RLE_simple_unpacker.RLE_simple_unpack");
				for (i = 0; i < (runlength + MIN_RUNLENGTH); i++) {
					put_buf(cc); 
				}
			}
		}
		else {
			put_buf(cc);
		}
	}
	fclose(infil);
	fclose(utfil);
	if (separate) {
		fclose(runlengths_file);
	}
}

void RLE_simple_unpack(const char* src, const char* dest) {
	separate = false;
	RLE_simple_unpack_internal(src, dest);
}

void RLE_simple_unpack_separate(const char* src, const char* dest, const char* bd) {
	base_dir = bd;
	separate = true;
	RLE_simple_unpack_internal(src, dest);
}
