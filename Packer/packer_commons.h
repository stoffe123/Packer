#ifndef PACKER_COMMONS_H
#define PACKER_COMMONS_H

#define BLOCK_SIZE 2110700

static bool DOUBLE_CHECK_PACK = false;

typedef struct value_freq_t {
	uint8_t value;
	uint64_t freq;
} value_freq_t;


typedef struct packProfile_t {
	int seqlen_pages;
	int offset_pages;
	int rle_ratio;
	int twobyte_ratio;
	int seq_ratio;
	int recursive_limit;
	int twobyte_threshold_max;
	int twobyte_threshold_divide;
	int twobyte_threshold_min;
} packProfile_t;

void printProfile(packProfile_t*);

void copyProfile(packProfile_t*, packProfile_t*);

void CanonicalDecodeAndReplace(const char*);

bool CanonicalEncodeAndTest(const char*);

bool SeqPackAndTest(const char* src, int seqlen_pages, int offset_pages, int ratio_limit);

bool MultiPackAndTest(const char* src, packProfile_t profile,
	packProfile_t seqlensProfile, packProfile_t offsetsProfile);

value_freq_t find_best_code(unsigned long* char_freq);

#endif