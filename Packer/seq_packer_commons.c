#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_packer_commons.h"
#include "packer_commons.h"


unsigned char getSeqlenMin(uint64_t best_offset, packProfile profile) {
	if (best_offset < (uint64_t)2 * profile.seqlenMinLimit3) {
		return 3;
	}
	return (best_offset >= SEQLEN_MIN_LIMIT4) ? 6 : 4;
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
