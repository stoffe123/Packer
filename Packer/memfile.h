#ifndef MEMFILE_H
#define MEMFILE_H

#include "common_tools.h"
#include <stdbool.h>

typedef struct {
	uint32_t pos;
	uint32_t size;
	uint8_t* block;
} memfile;

memfile* getMemfile();

memfile* get_memfile_from_file(const char* src);

memfile* getMemfileFromFile(const wchar_t* src);

int fgetcc(memfile* mf);

void fputcc(int c, memfile* mf);

void memfileToFile(memfile* m, const wchar_t* dst);

void memfile_to_file(memfile* m, const char* dst);

void fre(memfile* mf);

void rewindMem(memfile* m);

uint32_t getPos(memfile* m);

uint32_t setPos(memfile* m, uint32_t p);

uint32_t incPos(memfile* m);

uint32_t getSize(memfile* m);

uint32_t setSize(memfile* m, uint32_t s);

uint8_t* getBlock(memfile* m);

memRead(uint8_t* arr, uint32_t size, memfile* m);

#endif