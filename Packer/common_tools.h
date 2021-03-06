#ifndef COMMON_TOOLS_H 
#define COMMON_TOOLS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#define debug if(VERBOSE) printf

#define math_max(x,y) ((x) >= (y)) ? (x) : (y)
#define math_min(x,y) ((x) <= (y)) ? (x) : (y)

#define TEMP_DIR "c:/test/temp_files/"
#define TEMP_DIRW L"c:/test/temp_files/"



typedef struct file_t {
	uint64_t size;
	wchar_t name[2000]; //TODO maybe change to dynamically assigned memory to reduce memory usage
	uint64_t equalSizeNumber;
} file_t;

void quickSortCompareEndings(file_t* f, uint64_t size);

void quickSort(file_t* f, uint64_t size);

void myRename(const wchar_t* f_old, const wchar_t* f_new);

size_t to_narrow(const wchar_t* src, char* dest);

void deleteAllFilesInDir(const wchar_t* dir);

bool contains(const wchar_t* s, const wchar_t* find);

uint64_t indexOfChar(const wchar_t* s, const wchar_t find);

unsigned char isKthBitSet(unsigned char value, unsigned char bit);

unsigned char setKthBit(unsigned char value, unsigned char bit);

unsigned char clearKthBit(unsigned char value, unsigned char bit);

unsigned char setKthBitToVal(unsigned char value, unsigned char bit, unsigned char bitValue);

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

void get_randw(const wchar_t* dst);

void toUni(const wchar_t* dst, const char* string);

bool equals(const char* s1, const char* s2);

bool equalsw(const wchar_t* s1, const wchar_t* s2);

bool equalsNormalAndUni(char* s1, wchar_t* s2);

bool equalsIgnoreCase(const wchar_t* str1, const wchar_t* str2);

void substringAfterLast(const wchar_t* dst, const wchar_t* s, const wchar_t* find);

void myExit();

uint8_t getByteAtPos(uint64_t bytes, int pos);

void setByteAtPos(uint64_t* bytes, uint8_t byte, int pos);

void substring(wchar_t* dst, const wchar_t* src, uint64_t m, uint64_t n);

void concatSubstring(wchar_t* dst, const wchar_t* src, uint64_t m, uint64_t n);


#endif