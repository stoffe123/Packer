#ifndef PACKER_COMMONS_H
#define PACKER_COMMONS_H

#include "common_tools.h"
#include <stdbool.h>
#include "memfile.h"

//3 bytes can handle block sizes up to 16777216‬

#define BLOCK_SIZE 8000000

//  16777215  is largest number for 24 bits in multipacker.tar
static bool DOUBLE_CHECK_PACK = false;

typedef struct {
	uint64_t value;
	uint64_t freq;
} val_freq_t;



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
	int64_t blockSizeMinus;
	int64_t winsize;
	int64_t sizeMaxForCanonicalHeaderPack;
	int64_t sizeMinForSeqPack;
	int64_t sizeMinForCanonical;
	int64_t sizeMaxForSuperslim;
	int64_t archiveType;
} packProfile;

typedef struct completePackProfile {
	packProfile main;
	packProfile seqlen;
	packProfile offset;
	packProfile distance;
} completePackProfile;

void lockTempfileMutex();

void releaseTempfileMutex();

void quickSortCompareEndings(file_t* f, uint64_t size);

void quickSortOnSizes(file_t* f, uint64_t size);

void printProfile(packProfile*);

void fprintProfile(FILE* file, packProfile* profile);

bool testPack(memfile* src, memfile* tmp, const wchar_t* packerName, int limit);

void copyProfile(packProfile* source, packProfile* dest);

completePackProfile cloneCompleteProfile(completePackProfile src);

value_freq_t find_best_code(unsigned long* char_freq);

packProfile getPackProfile();

void doDoubleCheck(memfile* src, memfile* packedName, const wchar_t* kind);

bool packAndTest(const wchar_t* kind, memfile* src, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile, packProfile distancesProfile);

memfile* unpackByKind(const wchar_t* kind, memfile* tmp);

void writeDynamicSize16or64(uint64_t s, FILE* out);

void writeDynamicSize32or64(uint64_t s, FILE* out);

uint64_t readDynamicSize16or64(FILE* in);

uint64_t readDynamicSize32or64(FILE* in);

completePackProfile getCompletePackProfile(packProfile main, packProfile seqlen, packProfile offset, packProfile distance);

#endif