#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include "memfile.h"
#include "common_tools.h"
#include "packer_commons.h"


uint32_t getMemPos(memfile* m) {
	return m->pos;
}



void syncMemSize(memfile* m) {
	if (m->pos > m->size) {
		m->size = m->pos;
	}
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

void checkAlloc(memfile* mf, int allocSizeNeeded) {
	if (allocSizeNeeded > (mf->allocSize)) {		
		reallocMem(mf, allocSizeNeeded);
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
	 return (mf->pos < mf->size) ? mf->block[mf->pos++] : EOF;	
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

int fputcc(int c, memfile* mf) {		
	checkAlloc(mf, mf->pos + 1);
	mf->block[mf->pos++] = c;	
	mf->size = mf->pos;
	return c;
}

int fputccLight(int c, memfile* mf) {
	if (mf->pos >= mf->allocSize) {
		reallocMem(mf, mf->allocSize + 16384);
	}
	return (mf->block[mf->pos++] = c);
}

memRead(uint8_t* arr, uint32_t size, memfile* m) {	
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
	uint64_t size = src->size;		
	if (dst->block == NULL) {
		dst->block = malloc(size);
	}
	else {
		checkAlloc(dst, size);
	}
	dst->pos = src->pos;
	dst->size = size;
	wcscpy(dst->name, src->name);
	uint8_t* src_block = src->block;
	for (int i = 0; i < size; i++) {
		dst->block[i] = src_block[i];
	}
}

bool memsEqual(memfile* m1, memfile* m2) {
	if (m1->size != m2->size) {
		return false;
	}
	uint8_t* m2_block = m2->block;
	for (int i = 0; i < m1->size; i++) {
		if (m1->block[i] != m2_block[i]) {
			return false;
		}
	}
	return true;
}

void copy_chunk_mem(memfile* source, memfile* dest, uint64_t size) {
	checkAlloc(dest, dest->pos + size);
	uint8_t* source_ar = source->block;
	for (int i = 0; i < size; i++) {
		dest->block[dest->pos++] = source_ar[source->pos++];
	}
	syncMemSize(dest);
}

//writes the whole of "append_file" into "main_file"'s current position
void append_to_mem(memfile* dest, memfile* append_file) {
		
	uint8_t* ar = append_file->block;
	uint64_t size = getMemSize(append_file);
	checkAlloc(dest, dest->pos + size);
	for (int i = 0; i < size; i++) {
		dest->block[dest->pos++] = ar[i];
	}	
	syncMemSize(dest);
}


// writes the rest of file "in" into file "dest"'s current position
void copy_the_rest_mem(memfile* in, memfile* dest) {

	uint64_t size = getMemSize(in) - getMemPos(in);
	checkAlloc(dest, dest->pos + size);
	uint8_t* in_block = in->block;
	for (int i = 0; i < size; i++) {
		dest->block[dest->pos++] = in_block[in->pos++];
	}
	syncMemSize(dest);
}


void append_mem_to_file(FILE* main_file, memfile* append_file) {
	uint64_t size = getMemSize(append_file);
	uint8_t* ar = append_file->block;
	for (int i=0; i<size; i++) {	
		fputc(ar[i], main_file);
	}
}

void copy_chunk_to_mem(FILE* source_file, memfile* dest, uint64_t size) {
	int ch;
	checkAlloc(dest, dest->pos + size);
	uint64_t i = 0;
	while (i < size && (ch = fgetc(source_file)) != EOF) {
		dest->block[dest->pos + (i++)] = ch;
	}
	setPos(dest, dest->pos + i);
	syncMemSize(dest);
}



