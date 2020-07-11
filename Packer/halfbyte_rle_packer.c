#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

#include "common_tools.h"
#include "packer_commons.h"
#include "memfile.h"

#define VERBOSE false

#define MIN_RUNLENGTH 2

/* Canonical header packer */

__declspec(thread) static int globalByte, globalPos;

void writeHalfbyte(memfile* file, int halfbyte)
{

	switch (halfbyte)
	{
	case -1:  /* initialize */
		globalPos = 0;
		break;
	case -2:  /* flush */
		//tricky part here if we get a trailing value
		//we want that trailing value to be a code
		//so we can ignore it in the unpacker
		if (globalPos == 1) fputcc((uint64_t)15 + globalByte, file);
		break;
	default:
		if (globalPos == 0)
		{
			globalByte = halfbyte * 16;
			globalPos = 1;
		}
		else
		{
			fputcc((uint64_t)halfbyte + globalByte, file);
			globalPos = 0;
		}
	}
}

//--------------------------------------------------------------------------------------------------------
/*
code1 = 15 used for short runlengths and escape

code2 = 14 used for long runlengths

kind = 0

	escape chars >=14   code1, code1,  low, high   (2 bytes)

	runlength < 18 code1, char, length    (1.5 bytes)

	runlength >= 18  code2, char, low, high  (2 bytes)

kind = 1

	only one code1 and only handle runlength < 18




kind = 2

*/

memfile* canonical_header_pack_internal(memfile* infil, int kind) {

	unsigned char code1 = 15;
	unsigned char code2 = 14;

	int max_runlength = 0, lowest_code = 0;

	if (kind == 0) {
		max_runlength = 273;
		lowest_code = 14;
	}
	else if (kind == 1) {
		max_runlength = 17;
		lowest_code = 15;
	}
	//printf("\n canonical header pack %s", src);

	memfile* utfil = getMemfile();
	// start compression!
	writeHalfbyte(utfil, -1); // init

	/* start compression */

	int runlength = 1;

	int read_char = fgetcc(infil);
	while (read_char != EOF) {

		unsigned char first_char = read_char;
		runlength = 1;

		if (read_char < 16 && read_char != code1) {

			while ((read_char = fgetcc(infil)) != EOF && runlength < max_runlength && read_char == first_char) {
				runlength++;
			}
		}
		else {
			read_char = fgetcc(infil);
		}
		if (runlength < MIN_RUNLENGTH) {

			for (int i = 0; i < runlength; i++) {
				if (first_char >= lowest_code) {
					//has to escape this char!
					writeHalfbyte(utfil, code1);
					writeHalfbyte(utfil, code1); //code here is signal for escape!
					writeHalfbyte(utfil, first_char % 16);
					writeHalfbyte(utfil, first_char / 16);
				}
				else {
					writeHalfbyte(utfil, first_char);
				}
			}
		}
		else { // Runlength found!		
			if (runlength < 18) {
				writeHalfbyte(utfil, code1);
				writeHalfbyte(utfil, first_char);
				writeHalfbyte(utfil, runlength - MIN_RUNLENGTH);
			}
			else {
				writeHalfbyte(utfil, code2);
				writeHalfbyte(utfil, first_char);
				runlength -= 18;
				writeHalfbyte(utfil, runlength % 16);
				writeHalfbyte(utfil, runlength / 16);
			}
		}
	}//end while
	writeHalfbyte(utfil, -2); // flush
	return utfil;
}

void halfbyte_rle_pack(const char* src, const char* dest, int kind)
{
	memfile* s = get_memfile_from_file(src);
	memfile* packed = canonical_header_pack_internal(s, kind);
	fre(s);
	memfile_to_file(packed, dest);
	fre(packed);
}

memfile* halfbyteRlePack(memfile* mem, int kind) {
	return canonical_header_pack_internal(mem, kind);
}

