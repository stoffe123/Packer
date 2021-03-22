#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_packer_commons.h"
#include "packer_commons.h"
#include "memfile.h"


unsigned char getSeqlenMin(uint64_t best_offset, uint8_t seqlenMinLimit3, packProfile profile) {
	if (best_offset < 2 * seqlenMinLimit3) {
		return 3;
	}
	return (best_offset >= profile.seqlenMinLimit4) ? 5 : 4;
}


uint64_t getLastByte(uint64_t longRange) {
	if (longRange == 0) {
		return 255;
	}
	if (longRange >= 3) {
		return 253;
	}
	return 254;
}
uint64_t getLowestSpecial(pageCoding_t pageCoding) {
	return getLastByte(pageCoding.useLongRange) + 1 - pageCoding.pages;
}

uint64_t calcPageMax(pageCoding_t coding) {
	return 	coding.pages * (uint64_t)256 + getLastByte(coding.useLongRange) - coding.pages;
}

uint64_t getBundleSize(seqPackBundle b) {
	uint32_t size = getMemSize(b.main) + getMemSize(b.seqlens) + getMemSize(b.offsets)
		+ getMemSize(b.distances);
	return (uint64_t)size;
}

void freBundle(seqPackBundle b) {

		freeMem(b.main);
		freeMem(b.seqlens);
		freeMem(b.offsets);
		freeMem(b.distances);
	
}