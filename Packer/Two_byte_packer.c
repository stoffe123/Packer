#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "common_tools.h"
#include "packer_commons.h"
#include "memfile.h"

#define VERBOSE false
#define START_CODES_SIZE 2

/* Two-byte packer */

//Global variables used in compressor

__declspec(thread) static  unsigned long two_byte_freq_table[65536] = { 0 };

__declspec(thread) static  unsigned long char_freq[256];
__declspec(thread) static  packProfile profile;


__declspec(thread) static  uint8_t codes[256];
__declspec(thread) static  uint16_t values[256];
__declspec(thread) static uint8_t master_code, numberOfCodes;

val_freq_t find_best_two_byte() {
	unsigned long best = 0;
	uint16_t two_byte = 0;
	for (unsigned int i = 0; i < 65536; i++) {
		if (two_byte_freq_table[i] > best) {
			best = two_byte_freq_table[i];
			two_byte = i;
		}
	}
	if (best > 0) {
		two_byte_freq_table[two_byte] = 0; // mark as used
	}
	val_freq_t res;
	res.value = two_byte;
	res.freq = best;
	return res;
}

uint64_t getGainThreshold(uint64_t source_size) {

	uint64_t res = source_size / profile.twobyte_threshold_divide;

	if (res < profile.twobyte_threshold_min) {
		return profile.twobyte_threshold_min;
	}
	if (res > profile.twobyte_threshold_max) {
		return profile.twobyte_threshold_max;
	}
	return res;
}


/*
this code sets global vars:
codes and values arrays
master_code
numberOfCodes

*/
int create_two_byte_table(uint64_t source_size) {
	debug("\n creating two_byte_table \n");

	value_freq_t master = find_best_code(char_freq);
	master_code = master.value;
	uint64_t threshold = getGainThreshold(source_size);
	debug("\n Two byte packer gain threshhold %lld", threshold);
	bool found_twobyte = false;
	int pair_table_pos = 0;
	do {
		val_freq_t two_byte = find_best_two_byte(); //ineffecient to search whole 65k table every time
		if (two_byte.freq < threshold) {
			break;
		}
		value_freq_t code = find_best_code(char_freq);
		found_twobyte = (code.freq + threshold < two_byte.freq);
		if (found_twobyte) {
			//worthwile
			codes[pair_table_pos] = (uint8_t)code.value;
			values[pair_table_pos] = two_byte.value;			
			pair_table_pos++;
		}
	} while (found_twobyte);
	numberOfCodes = pair_table_pos;	
	debug("\nCreated two_byte table size: %d\n", numberOfCodes);
}

int find_code_for_pair(uint16_t val) {
	for (int i = 0; i < numberOfCodes; i++) {
		if (values[i] == val) {
			return codes[i];
		}
	}
	return 256;
}

bool is_code(uint8_t ch) {
	if (ch == master_code) {
		return true;
	}
	for (int i = 0; i<numberOfCodes; i++) {
		if (codes[i] == ch) {
			return true;
		}
	}
	return false;
}


//--------------------------------------------------------------------------------------------------------


memfile* two_byte_pack_internal(memfile* infil, int pass) {

	rewindMem(infil);

	memfile* utfil = NULL;

	debug("\nTwo-byte pack pass=%d", pass);

	uint64_t source_size = getMemSize(infil);


	if (pass >= 2) {
		create_two_byte_table(source_size);

		if (pass == 3) {
			utfil = getMemfile((uint64_t)200 + infil->size, L"twobytepack_utfil");

			//write the metadata table
			fputccLight(master_code, utfil);
			fputccLight(numberOfCodes, utfil);
			for (int i = 0; i < numberOfCodes; i++) {

				fputccLight(codes[i], utfil);
				fput2ccLight(values[i], utfil);	
			}
		}
	}
	if (pass < 3) {
		for (int i = 0; i < 65536; i++) {
			two_byte_freq_table[i] = 0;
		}
		for (int i = 0; i < 256; i++) {
			char_freq[i] = 0;
		}
	}
	int ch1;
	while ((ch1 = fgetcc(infil)) != EOF) {

		int ch2 = nextcc(infil);
		bool lastChar = (ch2 == EOF);
		int val = lastChar ? 0 : ch1 + 256 * ch2;
		if (pass >= 2) {
			int code = (lastChar ? 256 :
				find_code_for_pair(val));

			if (code == 256) {
				// not found
				if (pass == 2) {
					if (char_freq[ch1] < LONG_MAX) {
						char_freq[ch1]++;
					}
				}
				else if (pass == 3) {
					if (is_code(ch1)) {
						fputccLight(master_code, utfil);
					}
					fputccLight(ch1, utfil);
				}
			}
			else { // write the code for the pair
				if (pass == 2) {
					if (two_byte_freq_table[val] < LONG_MAX) {
						two_byte_freq_table[val]++;
					}
				}
				else if (pass == 3) {
					fputccLight(code, utfil);
				}
				fgetcc(infil);
			}
		}
		else { // pass == 1
			if (!lastChar) {
				if (two_byte_freq_table[val] < LONG_MAX) {
					two_byte_freq_table[val]++;
				}
			}
			if (char_freq[ch1] < LONG_MAX) {
				char_freq[ch1]++;
			}
		}
	}//end while

	if (pass == 3) {
		syncMemSize(utfil);
		return utfil;
	}
}

memfile* twoBytePack(memfile* m, packProfile prof) {
	profile = prof;
	two_byte_pack_internal(m, 1); //analyse and build metadata
	two_byte_pack_internal(m, 2); //simulate pack and adjust metadata
	return two_byte_pack_internal(m, 3); //pack	
}


void two_byte_packw(const wchar_t* src, const wchar_t* dest, packProfile prof)
{
	profile = prof;
	memfile* srcm = getMemfileFromFile(src);
	memfile* dstm = twoBytePack(srcm, prof);
	freeMem(srcm);
	memfileToFile(dstm, dest);
	freeMem(dstm);
}



