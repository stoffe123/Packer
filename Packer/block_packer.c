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
append_to_tar(FILE* utfil, char* src, uint32_t size) {
	//write size
	if (size == 0) {
		size = 255;
		fwrite(&size, 1, 1, utfil);
	}
	else {
		printf("\n   WRITTEN BLOCK SIZE : %d", size);
		uint8_t byte1, byte2, byte3;
		byte1 = (size & 0xFF0000) >> 16;
		byte2 = (size & 0x00FF00) >> 8;
		byte3 = (size & 0x0000FF);
		fwrite(&byte1, 1, 1, utfil);
		fwrite(&byte2, 1, 1, utfil);
		fwrite(&byte3, 1, 1, utfil);
	}

	//write contents
	append_to_file(utfil, src);
}


//----------------------------------------------------------------------------------------

void block_pack(const wchar_t* src, const wchar_t* dst, packProfile profile) {

	uint64_t src_size = get_file_size_from_wname(src);

	

	FILE* utfil = openWrite(dst);
	FILE* infil = openRead(src);
	uint64_t chunkSize;
	do {
		const char chunkFilename[100] = { 0 };
		getTempFile(chunkFilename, "block_chunck");
		uint64_t read_size = BLOCK_SIZE;

		assert(read_size < 16711679, "too large blocksize must be < 16711679");

		//workaround for a bug that even 2 powers cause bug
		//this bug should be fixed but it is hard
		if (read_size % 256 == 0) {
			read_size--;
		}
		copy_chunk(infil, chunkFilename, read_size);
		chunkSize = get_file_size_from_name(chunkFilename);

		const char packedFilename[100] = { 0 };
		getTempFile(packedFilename, "block_multipacked");

		//meta testsuit 849813
		packProfile seqlenProfile = getPackProfile(53, 148);
		seqlenProfile.rle_ratio = 65;
		seqlenProfile.twobyte_ratio = 67;
		seqlenProfile.recursive_limit = 10;
		seqlenProfile.twobyte_threshold_max = 9299;
		seqlenProfile.twobyte_threshold_divide = 3925;
		seqlenProfile.twobyte_threshold_min = 24;

		packProfile offsetProfile = getPackProfile(93, 219);
		offsetProfile.rle_ratio = 54;
		offsetProfile.twobyte_ratio = 87;
		offsetProfile.recursive_limit = 169;
		offsetProfile.twobyte_threshold_max = 11509;
		offsetProfile.twobyte_threshold_divide = 1271;
		offsetProfile.twobyte_threshold_min = 963;

		multi_pack(chunkFilename, packedFilename, profile, seqlenProfile, offsetProfile);
		remove(chunkFilename);
		uint32_t size = get_file_size_from_name(packedFilename);
		if (chunkSize < BLOCK_SIZE) {
			size = 0;
		}
		append_to_tar(utfil, packedFilename, size);
		remove(packedFilename);
	} while (chunkSize == BLOCK_SIZE);

	fclose(infil);
	fclose(utfil);
}


// ----------------------------------------------------------------

void block_unpack(const wchar_t* src, const wchar_t* dst) {
	FILE* utfil = openWrite(dst);
	FILE* infil = openRead(src);

	//todo read packtype here and if bit 7 is set don't read size, just read til the end!
	//will save 16*4 = 64 bytes total in test suit 16
	while (true) {

		uint8_t byte1, byte2, byte3;
		const char tmp[100] = { 0 };
		getTempFile(tmp, "block_tobeunpacked");
		if (fread(&byte1, 1, 1, infil) == 0) {
			break;
		}
		if (byte1 == 255) {
			copy_the_rest(infil, tmp);
		}
		else {
			fread(&byte2, 1, 1, infil);
			fread(&byte3, 1, 1, infil);
			uint32_t size = byte1 * 65536 + byte2 * 256 + byte3;
			printf("\n   CALULCATED BLOCK SIZE : %d", size);
			copy_chunk(infil, tmp, size);
		}
		const char tmp2[100] = { 0 };
		getTempFile(tmp2, "block_multiunpacked");
		multi_unpack(tmp, tmp2);

		remove(tmp);

		append_to_file(utfil, tmp2);

		remove(tmp2);
	}
	fclose(infil);
	fclose(utfil);
}





