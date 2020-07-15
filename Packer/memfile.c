#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include "memfile.h"
#include "common_tools.h"
#include "packer_commons.h"


uint32_t getMemPos(memfile* m) {
	return m->pos;
}

void reallocMem(memfile* mf, uint64_t newAllocSize) {
	assert(newAllocSize >= mf->size, "wrong argument in memfile.reallocMem");	
	assert(newAllocSize > 0, "size=0 in memfile.c reallocmem");
	assert(newAllocSize < BLOCK_SIZE * 6, "size was too large in memfile.reallocMem");
	mf->allocSize = newAllocSize;
	uint8_t* pt = realloc(mf->block, mf->allocSize);
	if (pt != NULL) {
		mf->block = pt;
	}
	else {
		printf("\n out of memory in memfile.c checkAlloc allocSize=%d memfile %s", mf->allocSize, mf->name);
		exit(1);
	}
}

void checkAlloc(memfile* mf, int pos) {
	if (pos >= (mf->allocSize)) {
		reallocMem(mf, mf->allocSize + 8192);
	}
}

uint32_t setPos(memfile* m, uint32_t p) {
		checkAlloc(m, p);
	
		m->pos = p;
	
}

uint32_t incPos(memfile* m) {
	setPos(m, m->pos + 1);
}

uint32_t getMemSize(memfile* m) {
	if (m == NULL) {
		printf("\n  WARNING  getMemSize called with null argument! returning 0");
		return 0;
	}
	return m->size;
}

uint32_t setSize(memfile* m, uint32_t s) {	
	m->size = s;
}

uint8_t* getBlock(memfile* m) {
	return m->block;
}

memfile* getMemfile(uint64_t allocSize, const wchar_t* name) {
	memfile* m = malloc(sizeof(memfile));
	m->size = 0;
	m->pos = 0;
	m->block = malloc(allocSize);
	m->allocSize = allocSize;
	wcscpy(m->name, name);
	return m;
}

const wchar_t* getMemName(memfile* m) {
	/*
	if (m == NULL) {
		printf("\nWARNING getMemName was called with m=null");
		return "";
	}
	if (m != NULL) {
	*/
		return m->name;
	//}
	//return NULL;
}

void freeMem(memfile* mf) {
	if (mf != NULL) {
		free(mf->block);		
		free(mf);
	}
}

uint8_t getCCAtPos(memfile* m, uint64_t pos) {
	if (pos >= m->size) {
		printf("\n getCCAtPos out of range! %d memfile %s", pos, m->name);
		exit(1);
	}
	return m->block[pos];
}

void memfileToFile(memfile* mf, const wchar_t* dst) {
	FILE* out = openWrite(dst);
	uint32_t size = getMemSize(mf);	
	fwrite(getBlock(mf), 1, size, out);
	fclose(out);
}

void rewindMem(memfile* m) {
	setPos(m, 0);
}

memfile* getMemfileFromFile(const wchar_t* filename) {
	uint64_t fileSize = getFileSizeFromName(filename);	
	memfile* m = getMemfile(fileSize + 1, filename);
	FILE* infil = openRead(filename);
	uint64_t size = fread(getBlock(m), 1, fileSize, infil);
	fclose(infil);
	if (size > BLOCK_SIZE) {
		printf("\n error too large file %s in packer_commons.getMemfileFromFile", filename);
	}
	m->size = size;
	return m;
}

int fgetcc(memfile* mf) {
	if (getMemPos(mf) >= getMemSize(mf)) {
		return EOF;
	}
	else {
		int res = mf->block[getMemPos(mf)];
		incPos(mf);		
		return res;
	}
}

int nextcc(memfile* mf) {
	if (getMemPos(mf) >= getMemSize(mf)) {
		return EOF;
	}
	else {
		int res = mf->block[getMemPos(mf)];
		return res;
	}
}

bool eofcc(memfile* mf) {
	return getMemPos(mf) >= getMemSize(mf);
}

void fputcc(int c, memfile* mf) {		
	checkAlloc(mf, mf->pos + 1);
	mf->block[mf->pos] = c;
	incPos(mf);
	mf->size = mf->pos;
}

memRead(uint8_t* arr, uint32_t size, memfile* m) {
	checkAlloc(m, m->pos + size);
	for (int i = 0; i < size; i++) {
		arr[i] = m->block[m->pos + i];		
	}
	setPos(m, m->pos + size);
}

memWrite(uint8_t* arr, uint32_t size, memfile* m) {
	checkAlloc(m, m->pos + size);
	for (int i = 0; i < size; i++) {
		m->block[m->pos + i] = arr[i];
	}
	setPos(m, m->pos + size);
	if (m->pos > m->size) {
		setSize(m, m->pos);
	}
}

memfile* getEmptyMem(const wchar_t* name) {
	return getMemfile(0, name);
}

void deepCopyMem(memfile* src, memfile* dst) {
	if (src == dst) {
		return;
	}
	if (dst->allocSize < src->size) {
		dst->allocSize = src->size;
	}	
	dst->pos = src->pos;
	dst->size = src->size;
	wcscpy(dst->name, src->name);
	free(dst->block);
	dst->block = malloc(src->size);
	for (int i = 0; i < src->size; i++) {
		dst->block[i] = src->block[i];
	}
}

bool memsEqual(memfile* m1, memfile* m2) {
	if (m1->size != m2->size) {
		return false;
	}
	for (int i = 0; i < m1->size; i++) {
		if (m1->block[i] != m2->block[i]) {
			return false;
		}
	}
	return true;
}