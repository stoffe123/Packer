#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common_tools.h"
#include "memfile.h"

#define MIN_RUNLENGTH 2

// canonical header unpacker

//Global vars

__declspec(thread) static  long long read_packedfile_pos;

__declspec(thread) static int globalByte, globalPos;

int readHalfbyte(memfile* infil, int cmd)
{
	if (cmd == -1)
	{
		/* initialize */
		globalPos = 0;
		return 0;   /* the return value here is not used */
	}
	else
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
}

//------------------------------------------------------------------------------
memfile* halfbyte_rle_unpack_internal(memfile* infil)
{

	//printf("\n canonical_header_unpack: %s", source_filename);
	unsigned long i;

	unsigned char code = 15, code2 = 14;

	memfile* utfil = getMemfile();
	readHalfbyte(infil, -1); // init

	int cc;
	while ((cc = readHalfbyte(infil, 0)) != -1) {

		if (cc == code || cc == code2) {
			int byte = readHalfbyte(infil, 0);
			if (byte == -1) {
				//encountered a trailing value at end of file => ignore
				break;
			}
			if (byte == code && cc == code) {
				//escape sequence
				unsigned int escapedByte = readHalfbyte(infil, 0) + 16 * readHalfbyte(infil, 0);
				fputcc(escapedByte, utfil);
			}
			else {
				int runlength = readHalfbyte(infil, 0);				
				runlength += (cc == code ? MIN_RUNLENGTH : (18 + 16 * readHalfbyte(infil, 0)));				
				for (int i = 0; i < runlength; i++) {
					fputcc(byte, utfil);
				}
			}
		} 
		else {
			fputcc(cc, utfil);
		}
	}
	return utfil;
}

void halfbyte_rle_unpack(const char* src, const char* dest, int kind) {
	memfile* s = get_memfile_from_file(src);
	memfile* packed = halfbyte_rle_unpack_internal(s, kind);
	fre(s);
	memfile_to_file(packed, dest);
	fre(packed);
}

memfile* halfbyteRleUnpack(memfile* mem, int kind) {
	return halfbyte_rle_unpack_internal(mem, kind);
}
