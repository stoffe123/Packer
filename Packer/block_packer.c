#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_packer.h"  
#include "common_tools.h"
#include "multi_packer.h"
#include "canonical.h"

// tar contents of src => utfil
append_to_tar(FILE* utfil, char* src, uint32_t size, uint8_t packType) {

	if (size == 0) {
		packType = setKthBit(packType, 4);
	}
	fwrite(&packType, 1, 1, utfil);
	if (size > 0) {
		fwrite(&size, 3, 1, utfil);
	}

	//write contents
	append_to_file(utfil, src);
}


//----------------------------------------------------------------------------------------

void block_pack(const wchar_t* src, const wchar_t* dst, packProfile profile) {

	uint64_t src_size = get_file_size_from_wname(src);

	

	FILE* utfil = openWrite(dst);
	FILE* infil = openRead(src);
	uint64_t chunkSize, read_size;
	do {
		const char chunkFilename[100] = { 0 };
		getTempFile(chunkFilename, "block_chunck");
		read_size = BLOCK_SIZE - profile.blockSizeMinus * (uint64_t)10000;

		assert(read_size < 16777215, "too large blocksize must be < 16777215");

		//workaround for a bug that even 2 powers cause bug
		//this bug should be fixed but it is hard
		if (read_size % 256 == 0) {
		 	read_size--;
		}
		printf("\n Real blocksize used %d", read_size);
		copy_chunk(infil, chunkFilename, read_size);
		chunkSize = get_file_size_from_name(chunkFilename);

		const char packedFilename[100] = { 0 };
		getTempFile(packedFilename, "block_multipacked");

		//meta testsuit 1170029  / 33s
		packProfile seqlenProfile = getPackProfile();
		seqlenProfile.rle_ratio = 31;
		seqlenProfile.twobyte_ratio = 97;
		seqlenProfile.recursive_limit = 180;
		seqlenProfile.twobyte_threshold_max = 5226;
		seqlenProfile.twobyte_threshold_divide = 2233;
		seqlenProfile.twobyte_threshold_min = 185;
		seqlenProfile.seqlenMinLimit3 = 43;
		seqlenProfile.winsize = 78725;
		seqlenProfile.sizeMaxForCanonicalHeaderPack = 175;
		seqlenProfile.sizeMinForSeqPack = 2600;
		seqlenProfile.sizeMinForCanonical = 30;

		packProfile offsetProfile = getPackProfile();
		offsetProfile.rle_ratio = 74;
		offsetProfile.twobyte_ratio = 95;
		offsetProfile.recursive_limit = 61;
		offsetProfile.twobyte_threshold_max = 11404;
		offsetProfile.twobyte_threshold_divide = 2520;
		offsetProfile.twobyte_threshold_min = 384;
		offsetProfile.seqlenMinLimit3 = 82;
		offsetProfile.winsize = 91812;
		offsetProfile.sizeMaxForCanonicalHeaderPack = 530;
		offsetProfile.sizeMinForSeqPack = 2600;
		offsetProfile.sizeMinForCanonical = 261;

		packProfile distanceProfile = getPackProfile();
		distanceProfile.rle_ratio = 71;
		distanceProfile.twobyte_ratio = 100;
		distanceProfile.recursive_limit = 20;
		distanceProfile.twobyte_threshold_max = 3641;
		distanceProfile.twobyte_threshold_divide = 3972;
		distanceProfile.twobyte_threshold_min = 37;
		distanceProfile.seqlenMinLimit3 = 35;
		distanceProfile.winsize = 80403;
		distanceProfile.sizeMaxForCanonicalHeaderPack = 256;
		distanceProfile.sizeMinForSeqPack = 2600;
		distanceProfile.sizeMinForCanonical = 300;


		uint8_t packType = multiPack(chunkFilename, packedFilename, profile, seqlenProfile, 
			offsetProfile, distanceProfile);
		remove(chunkFilename);
		uint32_t size = get_file_size_from_name(packedFilename);
		if (chunkSize < read_size) {
			size = 0;
		}
		append_to_tar(utfil, packedFilename, size, packType);
		remove(packedFilename);
	} while (chunkSize == read_size);

	fclose(infil);
	fclose(utfil);
}


// ----------------------------------------------------------------

void block_unpack(const wchar_t* src, const wchar_t* dst) {
	FILE* utfil = openWrite(dst);
	FILE* infil = openRead(src);

	//todo read packtype here and if bit 4 is set don't read size, just read til the end!
	//will save 16*4 = 64 bytes total in test suit 16
	while (true) {

		uint8_t packType;
		const char tmp[100] = { 0 };
		getTempFile(tmp, "block_tobeunpacked");
		if (fread(&packType, 1, 1, infil) == 0) {
			break;
		}
		if (isKthBitSet(packType, 4)) {
			copy_the_rest(infil, tmp);
		}
		else {
			uint32_t size = 0;
			//3 bytes can handle block sizes up to 16777216‬
			//note that these are the sizes of the compressed chunks!
			fread(&size, 3, 1, infil);			
			copy_chunk(infil, tmp, size);
		}
		const char tmp2[100] = { 0 };
		getTempFile(tmp2, "block_multiunpacked");
		multiUnpack(tmp, tmp2, packType);

		remove(tmp);

		append_to_file(utfil, tmp2);

		remove(tmp2);
	}
	fclose(infil);
	fclose(utfil);
}





