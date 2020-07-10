#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include "memfile.h"
#include "common_tools.h"
#include "packer_commons.h"


uint32_t getPos(memfile* m) {
	return m->pos;
}

uint32_t setPos(memfile* m, uint32_t p) {
	m->pos = p;
}

uint32_t incPos(memfile* m) {
	m->pos++;
}

uint32_t getSize(memfile* m) {
	return m->size;
}

uint32_t setSize(memfile* m, uint32_t s) {
	m->size = s;
}

uint8_t* getBlock(memfile* m) {
	return m->block;
}

memfile* getMemfile() {
	memfile* m = malloc(sizeof(memfile));
	m->size = 0;
	m->pos = 0;
	m->block = malloc(BLOCK_SIZE);
	return m;
}

void fre(memfile* mf) {
	free(mf->block);
}



void memfileToFile(memfile* mf, const wchar_t* dst) {
	FILE* out = openWrite(dst);
	uint32_t size = getSize(mf);	
	fwrite(getBlock(mf), 1, size, out);
	fclose(out);
}

void memfile_to_file(memfile* mf, const char* dst) {
	FILE* out = fopen(dst, "wb");
	uint32_t size = getSize(mf);
	fwrite(getBlock(mf), 1, size, out);
	fclose(out);
}

void rewindMem(memfile* m) {
	setPos(m, 0);
}

memfile* get_memfile_from_file(const char* src) {
	memfile* m = getMemfile();
	FILE* infil = fopen(src, "rb");
	uint32_t size = fread(getBlock(m), 1, BLOCK_SIZE, infil);
	fclose(infil);
	if (size == BLOCK_SIZE) {
		printf("\n error too large file %s in packer_commons.get_memfile_from_file", src);
	}
	m->size = size;
	return m;
}

memfile* getMemfileFromFile(const wchar_t* src) {
	memfile* m = getMemfile();
	FILE* infil = openRead(src);
	uint32_t size = fread(getBlock(m), 1, BLOCK_SIZE, infil);
	fclose(infil);
	if (size == BLOCK_SIZE) {
		printf("\n error too large file %s in packer_commons.get_memfile_from_file", src);
	}
	m->size = size;
	return m;
}

int fgetcc(memfile* mf) {
	if (getPos(mf) >= getSize(mf)) {
		return EOF;
	}
	else {
		int res = mf->block[getPos(mf)];
		incPos(mf);
		return res;
	}
}

void fputcc(int c, memfile* mf) {	
	uint32_t pos = getPos(mf);
	mf->block[pos] = c;
	incPos(mf);
	mf->size = mf->pos;
}

