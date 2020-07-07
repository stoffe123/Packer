#ifndef PACKER_COMMONS_H
#define PACKER_COMMONS_H

#include "common_tools.h"
#include <stdbool.h>

//3 bytes can handle block sizes up to 16777216‬

#define BLOCK_SIZE 3500000

//  16777215  is largest number for 24 bits in multipacker.tar
static bool DOUBLE_CHECK_PACK = false;


typedef struct val_freq_t {
	uint64_t value;
	uint64_t freq;
} val_freq_t;

typedef struct fileListAndCount_t {
	file_t* fileList;
	uint64_t count;
	uint64_t allocSize;
} fileListAndCount_t;

typedef struct value_freq_t {
	uint8_t value;
	uint64_t freq;
} value_freq_t;


typedef struct packProfile {
	int64_t rle_ratio;
	int64_t twobyte_ratio;
	int64_t recursive_limit;
	int64_t twobyte_threshold_max;
	int64_t twobyte_threshold_divide;
	int64_t twobyte_threshold_min;
	int64_t seqlenMinLimit3;
	int64_t seqlenMinLimit4;
	int64_t blockSizeMinus;
	int64_t winsize;
	int64_t sizeMaxForCanonicalHeaderPack;
	int64_t sizeMinForSeqPack;
	int64_t sizeMinForCanonical;
	int64_t sizeMaxForSuperslim;
} packProfile;


void lockTempfileMutex();

void releaseTempfileMutex();

void quickSortCompareEndings(file_t* f, uint64_t size);

void printProfile(packProfile*);

bool testPack(const char* src, const char* tmp, const char* packerName, int limit);

void copyProfile(packProfile* source, packProfile* dest);

uint64_t CanonicalEncodeAndTest(const char*);

bool MultiPackAndTest(const char* src, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile, packProfile distancesProfile);

value_freq_t find_best_code(unsigned long* char_freq);

packProfile getPackProfile();

void doDoubleCheck(const char* src, const char* packedName, const char* kind);

bool packAndTest(const char* kind, const char* src, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile, packProfile distancesProfile);

void unpackByKind(const char* kind, const char* tmp, const char* tmp2);

#endif