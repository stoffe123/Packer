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

static uint64_t read_size;

//meta testsuit 1170029  / 33s
static packProfile seqlenProfile = {
.rle_ratio = 31,
.twobyte_ratio = 97,
.recursive_limit = 180,
.twobyte_threshold_max = 5226,
.twobyte_threshold_divide = 2233,
.twobyte_threshold_min = 185,
.seqlenMinLimit3 = 43,
.winsize = 78725,
.sizeMaxForCanonicalHeaderPack = 175,
.sizeMinForSeqPack = 2600,
.sizeMinForCanonical = 30,
.sizeMaxForSuperslim = 16384
},
offsetProfile = {	
	.rle_ratio = 74,
	.twobyte_ratio = 95,
	.recursive_limit = 61,
	.twobyte_threshold_max = 11404,
	.twobyte_threshold_divide = 2520,
	.twobyte_threshold_min = 384,
	.seqlenMinLimit3 = 82,
	.winsize = 91812,
	.sizeMaxForCanonicalHeaderPack = 530,
	.sizeMinForSeqPack = 2600,
	.sizeMinForCanonical = 261,
    .sizeMaxForSuperslim = 16384
},
distanceProfile = {
	.rle_ratio = 71,
	.twobyte_ratio = 100,
	.recursive_limit = 20,
	.twobyte_threshold_max = 3641,
	.twobyte_threshold_divide = 3972,
	.twobyte_threshold_min = 37,
	.seqlenMinLimit3 = 35,
	.winsize = 80403,
	.sizeMaxForCanonicalHeaderPack = 256,
	.sizeMinForSeqPack = 2600,
	.sizeMinForCanonical = 300,
    .sizeMaxForSuperslim = 16384
};

HANDLE tempfileMutex, clockdirMutex, blockchunkMutex;

HANDLE lockTempfileMutex() {
	WaitForSingleObject(tempfileMutex, INFINITE);
}

HANDLE releaseTempfileMutex() {
	ReleaseMutex(tempfileMutex);
}

HANDLE lockClockdirMutex() {
	WaitForSingleObject(clockdirMutex, INFINITE);
}

HANDLE releaseClockdirMutex() {
	ReleaseMutex(clockdirMutex);
}

HANDLE lockBlockchunkMutex() {
	WaitForSingleObject(blockchunkMutex, INFINITE);
}

HANDLE releaseBlockchunkMutex() {
	ReleaseMutex(blockchunkMutex);
}

// tar contents of src => utfil
append_to_tar(FILE* utfil, const char* packedFilename, uint32_t size, uint8_t packType) {

	if (size == 0) {
		packType = setKthBit(packType, 4);
	}
	fwrite(&packType, 1, 1, utfil);
	if (size > 0) {
		fwrite(&size, 3, 1, utfil);
	}
	//write contents
	append_to_file(utfil, packedFilename);
}

void threadMultiPack(void* pMyID)
{
	lockBlockchunkMutex();
	blockChunk_t* bc = (blockChunk_t*)pMyID;
	const char* src = bc->unpackedFilename;
	const char* dst = bc->packedFilename;
	packProfile prof = bc->profile;
	releaseBlockchunkMutex();

	uint8_t packType = multiPack(src, dst, prof, seqlenProfile,
		offsetProfile, distanceProfile);

	lockBlockchunkMutex();
	bc->packType = packType;
	bc->size = get_file_size_from_name(bc->packedFilename);
	if (bc->chunkSize < read_size) {
		bc->size = 0;
	}
	printf("\n THREAD MULTIPACK FOR %s FINISHED!", bc->unpackedFilename);
	releaseBlockchunkMutex();
}


void threadMultiUnpack(void* pMyID)
{
	lockBlockchunkMutex();
	blockChunk_t* bc = (blockChunk_t*)pMyID;
	const char* src = bc->packedFilename;
	const char* dst = bc->unpackedFilename;
	int packType = bc->packType;
	releaseBlockchunkMutex();

	multiUnpack(src, dst, packType);
	printf("\n THREAD MULTIUNPACK FOR %s FINISHED!", bc->unpackedFilename);
}

//----------------------------------------------------------------------------------------

void block_pack(const wchar_t* src, const wchar_t* dst, packProfile profile) {

	uint64_t src_size = get_file_size_from_wname(src);

	tempfileMutex = CreateMutexW(NULL, FALSE, NULL);  // Cleared
	clockdirMutex = CreateMutexW(NULL, FALSE, NULL);  // Cleared
	blockchunkMutex = CreateMutexW(NULL, FALSE, NULL);  // Cleared

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

		char* packedFilename = (char*)malloc(100 * sizeof(char));
		getTempFile(packedFilename, "block_multipacked");

		lockBlockchunkMutex();
		blockChunks[chunkNumber].chunkSize = get_file_size_from_name(unpackedFilename);
		blockChunks[chunkNumber].unpackedFilename = unpackedFilename;
		blockChunks[chunkNumber].packedFilename = packedFilename;
		blockChunks[chunkNumber].profile = profile;	
		releaseBlockchunkMutex();

		printf("\n STARTING THREAD FOR MULTIPACK %d ", chunkNumber);
		HANDLE handle = _beginthread(threadMultiPack, 0, &blockChunks[chunkNumber]);

		lockBlockchunkMutex();
		blockChunks[chunkNumber].handle = handle;
		releaseBlockchunkMutex();
		
	} while (blockChunks[chunkNumber++].chunkSize == read_size);
	fclose(infil);

	printf("\n%d processes started!", chunkNumber);
	
	FILE* utfil = openWrite(dst);
	for (int i = 0; i < chunkNumber; i++) {
		WaitForSingleObject(blockChunks[i].handle, INFINITE);
		lockBlockchunkMutex();
		blockChunk_t blockChunk = blockChunks[i];
		const char* packedFilename = blockChunk.packedFilename;
		uint32_t size = blockChunk.size;
		uint8_t packType = blockChunk.packType;
		releaseBlockchunkMutex();

		append_to_tar(utfil, packedFilename, size, packType);

		lockBlockchunkMutex();
		remove(blockChunk.unpackedFilename);
		remove(blockChunk.packedFilename);
		free(blockChunk.unpackedFilename);
		free(blockChunk.packedFilename);
		releaseBlockchunkMutex();
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
		
		lockBlockchunkMutex();
		blockChunks[chunkNumber].packType = packType;
		blockChunks[chunkNumber].unpackedFilename = unpackedFilename;
		blockChunks[chunkNumber].packedFilename = packedFilename;
		printf("\n STARTING THREAD FOR MULTIUNPACK %d ", chunkNumber);
		releaseBlockchunkMutex();

		HANDLE handle = _beginthread(threadMultiUnpack, 0, &blockChunks[chunkNumber]);		

		lockBlockchunkMutex();
		blockChunks[chunkNumber].handle = handle;
		releaseBlockchunkMutex();

		chunkNumber++;
	}
	fclose(infil);

	FILE* utfil = openWrite(dst);
	for (int i = 0; i < chunkNumber; i++) {
		WaitForSingleObject(blockChunks[i].handle, INFINITE);

		lockBlockchunkMutex();
		const char* unpackedFilename = blockChunks[i].unpackedFilename;
		releaseBlockchunkMutex();

		append_to_file(utfil, unpackedFilename);
		remove(unpackedFilename);

		lockBlockchunkMutex();
		free(blockChunks[i].unpackedFilename);
		free(blockChunks[i].packedFilename);
		releaseBlockchunkMutex();
	}
	fclose(utfil);
}

//-----------------------------------------

void blockUnpackAndReplace(wchar_t* src) {
		
		const wchar_t tmp[100] = { 0 };		
		get_temp_filew(tmp, L"blockpacker_unpackandreplace");
		block_unpack(src, tmp);
		my_renamew(tmp, src);
	
}




