#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "common_tools.h"
#include "multi_packer.h"
#include "canonical.h"

// tar contents of src => utfil
append_to_tar(FILE* utfil, char* src) {
	//write size
	uint32_t size = get_file_size_from_name(src);
	fwrite(&size, sizeof(size), 1, utfil);

	//write contents
	append_to_file(utfil, src);
}


//----------------------------------------------------------------------------------------

void block_pack(const char* src, const char* dst, packProfile_t profile) {

	unsigned long long  src_size = get_file_size_from_name(src);

	FILE* utfil, * infil;
	utfil = fopen(dst, "wb");
	infil = fopen(src, "rb");

	uint64_t tmp_size;
	do {
		const char tmp[100] = { 0 };
		get_temp_file2(tmp, "block_chunck");
		uint64_t read_size = BLOCK_SIZE;

		//workaround for a bug that even 2 powers cause bug
		//this bug should be fixed but it is hard
		if (read_size % 256 == 0) {
			read_size--;
		}
		copy_chunk(infil, tmp, read_size);
		tmp_size = get_file_size_from_name(tmp);

		const char tmp2[100] = { 0 };
		get_temp_file2(tmp2, "block_multipacked");

		packProfile_t seqlenProfile;
		seqlenProfile.offset_pages = 220;
		seqlenProfile.seqlen_pages = 2;
		seqlenProfile.rle_ratio = 82;
		seqlenProfile.twobyte_ratio = 85;
		seqlenProfile.seq_ratio = 68;
		seqlenProfile.recursive_limit = 161;
		seqlenProfile.twobyte_threshold = 596;
		seqlenProfile.twobyte_divide = 1000;
		
		packProfile_t offsetProfile;
		offsetProfile.offset_pages = 105;
		offsetProfile.seqlen_pages = 57;
		offsetProfile.rle_ratio = 80;
		offsetProfile.twobyte_ratio = 79;
		offsetProfile.seq_ratio = 94;
		offsetProfile.recursive_limit = 100;
		offsetProfile.twobyte_threshold = 1400;
		offsetProfile.twobyte_divide = 1000;


		multi_pack(tmp, tmp2, profile, seqlenProfile, offsetProfile);
		remove(tmp);

		append_to_tar(utfil, tmp2);
		remove(tmp2);
	} while (tmp_size == BLOCK_SIZE);

	fclose(infil);
	fclose(utfil);
}


// ----------------------------------------------------------------

void block_unpack(const char* src, const char* dst) {
	FILE* utfil, * infil;
	utfil = fopen(dst, "wb");
	infil = fopen(src, "rb");

	uint32_t size;
	while (fread(&size, sizeof(size), 1, infil) == 1) {


		const char tmp[100] = { 0 };
		get_temp_file2(tmp, "block_tobeunpacked");
		copy_chunk(infil, tmp, size);

		const char tmp2[100] = { 0 };
		get_temp_file2(tmp2, "block_multiunpacked");
		multi_unpack(tmp, tmp2);

		remove(tmp);

		append_to_file(utfil, tmp2);

		remove(tmp2);
	}
	fclose(infil);
	fclose(utfil);
}




