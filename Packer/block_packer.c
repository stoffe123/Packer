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

void block_pack(const wchar_t* src, const wchar_t* dst, packProfile profile) {

	uint64_t src_size = get_file_size_from_wname(src);

	FILE* utfil, * infil;
	errno_t err = _wfopen_s(&utfil, dst, L"wb");
	if (err != 0) {
		wprintf(L"\n Block_pack can't find destination file: %s", dst);exit(0);
	}
	err = _wfopen_s(&infil, src, L"rb");
	if (err != 0) {
		wprintf(L"\n Block_pack can't find source file: %s", src); exit(0);
	}
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

		packProfile seqlenProfile = getPackProfile(92, 179);
		seqlenProfile.rle_ratio = 66;
		seqlenProfile.twobyte_ratio = 61;
		seqlenProfile.seq_ratio = 68;
		seqlenProfile.recursive_limit = 651;
		seqlenProfile.twobyte_threshold_max = 9577;
		seqlenProfile.twobyte_threshold_divide = 2160;
		seqlenProfile.twobyte_threshold_min = 7;
		seqlenProfile.twobyte2_ratio = 75;
		seqlenProfile.twobyte2_threshold_max = 5386;
		seqlenProfile.twobyte2_threshold_divide = 2570;
		seqlenProfile.twobyte2_threshold_min = 635;

		packProfile offsetProfile = getPackProfile(131, 247);
		offsetProfile.rle_ratio = 38;
		offsetProfile.twobyte_ratio = 77;
		offsetProfile.seq_ratio = 92;
		offsetProfile.recursive_limit = 771;
		offsetProfile.twobyte_threshold_max = 7170;
		offsetProfile.twobyte_threshold_divide = 788;
		offsetProfile.twobyte_threshold_min = 597;
		offsetProfile.twobyte2_ratio = 11;
		offsetProfile.twobyte2_threshold_max = 1299;
		offsetProfile.twobyte2_threshold_divide = 297;
		offsetProfile.twobyte2_threshold_min = 467;

		multi_pack(tmp, tmp2, profile, seqlenProfile, offsetProfile);
		remove(tmp);

		append_to_tar(utfil, tmp2);
		remove(tmp2);
	} while (tmp_size == BLOCK_SIZE);

	fclose(infil);
	fclose(utfil);
}


// ----------------------------------------------------------------

void block_unpack(const wchar_t* src, const wchar_t* dst) {
	FILE* utfil, * infil;
	errno_t err = _wfopen_s(&utfil, dst, L"wb");
	if (err != 0) {
		wprintf(L"\n Block_pack can't find destination file: %s", dst); exit(0);
	}
	err = _wfopen_s(&infil, src, L"rb");
	if (err != 0) {
		wprintf(L"\n Block_pack can't find source file: %s", src); exit(0);
	}

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




