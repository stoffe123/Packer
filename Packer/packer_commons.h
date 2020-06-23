#ifndef PACKER_COMMONS_H
#define PACKER_COMMONS_H

#include <stdbool.h>

//3 bytes can handle block sizes up to 16777216‬

#define BLOCK_SIZE 3500000

//  16777215  is largest number for 24 bits in multipacker.tar
static bool DOUBLE_CHECK_PACK = false;


typedef struct val_freq_t {
	uint64_t value;
	uint64_t freq;
} val_freq_t;


typedef struct value_freq_t {
	uint8_t value;
	uint64_t freq;
} value_freq_t;


typedef struct packProfile {
	int rle_ratio;
	int twobyte_ratio;
	int recursive_limit;
	int twobyte_threshold_max;
	int twobyte_threshold_divide;
	int twobyte_threshold_min;
	int seqlenMinLimit3;
	int64_t blockSizeMinus;
	int64_t winsize;
	int64_t sizeMaxForCanonicalHeaderPack;
	int64_t sizeMinForSeqPack;
	int64_t sizeMinForCanonical;
} packProfile;

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