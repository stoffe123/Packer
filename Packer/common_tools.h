#ifndef COMMON_TOOLS_H 
#define COMMON_TOOLS_H
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define debug if(VERBOSE) printf

#define math_max(x,y) ((x) >= (y)) ? (x) : (y)
#define math_min(x,y) ((x) <= (y)) ? (x) : (y)

#define BLOCK_SIZE 2510700

uint64_t get_file_size(const FILE* f);

uint64_t get_file_size_from_name(const char* name);

unsigned char isKthBitSet(unsigned char value, unsigned char bit);

unsigned char setKthBit(unsigned char value, unsigned char bit);

unsigned char clearKthBit(unsigned char value, unsigned char bit);

void copy_file(const char* src, const char* dst);

void copy_chunk(FILE* source_file, const char* dest_filename, uint64_t size_to_copy);

void copy_the_rest(FILE* source_file, const char* dest_filename);

void append_to_file(FILE* main_file, const char* append_filename);

void assert(uint64_t x, const char* msg);

void assertSmallerOrEqual(uint64_t x, uint64_t y, const char* msg);

void assertEqual(uint64_t x, uint64_t y, const char* msg);

void WRITE(FILE* ut, uint64_t c);

void concat(const char* dst, const char* s1, const char* s2);

void concat3(const char* dst, const char* s1, const char* s2, const char* s3);

void concat_int(const char* dst, const char* s1, int i);

void int_to_string(const char* dst, int64_t i);

void get_rand(const char* dst);

void get_temp_file2(const char* dst, const char* s);

void get_clock_dir(const char* dst);

bool files_equal(const char* source_filename, const char* dest_filename);

#endif