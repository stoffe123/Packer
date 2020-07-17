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

/* Halfbyte RLE packer */

__declspec(thread) static int globalByte, globalPos;

void flushWriteHalfbyte(memfile* file) {
	//tricky part here if we get a trailing value
		//we want that trailing value to be a code
		//so we can ignore it in the unpacker
	if (globalPos == 1) fputccLight((uint64_t)15 + globalByte, file);
}

void initWriteHalfbyte() {
	globalPos = 0;
}


void writeHalfbyte(memfile* file, int halfbyte)
{
	if (globalPos == 0)
	{
		globalByte = halfbyte * 16;
		globalPos = 1;
	}
	else
	{
		fputccLight(halfbyte + globalByte, file);
		globalPos = 0;
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

   code3 extra for escaping chars

*/

memfile* halfbyte_rle_pack_internal(memfile* infil, int kind) {

	rewindMem(infil);
	unsigned char code1 = 15;
	unsigned char code2 = 14;
	unsigned char code3 = 13;

	int max_runlength = 0, lowest_code = 0;

	if (kind == 0) {
		max_runlength = 273;
		lowest_code = 14;
	}
	else if (kind == 1) {
		max_runlength = 17;
		lowest_code = 15;
	}
	else if (kind == 2) {
		max_runlength = 273;
		lowest_code = 13;
	}
	else {
		printf("\n wrong kind in halfbyte_rle_pack"); exit(1);
	}
	//printf("\n canonical header pack %s", src);

	memfile* utfil = getMemfile((uint64_t)200 + infil->size, L"halfbyterlepack.utfil");
	
	initWriteHalfbyte();

	/* start compression */

	int runlength = 1;

	int read_char = fgetcc(infil);
	while (read_char != EOF) {

		unsigned char first_char = read_char;
		runlength = 1;

		if (kind < 2) {

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
		}
		else {
			// kind == 2

			if (read_char < 16) {

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
						writeHalfbyte(utfil, code3);
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

		}
	}//end while
	flushWriteHalfbyte(utfil);
	syncMemSize(utfil);
	return utfil;
}

memfile* halfbyteRlePack(memfile* mem, int kind) {
	return halfbyte_rle_pack_internal(mem, kind);
}

