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

long long get_file_size(const FILE* f) {
	fseek(f, 0, SEEK_END);
	long long res = ftell(f);
	fseek(f, 0, SEEK_SET);
	return res;
}

uint64_t get_file_size_from_name(const char* name) {
	const FILE* f = fopen(name, "r");
	if (f == NULL) {
		printf("\n Can't find file: %s", name);
		exit(0);
	}
	long long res = get_file_size(f);
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

void get_clock_dir(const char* dst) {
	const char number[30] = { 0 };
	get_rand(number);
	concat(dst, TEMP_DIR, number);
}

void concat(const char* dst, const char* s1, const char* s2) {
	const size_t s1_length = strlen(s1);
	strcpy(dst, s1);
	strcpy(dst + s1_length, s2);
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
	get_rand(number);
	concat(dst, s1, number);
}

void int_to_string(const char* s, int64_t i) {
	_itoa(i, s, 10);
}
