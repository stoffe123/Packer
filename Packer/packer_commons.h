#ifndef PACKER_COMMONS_H
#define PACKER_COMMONS_H

const char** get_test_filenames();


typedef struct packProfile_t {
	int seqlen_pages;
	int offset_pages;
	int rle_ratio;
	int twobyte_ratio;
	int seq_ratio;
	int recursive_limit;
	int twobyte_threshold;
} packProfile_t;

void printProfile(packProfile_t*);

void copyProfile(packProfile_t*, packProfile_t*);

void CanonicalDecodeAndReplace(const char*);

bool CanonicalEncodeAndTest(const char*);

bool SeqPackAndTest(const char* src, int seqlen_pages, int offset_pages, int ratio_limit);

bool MultiPackAndTest(const char* src, packProfile_t profile,
	packProfile_t seqlensProfile, packProfile_t offsetsProfile);


#endif