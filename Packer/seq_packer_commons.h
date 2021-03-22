#ifndef SEQ_PACKER_COMMONS_H
#define SEQ_PACKER_COMMONS_H
#define SEQ_LEN_FOR_CODE 255

#include "packer_commons.h"

static uint64_t SUPERSLIM_SIZELIMIT = 16384;
static uint64_t SUPERSLIM_SEQLEN_MIN_LIMIT3 = 222;

typedef struct seqPackBundle {
	memfile* main;
	memfile* seqlens;
	memfile* offsets;
	memfile* distances;
} seqPackBundle;

typedef struct pageCoding_t {
	uint64_t pages;
	uint64_t useLongRange;
} pageCoding_t;

typedef enum {
    SEQLEN, OFFSET, DISTANCE    
} meta_kind_t;

unsigned char getSeqlenMin(uint64_t best_offset, uint8_t seqlenMinLimit3, packProfile profile);

uint64_t getBundleSize(seqPackBundle b);

void freBundle(seqPackBundle b);

uint64_t getLastByte(uint64_t longRange);

uint64_t getLowestSpecial(pageCoding_t);

uint64_t calcPageMax(pageCoding_t);

#endif
