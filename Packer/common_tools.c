#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "common_tools.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define TEMP_DIR "c:/test/temp_files/"

//Global for all threads
uint64_t filename_count = 1000000;

uint64_t get_file_size(const FILE* f) {
	fseek(f, 0, SEEK_END);
	long long res = ftell(f);
	fseek(f, 0, SEEK_SET);
	return res;
}

uint64_t get_file_size_from_name(const char* name) {
	const FILE* f = fopen(name, "r");
	if (f == NULL) {
		printf("\n get_file_size_from_name: can't find file: %s", name);
		exit(0);
	}
	long long res = get_file_size(f);
	fclose(f);
	return res;
}

uint64_t get_file_size_from_wname(wchar_t* name) {
	wprintf(L"\n get_file_size_from_wname:%s", name);
	FILE* f;
	errno_t err = _wfopen_s(&f, name, L"rb");
	if (err != 0) {
		wprintf(L"\n get_file_size_from_name: can't find file: %s", name);
		exit(0);
	}
	uint64_t res = get_file_size(f);
	fclose(f);
	return res;
}

unsigned char isKthBitSet(unsigned char value, unsigned char bit)
{
	return (value & (1 << bit));
}

unsigned char setKthBit(unsigned char value, unsigned char bit)
{
	// kth bit of n is being set by this operation 
	return ((1 << bit) | value);
}

unsigned char clearKthBit(unsigned char value, unsigned char bit)
{
	// kth bit of n is being set by this operation 
	return (value & (~(1 << bit)));
}


void copy_chunk(FILE* source_file, const char* dest_filename, uint64_t size_to_copy) {
	FILE* out = fopen(dest_filename, "wb");

	uint8_t ch;
	for (int i = 0; i < size_to_copy; i++) {
		size_t bytes_got = fread(&ch, 1, 1, source_file);
		if (bytes_got == 0) {
			break;
		}
		fwrite(&ch, 1, 1, out);
	}
	fclose(out);
}

void copy_chunkw(FILE* source_file, wchar_t* dest_filename, uint64_t size_to_copy) {
	FILE* out;
	errno_t err = _wfopen_s(&out, dest_filename, L"wb");
	if (err != 0) {
		wprintf(L"\n copy_chunkw couldn't open file %s", dest_filename);
		exit(1);
	}
	uint8_t ch;
	for (int i = 0; i < size_to_copy; i++) {
		size_t bytes_got = fread(&ch, 1, 1, source_file);
		if (bytes_got == 0) {
			break;
		}
		fwrite(&ch, 1, 1, out);
	}
	fclose(out);
}

void copy_the_rest(FILE* in, const char* dest_filename) {
	FILE* out = fopen(dest_filename, "wb");
	uint8_t ch;
	while (fread(&ch, 1, 1, in) == 1) {
		fwrite(&ch, 1, 1, out);
	}
	fclose(out);
	fclose(in);
}

void copy_file(const char* src, const char* dst) {
	FILE* f = fopen(src, "rb");
	copy_the_rest(f, dst);
	fclose(f);
}

void append_to_file(FILE* main_file, const char* append_filename) {
	FILE* append_file = fopen(append_filename, "rb");
	int ch;
	while ((ch = fgetc(append_file)) != EOF) {
		fputc(ch, main_file);
	}
	fclose(append_file);
}

void append_to_filew(FILE* main_file, wchar_t* append_filename) {
	wprintf(L"\n append_to_filew: %s", append_filename);
	FILE* append_file; 	
	errno_t err = _wfopen_s(&append_file, append_filename, L"rb");
	int ch;
	while ((ch = fgetc(append_file)) != EOF) {
		fputc(ch, main_file);
	}
	fclose(append_file);
}

void assert(uint64_t x, const char* msg) {
	if (!x) {
		printf("\n\n\a ASSERTION FAILURE: %s", msg);
		exit(0);
	}
}

void assertEqual(uint64_t x, uint64_t y, const char* msg) {
	if (x != y) {
		printf("\n\n\a ASSERTION FAILURE: %d != %d \n %s", x, y, msg);
		exit(0);
	}
}

void assertSmallerOrEqual(uint64_t x, uint64_t y, const char* msg) {
	if (x > y) {
		printf("\n\n\a ASSERTION FAILURE: %d <= %d \n %s", x, y, msg);
		exit(0);
	}
}

void WRITE(FILE* ut, uint64_t c)
{
	fwrite(&c, 1, 1, ut);
}

void get_rand(const char* s) {
	int_to_string(s, filename_count++);
}

void get_temp_file2(const char* dst, const char* s) {
	char number[40] = { 0 };
	get_rand(number);
	concat3(dst, TEMP_DIR, s, number);
}

void get_clock_dir(const char* dir) {
	const char number[30] = { 0 };
	get_rand(number);
	concat(dir, TEMP_DIR, number);
}

void concat(const char* dst, const char* s1, const char* s2) {
	const size_t s1_length = strlen(s1);
	strcpy(dst, s1);
	strcpy(dst + s1_length, s2);
}

void concatw(wchar_t* dst, wchar_t* s1, wchar_t* s2) {
	wcscpy(dst, s1);
	wcscat(dst, s2);
}

void concat3(const char* dst, const char* s1, const char* s2, const char* s3) {
	const size_t s1_length = strlen(s1);
	const size_t s2_length = strlen(s2);
	strcpy(dst, s1);
	strcpy(dst + s1_length, s2);
	strcpy(dst + s1_length + s2_length, s3);
}

void concat_int(const char* dst, const char* s1, int i) {
	const char number[30] = { 0 };
	int_to_string(number, i);
	concat(dst, s1, number);
}

void int_to_string(const char* s, int64_t i) {
	_itoa(i, s, 10);
}

bool files_equal(const char* source_filename, const char* dest_filename) {
	FILE* f1, * f2;

	fopen_s(&f1, source_filename, "rb");
	if (!f1) {
		printf("\nHittade inte utfil: %s", source_filename);
		getchar();
		exit(1);
	}

	fopen_s(&f2, dest_filename, "rb");
	if (!f2) {
		puts("Hittade inte utfil!");
		getchar();
		exit(1);
	}
	long f1_size = get_file_size(f1);
	long f2_size = get_file_size(f2);
	int res = 1;
	if (f1_size != f2_size) {
		printf("\n\a >>>>>>>>>>>> FILES NOT EQUAL!!!! <<<<<<<<<<<<<<<< %s and %s", source_filename, dest_filename);
		printf("\n Lengths differ   %d  %d", f1_size, f2_size);
		res = 0;
	}
	unsigned char tmp1, tmp2;

	size_t bytes = 0;
	int count = 0;
	while (!feof(f1) && !feof(f2)) {
		fread(&tmp1, 1, 1, f1);
		fread(&tmp2, 1, 1, f2);

		if (tmp1 != tmp2) {

			printf("\n Contents differ at position  %d ", count);
			printf("\n File1:");
			printf("%c", tmp1);
			//print_string_rep(tmp1);
			printf("\n File2:");
			//print_string_rep(tmp2);
			printf("%c", tmp2);
			return 0;
		}
		count++;
	}
	fclose(f1);
	fclose(f2);
	return res;
}


bool files_equalw( wchar_t* source_filename,  wchar_t* dest_filename) {
	FILE* f1, * f2;

	errno_t err = _wfopen_s(&f1, source_filename, L"rb");
	if (err != 0) {
		wprintf(L"\n files_equalw: Hittade inte fil1: '%s'\n", source_filename);
		getchar();
		exit(1);
	}

	err = _wfopen_s(&f2, dest_filename, L"rb");
	if (err != 0) {
		wprintf(L"\n files_equalw: Hittade inte file2: '%s'\n", dest_filename);
		getchar();
		exit(1);
	}
	long f1_size = get_file_size(f1);
	long f2_size = get_file_size(f2);
	int res = 1;
	if (f1_size != f2_size) {
		printf("\n\a >>>>>>>>>>>> FILES NOT EQUAL!!!! <<<<<<<<<<<<<<<< %s and %s", source_filename, dest_filename);
		printf("\n Lengths differ   %d  %d", f1_size, f2_size);
		res = 0;
	}
	unsigned char tmp1, tmp2;

	size_t bytes = 0;
	int count = 0;
	while (!feof(f1) && !feof(f2)) {
		fread(&tmp1, 1, 1, f1);
		fread(&tmp2, 1, 1, f2);

		if (tmp1 != tmp2) {

			printf("\n Contents differ at position  %d ", count);
			printf("\n File1:");
			printf("%c", tmp1);
			//print_string_rep(tmp1);
			printf("\n File2:");
			//print_string_rep(tmp2);
			printf("%c", tmp2);
			return 0;
		}
		count++;
	}
	fclose(f1);
	fclose(f2);
	return res;
}
