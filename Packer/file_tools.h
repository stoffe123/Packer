#ifndef FILE_TOOLS_H 
#define FILE_TOOLS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "common_tools.h"

#define MAX_EXTENSION_LENGTH 10

typedef struct fileListAndCount_t {
	file_t* fileList;
	uint64_t count;
	uint64_t allocSize;
} fileListAndCount_t;

fileListAndCount_t storeDirectoryFilenames(const wchar_t* dir, bool storeSizes);

bool filesEqual(wchar_t* name1, wchar_t* name2);

void getFileExtension(const wchar_t* dst, const wchar_t* sourceStr);

FILE* openWrite(const wchar_t* filename);

FILE* openRead(const wchar_t* filename);

void copyFileChunkToFile(FILE* source_file, wchar_t* dest_filename, uint64_t size_to_copy);

void copyTheRest(FILE* source_file, wchar_t* dest_filename);

void appendFileToFile(FILE* main_file, wchar_t* append_filename);

uint64_t getFileSizeFromName(wchar_t* name);

uint64_t getFileSize(const FILE* f);

uint64_t getSizeLeftToRead(const FILE* f);

void copyFile(wchar_t* src, wchar_t* dst);

void createMissingDirs(wchar_t* fullPath, wchar_t* existingDir);

void get_temp_filew(const wchar_t* dst, const wchar_t* s);

FILE* openTempFile(const wchar_t* dst, const wchar_t* s);

#endif
