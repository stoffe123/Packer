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
		//tricky part here if we get a trailing value
		//we want that trailing value to be a code
		//so we can ignore it in the unpacker
		if (pos == 1) WRITE(file, (uint64_t)15 + byte);
		break;
	default:
		if (pos == 0)
		{
			byte = halfbyte * 16;
			pos = 1;
		}
		else
		{
			WRITE(file, (uint64_t)halfbyte + byte);
			pos = 0;
		}
	}
}

//--------------------------------------------------------------------------------------------------------

value_freq_t canonical_header_pack_internal(const char* src, const char* dest) {

	unsigned char code = 15;
	unsigned char code2 = 14;

	FILE* infil = NULL, * utfil = NULL;
	unsigned long long max_runlength = 273;

	printf("\n canonical_header_pack: %s", src);

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

	int runlength = 1;

	int read_char = fgetc(infil);
	while (read_char != EOF) {

		unsigned char first_char = read_char;
		runlength = 1;

		if (read_char < 16 && read_char != code) {

			while ((read_char = fgetc(infil)) != EOF && runlength < max_runlength && read_char == first_char) {
				runlength++;
			}
		}
		else {
			read_char = fgetc(infil);
		}
		if (runlength < MIN_RUNLENGTH) {
			
				for (int i = 0; i < runlength; i++) {
					if (first_char == code || first_char >=14) {
						//has to escape this char!
						writeHalfbyte(utfil, code);
						writeHalfbyte(utfil, code); //code here is signal for escape!
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
					writeHalfbyte(utfil, code);
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