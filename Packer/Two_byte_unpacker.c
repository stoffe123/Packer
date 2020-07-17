#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "common_tools.h"
#include "memfile.h"


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

memfile* two_byte_unpack_internal(memfile* infil) {

	rewindMem(infil);
	memfile* utfil = getMemfile(infil->size * 2, L"twobyteunpack.utfil");
	uint8_t* two_byte_table = malloc(16384 * sizeof(uint8_t));
	uint8_t master_code;
	int two_byte_table_size;

	two_byte_table_size = fgetcc(infil);
	master_code = fgetcc(infil);
	debug("\n mastercode:%d  two_byte_table_pos:%d", master_code, two_byte_table_size);
	two_byte_table_size *= 3;

	memRead(&two_byte_table[START_CODES_SIZE], two_byte_table_size, infil);
	two_byte_table_size += START_CODES_SIZE;

	int cc;
	while ((cc = fgetcc(infil)) != EOF) {

		if (cc == master_code) {
			cc = fgetcc(infil);
			fputccLight(cc, utfil);
		}
		else {
			int two_byte = get_two_byte_for_code(cc, two_byte_table, two_byte_table_size);
			if (two_byte == -1) {
				fputccLight(cc, utfil);
			}
			else {
				fputccLight(two_byte % 256, utfil);
				fputccLight(two_byte / 256, utfil);
			}
		}
	}
	free(two_byte_table);
	syncMemSize(utfil);
	return utfil;
}

void two_byte_unpackw(const wchar_t* src, const wchar_t* dest)
{
	memfile* m = getMemfileFromFile(src);
	memfile* d = two_byte_unpack_internal(m);
	freeMem(m);
	memfileToFile(d, dest);
	freeMem(d);
}

memfile* twoByteUnpack(memfile* m) {
	return two_byte_unpack_internal(m);
}



