#ifndef COMMON_TOOLS_H 
#define COMMON_TOOLS_H
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define debug if(VERBOSE) printf

#define math_max(x,y) ((x) >= (y)) ? (x) : (y)
#define math_min(x,y) ((x) <= (y)) ? (x) : (y)

#define TEMP_DIR "c:/test/temp_files/"
#define TEMP_DIRW L"c:/test/temp_files/"


uint64_t get_file_size(const FILE* f);

uint64_t get_file_size_from_name(const char* name);

uint64_t get_file_size_from_wname(wchar_t* name);

unsigned char isKthBitSet(unsigned char value, unsigned char bit);

unsigned char setKthBit(unsigned char value, unsigned char bit);

unsigned char clearKthBit(unsigned char value, unsigned char bit);

void copy_file(const char* src, const char* dst);

void copy_chunk(FILE* source_file, const char* dest_filename, uint64_t size_to_copy);

void copy_chunkw(FILE * source_file, wchar_t* dest_filename, uint64_t size_to_copy);

void copy_the_rest(FILE* source_file, const char* dest_filename);

void append_to_file(FILE* main_file, const char* append_filename);

void append_to_filew(FILE * main_file, wchar_t* append_filename);

void assert(uint64_t x, const char* msg);

void assertSmallerOrEqual(uint64_t x, uint64_t y, const char* msg);

void assertEqual(uint64_t x, uint64_t y, const char* msg);

void WRITE(FILE* ut, uint64_t c);

void concat(const char* dst, const char* s1, const char* s2);

void concatw(wchar_t* dst, wchar_t* s1, wchar_t* s2);

void concat3(const char* dst, const char* s1, const char* s2, const char* s3);

void concat3w(wchar_t* dst, wchar_t* s1, wchar_t* s2, wchar_t* s3);
	
void concat4w(wchar_t* dst, wchar_t* s1, wchar_t* s2, wchar_t* s3, wchar_t* s4);
	
void concat_int(const char* dst, const char* s1, int i);

void concat_intw(const wchar_t* dst, const wchar_t* s1, int i);

void int_to_string(const char* dst, int64_t i);

void int_to_stringw(const wchar_t* dst, int64_t i);

void get_rand(const char* dst);

void get_randw(const wchar_t* dst);

void getTempFile(const char* dst, const char* s);

void get_temp_filew(const wchar_t* dst, const wchar_t* s);

void get_clock_dir(const char* dst);

bool files_equal(const char* source_filename, const char* dest_filename);

bool files_equalw(wchar_t* source_filename, wchar_t* dest_filename);

void toUni(const wchar_t* dst, const char* string);

FILE* openWrite(const wchar_t* filename);

FILE* openRead(const wchar_t* filename);

bool equals(const char* s1, const char* s2);

bool equalsw(const wchar_t* s1, const wchar_t* s2);

#endif