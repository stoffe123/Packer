#ifndef COMMON_TOOLS_H 
#define COMMON_TOOLS_H
#include <stdbool.h>
#define debug if(VERBOSE) printf("\n");if(VERBOSE) printf

#define math_max(x,y) ((x) >= (y)) ? (x) : (y)
#define math_min(x,y) ((x) <= (y)) ? (x) : (y)


long long get_file_size(const FILE* f);

long long get_file_size_from_name(const char* name);

unsigned char isKthBitSet(unsigned char value, unsigned char bit);

unsigned char setKthBit(unsigned char value, unsigned char bit);

void copy_file(const char* src, const char* dst);

void copy_chunk(FILE* source_file, const char* dest_filename, unsigned long long size_to_copy);

void copy_the_rest(FILE* source_file, const char* dest_filename);

void append_to_file(FILE* main_file, const char* append_filename);

void assert(int x, const char* msg);

void WRITE(FILE * ut, unsigned long long c);

char* concat(const char* s1, const char* s2);

void make_dir(const char* path);

char* int_to_string(int i);


#endif