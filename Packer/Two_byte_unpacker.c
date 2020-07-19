#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "common_tools.h"
#include "memfile.h"


#define VERBOSE false

/* Two-byte unpacker */

int get_two_byte_for_code(unsigned char code, uint8_t* two_byte_table, uint16_t* values, int two_byte_table_pos) {
	for (int i = 0; i < two_byte_table_pos; i++) {
		if (code == two_byte_table[i]) {
			return values[i];
		}
	}
	return -1;
}

//--------------------------------------------------------------------------------------------------------

memfile* two_byte_unpack_internal(memfile* infil) {

	rewindMem(infil);
	memfile* utfil = getMemfile(infil->size * (uint64_t)2, L"twobyteunpack.utfil");

	uint8_t master_code = fgetcc(infil);
	int two_byte_table_size = fgetcc(infil);
	debug("\n twobyte unpack mastercode:%d  two_byte_table_size:%d\n", master_code, two_byte_table_size);

	uint8_t* two_byte_table = malloc(two_byte_table_size * sizeof(uint8_t));
	uint16_t* values = malloc(two_byte_table_size * sizeof(uint16_t));
	int lastCode = -1;
	for (int i = 0; i < two_byte_table_size; i++) {
		int c = fgetcc(infil);
		c = (c + lastCode) + 1;
		lastCode = c;
		debug("%d ", c);
		two_byte_table[i] = c;
}
	debug("\n");
	for (int i = 0; i < two_byte_table_size; i++) {
		values[i] = fget2cc(infil);
	}	
	int cc;
	while ((cc = fgetcc(infil)) != EOF) {

		if (cc == master_code) {
			cc = fgetcc(infil);
			fputccLight(cc, utfil);
		}
		else {
			int two_byte = get_two_byte_for_code(cc, two_byte_table, values, two_byte_table_size);
			if (two_byte == -1) {
				fputccLight(cc, utfil);
			}
			else {
				fput2ccLight(two_byte, utfil);			
			}
		}
	}
	free(two_byte_table);
	free(values);
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



