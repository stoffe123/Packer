#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>


unsigned char getSeqlenMin(uint64_t best_offset) {
	if (best_offset < 256) {
		return 3;
	}
	return (best_offset >= 40000) ? 6 : 4;
}


uint64_t getLastByte(uint64_t longRange) {
	return longRange ? 254 : 255;
}

uint64_t getLowestSpecial(uint64_t pages, uint64_t longRange) {
	return getLastByte(longRange) + 1 - pages;
}

uint64_t calcPageMax(uint64_t pages, uint64_t useLongRange) {
	return 	pages * (uint64_t)256 + ((useLongRange ? 254 : 255) - pages);
}
