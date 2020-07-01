#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <windows.h>
#include <string.h>
#include <conio.h>
#include <process.h>
#include "seq_packer.h"  
#include "common_tools.h"
#include "multi_packer.h"
#include "canonical.h"

typedef struct blockChunk_t {
	const char* packedFilename;
	const char* unpackedFilename;
	packProfile profile;
	uint64_t size;
	uint16_t packType;
	uint64_t chunkSize;
	HANDLE  handle;
} blockChunk_t;

static blockChunk_t blockChunks[1000];

static packProfile seqlenProfile, offsetProfile, distanceProfile;

static uint64_t read_size;

HANDLE  hScreenMutex;                // "Screen update" mutex

HANDLE lockTempfileMutex() {
	WaitForSingleObject(hScreenMutex, INFINITE);
}

HANDLE releaseTempfileMutex() {
	ReleaseMutex(hScreenMutex);
}

// tar contents of src => utfil
append_to_tar(FILE* utfil, blockChunk_t blockChunk) {
	
	uint32_t size = blockChunk.size;
	uint8_t packType = blockChunk.packType;

	if (size == 0) {
		packType = setKthBit(packType, 4);
	}
	fwrite(&packType, 1, 1, utfil);
	if (size > 0) {
		fwrite(&size, 3, 1, utfil);
	}
	//write contents
	append_to_file(utfil, blockChunk.packedFilename);
}

void threadMultiPack(void* pMyID)
{
	blockChunk_t* bc = (blockChunk_t*)pMyID;

	bc->packType = multiPack(bc->unpackedFilename, bc->packedFilename, bc->profile, seqlenProfile,
		offsetProfile, distanceProfile);
	remove(bc->unpackedFilename);
	bc->size = get_file_size_from_name(bc->packedFilename);
	if (bc->chunkSize < read_size) {
		bc->size = 0;
	}
	printf("\n THREAD MULTIPACK FOR %s FINISHED!", bc->unpackedFilename);
}


void threadMultiUnpack(void* pMyID)
{
	blockChunk_t* bc = (blockChunk_t*)pMyID;

	multiUnpack(bc->packedFilename, bc->unpackedFilename, bc->packType);
	remove(bc->packedFilename);
	printf("\n THREAD MULTIUNPACK FOR %s FINISHED!", bc->unpackedFilename);
}

//----------------------------------------------------------------------------------------

void block_pack(const wchar_t* src, const wchar_t* dst, packProfile profile) {

	uint64_t src_size = get_file_size_from_wname(src);

	hScreenMutex = CreateMutexW(NULL, FALSE, NULL);  // Cleared

	//meta testsuit 1170029  / 33s
	seqlenProfile = getPackProfile();
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

	offsetProfile = getPackProfile();
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

	distanceProfile = getPackProfile();
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

	FILE* infil = openRead(src);
	int chunkNumber = 0;
	do {
		char* unpackedFilename = (char*)malloc(100 * sizeof(char));		
		getTempFile(unpackedFilename, "block_chunk");
		read_size = BLOCK_SIZE - profile.blockSizeMinus * (uint64_t)10000;

		assert(read_size < 16777215, "too large blocksize must be < 16777215");

		//workaround for a bug that even 2 powers cause bug
		//this bug should be fixed but it is hard
		if (read_size % 256 == 0) {
		 	read_size--;
		}
		printf("\n Real blocksize used %d", read_size);
		copy_chunk(infil, unpackedFilename, read_size);
		blockChunks[chunkNumber].chunkSize = get_file_size_from_name(unpackedFilename);

		char* packedFilename = (char*)malloc(100 * sizeof(char));
		getTempFile(packedFilename, "block_multipacked");

		blockChunks[chunkNumber].unpackedFilename = unpackedFilename;
		blockChunks[chunkNumber].packedFilename = packedFilename;
		blockChunks[chunkNumber].profile = profile;		

		printf("\n STARTING THREAD FOR MULTIPACK %d ", chunkNumber);
		blockChunks[chunkNumber].handle = _beginthread(threadMultiPack, 0, &blockChunks[chunkNumber]);
		
	} while (blockChunks[chunkNumber++].chunkSize == read_size);
	fclose(infil);

	printf("\n%d processes started!", chunkNumber);
	
	FILE* utfil = openWrite(dst);
	for (int i = 0; i < chunkNumber; i++) {
		WaitForSingleObject(blockChunks[i].handle, INFINITE);
		blockChunk_t blockChunk = blockChunks[i];
		append_to_tar(utfil, blockChunk);
		remove(blockChunk.packedFilename);
		free(blockChunk.unpackedFilename);
		free(blockChunk.packedFilename);
	}
	fclose(utfil);	
}


// ----------------------------------------------------------------

void block_unpack(const wchar_t* src, const wchar_t* dst) {
	

	
	FILE* infil = openRead(src);

	//todo read packtype here and if bit 4 is set don't read size, just read til the end!
	//will save 16*4 = 64 bytes total in test suit 16
	int chunkNumber = 0;
	while (true) {

		uint8_t packType;
		
		char* packedFilename = (char*)malloc(100 * sizeof(char));
		getTempFile(packedFilename, "block_tobeunpacked");
		

		if (fread(&packType, 1, 1, infil) == 0) {
			break;
		}
		if (isKthBitSet(packType, 4)) {
			copy_the_rest(infil, packedFilename);
		}
		else {
			uint32_t size = 0;
			//3 bytes can handle block sizes up to 16777216‬
			//note that these are the sizes of the compressed chunks!
			fread(&size, 3, 1, infil);
			copy_chunk(infil, packedFilename, size);
		}
		char* unpackedFilename = (char*)malloc(100 * sizeof(char));
		getTempFile(unpackedFilename, "block_multiunpacked");
		blockChunks[chunkNumber].packType = packType;
		blockChunks[chunkNumber].unpackedFilename = unpackedFilename;
		blockChunks[chunkNumber].packedFilename = packedFilename;
		printf("\n STARTING THREAD FOR MULTIUNPACK %d ", chunkNumber);
		blockChunks[chunkNumber].handle = _beginthread(threadMultiUnpack, 0, &blockChunks[chunkNumber]);		
		chunkNumber++;
	}
	fclose(infil);

	FILE* utfil = openWrite(dst);
	for (int i = 0; i < chunkNumber; i++) {
		WaitForSingleObject(blockChunks[i].handle, INFINITE);
		blockChunk_t blockChunk = blockChunks[i];
		append_to_file(utfil, blockChunk.unpackedFilename);
		free(blockChunk.unpackedFilename);
		free(blockChunk.packedFilename);
	}
	fclose(utfil);
}





