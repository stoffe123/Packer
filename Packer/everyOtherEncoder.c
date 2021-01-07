#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

#include "common_tools.h"
#include "packer_commons.h"

#define VERBOSE false


/* Every other encoder  encodes  00 fb 00 fc 00 a5  => 00 00 00 fb fc a5  */

//--------------------------------------------------------------------------------------------------------

void everyOtherEncode(const wchar_t* src, const wchar_t* dest) {

	FILE* infil = NULL, * utfil = NULL;

	infil = openRead(src);
	if (!infil) {
		printf("Hittade inte infil: %ls", src);
		myExit();
		return;
	}
	utfil = openWrite(dest);
	if (!utfil) {
		printf("Hittade inte utfil: %ls", dest); 
		myExit();
		return;
	}
	const wchar_t file1name[500] = { 0 };
	get_temp_filew(file1name, L"everyotherencoder_file_a");

	const wchar_t file2name[500] = { 0 };
	get_temp_filew(file2name, L"everyotherencoder_file_b");

	FILE* file1 = openWrite(file1name), * file2 = openWrite(file2name);

	/* start encoding */

	bool everyOtherFlag = true;

	int read_char;
	while ((read_char = fgetc(infil)) != EOF) {

		if (everyOtherFlag) {
			putc(read_char, file2);
		}
		else {
			putc(read_char, file1);
		}
		everyOtherFlag = !everyOtherFlag;
	}
	fclose(file1);
	fclose(file2);
	appendFileToFile(utfil, file1name);
	appendFileToFile(utfil, file2name);
	_wremove(file1name);
	_wremove(file2name);
	fclose(infil);
	if (utfil != NULL) {
		fclose(utfil);	
	}
}

void everyOtherDecode(const wchar_t* src, const wchar_t* dest) {
	
	FILE* infil = openRead(src);
	if (infil == NULL || infil == 0) {
		printf("Hittade inte infil: %ls", src);
		myExit();
		return;
	}
	FILE* utfil = openWrite(dest);
	if (utfil == NULL || utfil == 0) {
		printf("Hittade inte utfil!%ls", dest);
		myExit();
		return;
	}
	uint64_t size = getFileSizeFromName(src);
	uint64_t pos1 = 0, pos2 = size / 2;

	// start with pos2

	bool everyOtherFlag = true;

	int read_char;
	while (true) {

		if (everyOtherFlag) {
			fseek(infil, pos2++, SEEK_SET);
		}
		else {
			fseek(infil, pos1++, SEEK_SET);
		}
		int ch = fgetc(infil);
		if (ch == EOF) {
			break;
		}
		putc(ch, utfil);
		everyOtherFlag = !everyOtherFlag;
	}
	fclose(infil);
	fclose(utfil);
}
