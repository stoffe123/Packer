#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common_tools.h"

#define MIN_RUNLENGTH 2

// canonical header unpacker

//Global vars

static   long long read_packedfile_pos;

int readHalfbyte(FILE* infil, int cmd)
{
	static int byte, pos;
	if (cmd == -1)
	{
		/* initialize */
		pos = 0;
		return 0;   /* the return value here should is not used */
	}
	else
	{
		if (pos == 0)
		{
			int res = fread(&byte, 1, 1, infil);
			if (res == 0) {
				return -1; // EOF
			}
			pos = 1;
			return(byte / 16);
		}
		else
		{
			pos = 0;
			return(byte % 16);

		}
	}
	return 1;
}

//------------------------------------------------------------------------------
void canonical_header_unpack_internal(const char* source_filename, const char* dest_filename)
{

	FILE* infil, * utfil;

	//printf("\n canonical_header_unpack: %s", source_filename);
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

	unsigned char code = 15, code2 = 14;
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
				putc(escapedByte, utfil);
			}
			else {
				int runlength = readHalfbyte(infil, 0);				
				runlength += (cc == code ? MIN_RUNLENGTH : (18 + 16 * readHalfbyte(infil, 0)));				
				for (int i = 0; i < runlength; i++) {
					putc(byte, utfil);
				}
			}
		} 
		else {
			putc(cc, utfil);
		}
	}
	fclose(infil);
	fclose(utfil);	
}

int canonical_header_unpack(const char* src, const char* dest) {
	canonical_header_unpack_internal(src, dest);
	return 0;
}
