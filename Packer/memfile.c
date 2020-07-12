#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include "memfile.h"
#include "common_tools.h"
#include "packer_commons.h"


uint32_t getPos(memfile* m) {
	return m->pos;
}

void reallocMem(memfile* mf, uint64_t size) {
	assert(size > mf->size, "wrong argument in memfile.c reallocmem");	
	mf->allocSize = size;
	uint8_t* pt = realloc(mf->block, mf->allocSize);
	if (pt != NULL) {
		mf->block = pt;
	}
	else {
		printf("\n out of memory in memfile.c checkAlloc");
		exit(1);
	}
}

void checkAlloc(memfile* mf, int pos) {
	if (pos >= mf->allocSize - 1) {
		reallocMem(mf, mf->allocSize + 8192);
	}
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

memfile* getMemfile(uint64_t allocSize) {
	memfile* m = malloc(sizeof(memfile));
	m->size = 0;
	m->pos = 0;
	m->block = malloc(allocSize);
	m->allocSize = allocSize;
	return m;
}

void fre(memfile* mf) {
	if (mf != NULL) {
		free(mf->block);		
		free(mf);
	}
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
	uint64_t fileSize = get_file_size_from_name(src);
	memfile* m = getMemfile(fileSize + 1);
	FILE* infil = fopen(src, "rb");
	uint32_t size = fread(getBlock(m), 1, fileSize, infil);
	fclose(infil);
	if (size > BLOCK_SIZE) {
		printf("\n error too large file %s in packer_commons.get_memfile_from_file", src);
	}
	m->size = size;
	return m;
}

memfile* getMemfileFromFile(const wchar_t* src) {
	uint64_t fileSize = get_file_size_from_wname(src);
	memfile* m = getMemfile(fileSize + 1);
	FILE* infil = openRead(src);
	uint32_t size = fread(getBlock(m), 1, fileSize, infil);
	fclose(infil);
	if (size > BLOCK_SIZE) {
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

int nextcc(memfile* mf) {
	if (getPos(mf) >= getSize(mf)) {
		return EOF;
	}
	else {
		int res = mf->block[getPos(mf)];
		return res;
	}
}

bool eofcc(memfile* mf) {
	return getPos(mf) >= getSize(mf);
}

void fputcc(int c, memfile* mf) {		
	checkAlloc(mf, mf->pos);
	mf->block[mf->pos] = c;
	incPos(mf);
	mf->size = mf->pos;
}

memRead(uint8_t* arr, uint32_t size, memfile* m) {
	checkAlloc(m, m->pos + size);
	for (int i = 0; i < size; i++) {
		arr[i] = m->block[m->pos + i];		
	}
	m->pos = m->pos + size;
}