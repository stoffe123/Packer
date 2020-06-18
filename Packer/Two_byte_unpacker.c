#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#define VERBOSE false
#include "common_tools.h"
#define START_CODES_SIZE 2

/* Two-byte unpacker */

//Global variables used in compressor
static FILE* infil, * utfil;

static unsigned char two_byte_table[16384] = { 0 }, master_code;
static int two_byte_table_pos;

int get_two_byte_for_code(unsigned char code) {
	for (int i = START_CODES_SIZE; i < two_byte_table_pos; i += 3) {
		if (code == two_byte_table[i]) {
			return two_byte_table[i + 1] + 256 * two_byte_table[i + 2];
	
		}
	}
	return -1;
}

//--------------------------------------------------------------------------------------------------------

void two_byte_unpack_internal(const wchar_t* src, const wchar_t* dest) {

	wprintf(L"\nTwo-byte unpack of %s", src);

	errno_t err = _wfopen_s(&infil, src, L"rb");
	if (err != 0) {
		wprintf(L"\nTwo byte unpack: Hittade inte infil: %s", src);
		getchar();
		exit(1);
	}
	err = _wfopen_s(&utfil, dest, L"wb");
	if (err != 0) {
		wprintf(L"\nTwo byte unpack: Hittade inte utfil! %s", dest);
		getchar();
		exit(1);
	}
	two_byte_table_pos = fgetc(infil);
	master_code = fgetc(infil);
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

void two_byte_unpack(const char* src, const char* dest) {	
	wchar_t d[500], s[500];
	toUni(d, dest);
	toUni(s, src);
	two_byte_unpack_internal(s, d);
}

void twoByteUnpack(const wchar_t* src, const wchar_t* dest)
{	
	two_byte_unpack_internal(src, dest); 
}



