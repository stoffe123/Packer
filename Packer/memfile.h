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

void fputcc(int c, memfile* mf);

void memfileToFile(memfile* m, const wchar_t* dst);

void fre(memfile* mf);

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

void shallowCopyMem(memfile* src, memfile* dst);

void deepCopyMem(memfile* src, memfile* dst);

bool memsEqual(memfile* m1, memfile* m2);

#endif