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

		//meta testsuit 858724
		packProfile seqlenProfile = getPackProfile(53, 143);
		seqlenProfile.rle_ratio = 63;
		seqlenProfile.twobyte_ratio = 56;
		seqlenProfile.seq_ratio = 68;
		seqlenProfile.recursive_limit = 614;
		seqlenProfile.twobyte_threshold_max = 9642;
		seqlenProfile.twobyte_threshold_divide = 2988;
		seqlenProfile.twobyte_threshold_min = 24;
		seqlenProfile.twobyte2_ratio = 48;
		seqlenProfile.twobyte2_threshold_max = 6361;
		seqlenProfile.twobyte2_threshold_divide = 329;
		seqlenProfile.twobyte2_threshold_min = 659;

		packProfile offsetProfile = getPackProfile(89, 216);
		offsetProfile.rle_ratio = 56;
		offsetProfile.twobyte_ratio = 83;
		offsetProfile.seq_ratio = 92;
		offsetProfile.recursive_limit = 679;
		offsetProfile.twobyte_threshold_max = 10801;
		offsetProfile.twobyte_threshold_divide = 316;
		offsetProfile.twobyte_threshold_min = 946;
		offsetProfile.twobyte2_ratio = 48;
		offsetProfile.twobyte2_threshold_max = 4079;
		offsetProfile.twobyte2_threshold_divide = 290;
		offsetProfile.twobyte2_threshold_min = 309;

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




