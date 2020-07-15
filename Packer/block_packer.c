#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_packer.h"  
#include "common_tools.h"
#include "multi_packer.h"
#include "canonical.h"
#include "memfile.h"


void append_mem_to_file(FILE* main_file, memfile* append_file) {	
	
	int ch;
	rewindMem(append_file);
	while ((ch = fgetcc(append_file)) != EOF) {
		fputc(ch, main_file);
	}
}

// tar contents of src => utfil
append_to_tar(FILE* utfil, memfile* src, uint32_t size, uint8_t packType) {

	if (size == 0) {
		packType = setKthBit(packType, 4);
	}
	fwrite(&packType, 1, 1, utfil);
	if (size > 0) {
		fwrite(&size, 3, 1, utfil);
	}

	//write contents
	append_mem_to_file(utfil, src);
}

void copy_chunk_to_mem(FILE* source_file, memfile* dest_filename, uint64_t size_to_copy) {	
	uint8_t ch;
	for (int i = 0; i < size_to_copy; i++) {
		size_t bytes_got = fread(&ch, 1, 1, source_file);
		if (bytes_got == 0) {
			break;
		}
		fputcc(ch, dest_filename);
	}	
}

//----------------------------------------------------------------------------------------

void block_pack(const wchar_t* src, const wchar_t* dst, packProfile profile) {

	uint64_t src_size = get_file_size_from_wname(src);

	FILE* utfil = openWrite(dst);
	FILE* infil = openRead(src);
	uint64_t chunkSize, read_size;
	do {
		memfile* chunkFilename = getEmptyMem(L"blockpacker_chunk");		
		read_size = BLOCK_SIZE - profile.blockSizeMinus * (uint64_t)10000;

		assert(read_size < 16777215, "too large blocksize must be < 16777215");

		//workaround for a bug that even 2 powers cause bug
		//this bug should be fixed but it is hard
		if (read_size % 256 == 0) {
			read_size--;
		}
		printf("\n Real blocksize used %d", read_size);
		copy_chunk_to_mem(infil, chunkFilename, read_size);
		chunkSize = getMemSize(chunkFilename);

		memfile* packedFilename = getEmptyMem(L"blockpacker_packed");

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

		printf("\ blockpack multipack of chunk");
		uint8_t packType = multiPackAndReturnPackType(chunkFilename, packedFilename, profile, seqlenProfile,
			offsetProfile, distanceProfile);
		freMem(chunkFilename);
		printf("\n blockpack checking size of chunk %s", getMemName(packedFilename));
		uint32_t size = getMemSize(packedFilename);
		if (chunkSize < read_size) {
			size = 0;
		}
		append_to_tar(utfil, packedFilename, size, packType);
		freMem(packedFilename);
	} while (chunkSize == read_size);

	fclose(infil);
	fclose(utfil);
}

void copy_the_rest2(FILE* in, memfile* out) {	
	
	int ch;
	while ((ch = fgetc(in)) != EOF) {
		fputcc(ch, out);
	}	
}


// ----------------------------------------------------------------

void block_unpack(const wchar_t* src, const wchar_t* dst) {
	FILE* utfil = openWrite(dst);
	FILE* infil = openRead(src);

	//todo read packtype here and if bit 4 is set don't read size, just read til the end!
	//will save 16*4 = 64 bytes total in test suit 16
	while (true) {

		uint8_t packType;
		memfile* tmp = getEmptyMem(L"blockpacker_tmp");
		if (fread(&packType, 1, 1, infil) == 0) {
			break;
		}
		if (isKthBitSet(packType, 4)) {
			copy_the_rest2(infil, tmp);
		}
		else {
			uint32_t size = 0;
			//3 bytes can handle block sizes up to 16777216‬
			//note that these are the sizes of the compressed chunks!
			fread(&size, 3, 1, infil);
			copy_chunk_to_mem(infil, tmp, size);
		}				
		memfile* tmp2 = multiUnpack(tmp, packType);

		freMem(tmp);
		append_mem_to_file(utfil, tmp2);

		freMem(tmp2);
	}
	fclose(infil);
	fclose(utfil);
}


void blockUnpackAndReplace(wchar_t* src) {

	const wchar_t tmp[100] = { 0 };
	get_temp_filew(tmp, L"blockpacker_unpackandreplace");
	block_unpack(src, tmp);
	my_renamew(tmp, src);

}



