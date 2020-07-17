#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common_tools.h"
#include "memfile.h"

#define MIN_RUNLENGTH 2

// Halfbyte RLE unpacker

// Global vars

__declspec(thread) static int globalByte, globalPos;

void initReadHalfbyte() {
	globalPos = 0;
}

int readHalfbyte(memfile* infil)
{
	if (globalPos == 0)
	{
		if ((globalByte = fgetcc(infil)) != EOF) {
			globalPos = 1;
			return(globalByte / 16);
		}
		return -1; // EOF						
	}
	else
	{
		globalPos = 0;
		return(globalByte % 16);
	}
}

//------------------------------------------------------------------------------
memfile* halfbyte_rle_unpack_internal(memfile* infil, int kind)
{
	rewindMem(infil);
	//printf("\n canonical_header_unpack: %s", source_filename);
	unsigned long i;

	unsigned char code1 = 15, code2 = 14, code3 = 13;

	memfile* utfil = getMemfile((uint64_t)2 * infil->size + 300, L"halfbyterleunpack.utfil");
	initReadHalfbyte();

	int cc;
	while ((cc = readHalfbyte(infil)) != -1) {

		if (kind < 2) {

			if (cc == code1 || (cc == code2 && kind == 0)) {
				int byte = readHalfbyte(infil);
				if (byte == -1) {
					//encountered a trailing value at end of file => ignore
					break;
				}
				if (byte == code1 && cc == code1) {
					//escape sequence
					unsigned int escapedByte = readHalfbyte(infil) + 16 * readHalfbyte(infil);
					fputccLight(escapedByte, utfil);
				}
				else {
					int runlength = readHalfbyte(infil);
					runlength += (cc == code1 ? MIN_RUNLENGTH : (18 + 16 * readHalfbyte(infil)));
					for (int i = 0; i < runlength; i++) {
						fputccLight(byte, utfil);
					}
				}
			}
			else {
				fputccLight(cc, utfil);
			}
		}
		else {

			// kind = 2

			if (cc == code1 || cc == code2 || cc == code3) {
				int byte = readHalfbyte(infil);
				if (byte == -1) {
					//encountered a trailing value at end of file => ignore
					break;
				}
				if (cc == code3) {
					//escape sequence
					int escapedByte = byte + 16 * readHalfbyte(infil);
					fputccLight(escapedByte, utfil);
				}
				else {
					int runlength = readHalfbyte(infil);
					runlength += (cc == code1 ? MIN_RUNLENGTH : (18 + 16 * readHalfbyte(infil)));
					for (int i = 0; i < runlength; i++) {
						fputccLight(byte, utfil);
					}
				}
			}
			else {
				fputccLight(cc, utfil);
			}
		}
	}
	syncMemSize(utfil);
	return utfil;
}

memfile* halfbyteRleUnpack(memfile* mem, int kind) {
	return halfbyte_rle_unpack_internal(mem, kind);
}
