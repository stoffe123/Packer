#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

#include "common_tools.h"
#include "packer_commons.h"
#include "RLE_simple_packer_commons.h"
#include "memfile.h"

#define VERBOSE false

/* RLE simple packer */

//--------------------------------------------------------------------------------------------------------

memfile* RLE_pack_internal(memfile* infil, int pass, int code) {

	memfile* utfil = getMemfile();
	unsigned long char_freq[256] = { 0 };
	debug("\nRLE_simple_pack pass=%d", pass);
	
	unsigned long long max_runlength = 254 + MIN_RUNLENGTH;

	if (pass == 2) {
		
		// start compression!
		//write_runlength(code_occurred ? 1 : 0, utfil, runlengths_file);
		fputcc(code, utfil);
	}
	else { // pass = 1
		//code_occurred = false;
	}

	/* start compression */

	unsigned int runlength = 1;

	int read_char = fgetcc(infil);
	while (read_char != EOF) {

		if (pass == 1) {
			if (char_freq[read_char] < ULONG_MAX - runlength) {
				char_freq[read_char]++;
			}
		}
		int first_char = read_char;
		runlength = 1;
		while ((read_char = fgetcc(infil)) != EOF && runlength < max_runlength && read_char == first_char) {
			runlength++;
		}

		/* now we found the longest runlength in the window! */

		assert(runlength > 0, "runlength > 0 in RLE_simple_packer.RLE_pack_internal");
		if (runlength < MIN_RUNLENGTH) {
			if (pass == 1) {
				if (char_freq[first_char] < ULONG_MAX - runlength) {
					char_freq[first_char]++;
				}
			}
			else { // pass = 2
				for (int i = 0; i < runlength; i++) {
					if (first_char == code) {
						fputcc(code, utfil);
						fputcc(255, utfil);
						//assert(code_occurred, "code_occured in RLE_simple_packer.RLE_pack_internal");
					}
					else {
						fputcc(first_char, utfil);
					}
				}
			}
		}
		else { // Runlength fond!
			if (pass == 2) {
				fputcc(code, utfil);
				fputcc(runlength - MIN_RUNLENGTH, utfil);
				fputcc(first_char, utfil);
			}
			else {
				//since the code appeared in a runlength no penalty needed!
				if (char_freq[first_char] > 0) {
					char_freq[first_char]--;
				}
			}
		}
	}//end while
	if (pass == 1) {
		setPos(utfil, find_best_code(char_freq).value); // ugly but temporary!	
	}
	return utfil;
}

memfile* RLE_simple_pack_internal(memfile* src)
{
	memfile* res = RLE_pack_internal(src, 1, 0); //find code
	int code = getPos(res);
	fre(res);
	rewindMem(src);
	return RLE_pack_internal(src, 2, code); //pack
}

memfile* RleSimplePack(memfile* src) {
	return RLE_simple_pack_internal(src);
}

void RLE_simple_pack(const char* src, const char* dst) {	
	memfile* s = get_memfile_from_file(src);
	memfile* packed = RLE_simple_pack_internal(s);
	memfile_to_file(packed, dst);
	fre(s);
	fre(packed);
}

void RLE_simple_packw(const wchar_t* srcw, const wchar_t* dstw) {
	memfile* s = getMemfileFromFile(srcw);
	memfile* packed = RLE_simple_pack_internal(s);
	memfileToFile(packed, dstw);
	fre(s);
	fre(packed);
}

