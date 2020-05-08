#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "common_tools.h"
#include <sys/types.h>
#include <sys/stat.h>

#define TEMP_DIR "c:/test/temp_files/"

//Global for all threads
unsigned long long filename_count = 1000000;


static const unsigned long BUF_SIZE = 32768;
static unsigned char buf[32768];


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

void copy_internal(FILE* source_file, const char* dest_filename, unsigned long long size_to_copy, bool copy_the_rest) {
	FILE* out = fopen(dest_filename, "wb");

	unsigned long long total_read = 0, bytes_to_read;
	size_t bytes_got;
	do {
		if (!copy_the_rest && total_read + BUF_SIZE > size_to_copy) {
			bytes_to_read = (size_to_copy - total_read);
		}
		else {
			bytes_to_read = BUF_SIZE;
		}
		bytes_got = fread(&buf, 1, bytes_to_read, source_file);
		if (bytes_got > 0) {
			fwrite(&buf, 1, bytes_got, out);
		}
		total_read += bytes_got;
	} while (bytes_got == BUF_SIZE);
	fclose(out);
}

void copy_chunk(FILE* source_file, const char* dest_filename, unsigned long long size_to_copy) {
	copy_internal(source_file, dest_filename, size_to_copy, false);
}

void copy_the_rest(FILE* source_file, const char* dest_filename) {
	copy_internal(source_file, dest_filename, 0, true);
}


void copy_file(const char* src, const char* dst) {
	FILE* f = fopen(src, "rb");
	copy_the_rest(f, dst);
	fclose(f);
}


void append_to_file(FILE* main_file, const char* append_filename) {
	FILE* append_file = fopen(append_filename, "rb");
	size_t bytes_got;
	do {
		bytes_got = fread(&buf, 1, BUF_SIZE, append_file);
		if (bytes_got > 0) {
			fwrite(&buf, 1, bytes_got, main_file);
		}
	} while (bytes_got == BUF_SIZE);
	fclose(append_file);
}

void assert(uint64_t x, const char* msg) {
	if (!x) {
		printf("\n\n ASSERTION FAILURE: %s", msg);		
		exit(0);
	}
}

void assertEqual(uint64_t x, uint64_t y, const char* msg) {
	if (x != y) {
		printf("\n\n ASSERTION FAILURE: %d != %d \n %s", x, y, msg);
		exit(0);
	}
}


void assertSmallerOrEqual(uint64_t x, uint64_t y, const char* msg) {
	if (x > y) {
		printf("\n\n ASSERTION FAILURE: %d <= %d \n %s", x,y, msg);
		exit(0);
	}
}

void WRITE(FILE* ut, unsigned long long c)
{
	fwrite(&c, 1, 1, ut);
}

char* get_rand() {
	return int_to_string(filename_count++);
}


char* get_temp_file() {
	return concat(TEMP_DIR, concat("tmp", get_rand()));
}




char* get_clock_dir() {
	const char* dir = concat(TEMP_DIR, get_rand());
	dir = concat(dir, "_");
	return dir;
}


char* concat(const char* s1, const char* s2) {

	const size_t s1_length = strlen(s1);
	const size_t totalLength = s1_length + strlen(s2);

	char* const strBuf = malloc(totalLength + 1);
	if (strBuf == NULL) {
		fprintf(stderr, "malloc failed\n");
		exit(EXIT_FAILURE);
	}
	strcpy(strBuf, s1);
	strcpy(strBuf + s1_length, s2);
	return strBuf;
}

char* concat_int(const char* s1, int i) {
	return concat(s1, int_to_string(i));
}

char* int_to_string(int64_t i) {
	const char* ra = malloc(100);
	_itoa(i, ra, 10);
	return ra;
}

void make_dir(const char* path) {
	
	struct stat st = { 0 };
	if (stat(path, &st) == -1) {
		mkdir(path, 0777);
	}
}
