#ifndef COMMON_TOOLS_H 
#define COMMON_TOOLS_H
#include <stdbool.h>
#define debug if(VERBOSE) printf("\n");if(VERBOSE) printf

long long get_file_size(const FILE* f);

long long get_file_size_from_name(const char* name);

unsigned char isKthBitSet(unsigned char value, unsigned char bit);

unsigned char setKthBit(unsigned char value, unsigned char bit);

void copy_file(const char* src, const char* dst);

void copy_chunk(FILE* source_file, const char* dest_filename, unsigned long long size_to_copy);

void copy_the_rest(FILE* source_file, const char* dest_filename);

void append_to_file(FILE* main_file, const char* append_filename);

#endif