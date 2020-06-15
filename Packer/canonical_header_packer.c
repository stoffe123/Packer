#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

#include "common_tools.h"
#include "packer_commons.h"

#define VERBOSE false

#define MIN_RUNLENGTH 2

/* Canonical header packer */

void writeHalfbyte(FILE* file, int halfbyte)
{
	static int byte, pos;
	switch (halfbyte)
	{
	case -1:  /* initialize */
		pos = 0;
		break;
	case -2:  /* flush */
		if (pos == 1) WRITE(file, byte);
		break;
	default:
		if (pos == 0)
		{
			byte = halfbyte * 16;
			pos = 1;
		}
		else
		{
			WRITE(file, halfbyte + byte);
			pos = 0;
		}
	}
}

//--------------------------------------------------------------------------------------------------------

value_freq_t canonical_header_pack_internal(const char* src, const char* dest) {

	unsigned char code = 15;

	FILE* infil = NULL, * utfil = NULL;
	unsigned long char_freq[256] = { 0 };
	unsigned long long max_runlength = 17;

	infil = fopen(src, "rb");
	if (!infil) {
		printf("Hittade inte infil: %s", src);
		getchar();
		exit(1);
	}
	fopen_s(&utfil, dest, "wb");
	if (!utfil) {
		printf("Hittade inte utfil!%s", dest); getchar(); exit(1);
	}
	// start compression!
	writeHalfbyte(utfil, -1); // init

	/* start compression */

	unsigned int runlength = 1;

	int read_char = fgetc(infil);
	while (read_char != EOF) {

		unsigned char first_char = read_char;
		runlength = 1;
		while ((read_char = fgetc(infil)) != EOF && runlength < max_runlength && read_char == first_char) {
			runlength++;
		}

		/* now we found the longest runlength in the window! */

		assert(runlength > 0, "runlength > 0 in RLE_simple_packer.RLE_pack_internal");
		if (runlength < MIN_RUNLENGTH) {
			
				for (int i = 0; i < runlength; i++) {
					if (first_char == code || first_char >=15) {
						//has to escape this char!
						writeHalfbyte(utfil, code);
						writeHalfbyte(utfil, 15); //15 is signal for escape!
						WRITE(utfil, first_char); //write full byte!
					}
					else {
						writeHalfbyte(utfil, first_char);
					}
				}
		}
		else { // Runlength found!	
				writeHalfbyte(utfil, code);
				writeHalfbyte(utfil, first_char);
				writeHalfbyte(utfil, runlength - MIN_RUNLENGTH);
		}
	}//end while
	writeHalfbyte(utfil, -2); // flush
	fclose(infil);
	fclose(utfil);
}

int canonical_header_pack(const char* src, const char* dest)
{
	canonical_header_pack_internal(src, dest); 
	return 0;
}



/*
DWORD WINAPI thread_RLE_simple_pack(LPVOID lpParam)
{
	char test_filenames[16][100] = { "bad.cdg","repeatchar.txt", "onechar.txt", "empty.txt",  "oneseq.txt", "book_med.txt","book.txt",
			 "amb.dll",
			 "rel.pdf",
			 "nex.doc",
			"did.csh",
			 "aft.htm",
			 "tob.pdf",
		 "pazera.exe",
		"voc.wav",
		 "bad.mp3"



	};
	uint32_t i = *(DWORD*)lpParam;

	printf("OOOOOOOOOOOOOOOOO The parameter: %d.\n", i);

	printf("filename=%s", test_filenames[i]);

	const char* src = concat("c:/test/testsuite/", test_filenames[i]);

	RLE_simple_pack(src, concat(src, "_packed"));

}
*/