#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common_tools.h"
#include "packer_commons.h"
#include "RLE_simple_packer_commons.h"
#include "memfile.h"

// RLE simple  - unpacker


//------------------------------------------------------------------------------
memfile* RLE_simple_unpack_internal(memfile* infil)
{
	unsigned long i;
	rewindMem(infil);
	memfile* utfil = getMemfile((uint64_t)2 * infil->size, L"rlesimpleunpack.utfil");

	//code_occurred = read_runlength(infil, runlengths_file);
	uint8_t code = fgetcc(infil);

	int cc;
	while ((cc = fgetcc(infil)) != EOF) {

		if (cc == code) {

			unsigned int runlength = fgetcc(infil);

			if (runlength == 255) {
				//occurrence of code in original
				fputcc(code, utfil);
			}
			else {
				cc = fgetcc(infil);
				assert(cc != EOF, "cc == EOF  in RLE_simple_unpacker.RLE_simple_unpack");
				for (i = 0; i < (runlength + MIN_RUNLENGTH); i++) {
					fputcc(cc, utfil);
				}
			}
		}
		else {
			fputcc(cc, utfil);
		}
	}
	return utfil;
}


memfile* RleSimpleUnpack(memfile* m) {

	return RLE_simple_unpack_internal(m);
}


void RLE_simple_unpackw(const wchar_t* src, const wchar_t* dst) {
	memfile* s = getMemfileFromFile(src);
	memfile* res = RleSimpleUnpack(s);
	fre(s);
	memfileToFile(res, dst);
	fre(res);
}
