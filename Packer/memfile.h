#ifndef MEMFILE_H
#define MEMFILE_H

#include "common_tools.h"
#include <stdbool.h>

typedef struct {
	uint32_t pos;
	uint32_t size;
	uint64_t allocSize;
	wchar_t name[100];
	uint8_t* block;
} memfile;

memfile* getMemfile(uint64_t allocSize, const wchar_t* name);

memfile* getMemfileFromFile(const wchar_t* src);

int fgetcc(memfile* mf);

int fputcc(int c, memfile* mf);

void memfileToFile(memfile* m, const wchar_t* dst);

void freeMem(memfile* mf);

void rewindMem(memfile* m);

uint32_t getMemPos(memfile* m);

uint32_t setPos(memfile* m, uint32_t p);



uint32_t incPos(memfile* m);

uint32_t getMemSize(memfile* m);

const wchar_t* getMemName(memfile* m);

uint32_t setSize(memfile* m, uint32_t s);

uint8_t* getBlock(memfile* m);

bool eofcc(memfile* mf);

int nextcc(memfile* mf);

memRead(uint8_t* arr, uint32_t size, memfile* m);

memWrite(uint8_t* arr, uint32_t size, memfile* m);

void reallocMem(memfile* mf, uint64_t size);

uint8_t getCCAtPos(memfile* m, uint64_t pos);

memfile* getEmptyMem(const wchar_t* name);

void deepCopyMem(memfile* src, memfile* dst);

bool memsEqual(memfile* m1, memfile* m2);

void copy_chunk_mem(memfile* source, memfile* dest, uint64_t size);

void append_to_mem(memfile* main_file, memfile* append_file);

void append_mem_to_file(FILE* main_file, memfile* append_file);

void copy_chunk_to_mem(FILE* source_file, memfile* dest_filename, uint64_t size_to_copy);

void copy_the_rest_mem(memfile* in, memfile* dest);

void syncMemSize(memfile* m);

int fputccLight(int c, memfile* mf);

int fput2ccLight(int c, memfile* mf);

int fget2cc(memfile* mf);

void copy_the_rest_to_mem(FILE* in, memfile* out);

#endif