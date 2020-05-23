#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>


unsigned char getSeqlenMin(uint64_t best_offset,uint64_t lowest_special_offset,uint64_t offsetPagesMax) {
	if (best_offset < lowest_special_offset) {
		return 3;
	}
	return (best_offset >= offsetPagesMax) ? 6 : 4;
}