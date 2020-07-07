#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "common_tools.h"


#define VERBOSE false
#define START_CODES_SIZE 2

/* Two-byte unpacker */

int get_two_byte_for_code(unsigned char code, uint8_t* two_byte_table, int two_byte_table_pos) {
	for (int i = START_CODES_SIZE; i < two_byte_table_pos; i += 3) {
		if (code == two_byte_table[i]) {
			return two_byte_table[i + 1] + 256 * two_byte_table[i + 2];

		}
	}
	return -1;
}

//--------------------------------------------------------------------------------------------------------

void two_byte_unpack_internal(const wchar_t* src, const wchar_t* dest) {

	uint8_t* two_byte_table = malloc(16384 * sizeof(uint8_t));
	uint8_t master_code;
	int two_byte_table_pos;

	wprintf(L"\nTwo-byte unpack of %s", src);

	FILE* infil = openRead(src),
		* utfil = openWrite(dest);

	two_byte_table_pos = fgetc(infil);
	master_code = fgetc(infil);
	two_byte_table_pos *= 3;

	fread(&two_byte_table[START_CODES_SIZE], 1, (size_t)two_byte_table_pos, infil);
	two_byte_table_pos += START_CODES_SIZE;

	unsigned char cc;
	while (fread(&cc, 1, 1, infil) == 1) {

		if (cc == master_code) {
			fread(&cc, 1, 1, infil);
			fputc(cc, utfil);
		}
		else {
			int two_byte = get_two_byte_for_code(cc, two_byte_table, two_byte_table_pos);
			if (two_byte == -1) {
				fputc(cc, utfil);
			}
			else {
				fputc(two_byte % 256, utfil);
				fputc(two_byte / 256, utfil);
			}
		}
	}
	free(two_byte_table);
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



