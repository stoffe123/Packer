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
		printf("Hittade inte infil: %s", src);
		getchar();
		exit(1);
	}
	utfil = openWrite(dest);
	if (!utfil) {
		printf("Hittade inte utfil!%s", dest); getchar(); exit(1);
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
	append_to_filew(utfil, file1name);
	append_to_filew(utfil, file2name);
	_wremove(file1name);
	_wremove(file2name);
	fclose(infil);
	fclose(utfil);
}

void everyOtherDecode(const wchar_t* src, const wchar_t* dest) {

	FILE* infil = NULL, * utfil = NULL;

	infil = openRead(src);
	if (!infil) {
		printf("Hittade inte infil: %s", src);
		getchar();
		exit(1);
	}
	utfil = openWrite(dest);
	if (!utfil) {
		printf("Hittade inte utfil!%s", dest); getchar(); exit(1);
	}
	uint64_t size = get_file_size_from_wname(src);
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
