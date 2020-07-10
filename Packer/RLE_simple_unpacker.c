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

	memfile* utfil = getMemfile();

	//code_occurred = read_runlength(infil, runlengths_file);
	unsigned char code = fgetcc(infil);

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

	RLE_simple_unpack_internal(m);
}

void RLE_simple_unpack(const char* src, const char* dst) {
	memfile* s = get_memfile_from_file(src);
	memfile* res = RleSimpleUnpack(s);
	memfile_to_file(res, dst);
	fre(s);
	fre(res);
}

void RLE_simple_unpackw(const wchar_t* src, const wchar_t* dst) {
	memfile* s = getMemfileFromFile(src);
	memfile* res = RleSimpleUnpack(s);
	memfileToFile(res, dst);
	fre(s);
	fre(res);
}
