#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_packer_commons.h"
#include "packer_commons.h"
#include "memfile.h"


unsigned char getSeqlenMin(uint64_t best_offset, packProfile profile) {
	if (best_offset < (uint64_t)2 * profile.seqlenMinLimit3) {
		return 3;
	}
	return (best_offset >= SEQLEN_MIN_LIMIT4) ? 5 : 4;
}


uint64_t getLastByte(uint64_t longRange) {
	return longRange ? 254 : 255;
}

uint64_t getLowestSpecial(pageCoding_t pageCoding) {
	return getLastByte(pageCoding.useLongRange) + 1 - pageCoding.pages;
}

uint64_t calcPageMax(pageCoding_t coding) {
	return 	coding.pages * (uint64_t)256 + ((coding.useLongRange ? 254 : 255) - coding.pages);
}

uint64_t getBundleSize(seqPackBundle b) {
	uint32_t size = getMemSize(b.main) + getMemSize(b.seqlens) + getMemSize(b.offsets)
		+ getMemSize(b.distances);
	return (uint64_t)size;
}

void freBundle(seqPackBundle b) {

		freMem(b.main);
		freMem(b.seqlens);
		freMem(b.offsets);
		freMem(b.distances);
	
}