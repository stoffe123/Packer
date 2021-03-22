#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_packer.h"  
#include "common_tools.h"
#include "multi_packer.h"
#include "canonical.h"
#include "memfile.h"
#include "file_tools.h"
#include <windows.h>
#include <string.h>
#include <conio.h>
#include <process.h>
#include <inttypes.h>

#define BLOCK_PACK_MAX_THREADS 17

typedef struct blockChunk_t {
	memfile* packed;
	memfile* unpacked;
	completePackProfile profile;
	uint64_t size;
	uint16_t packType;
	uint64_t chunkSize;
	HANDLE  handle;
} blockChunk_t;

__declspec(thread) static packProfile dummyProfile = {
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

defaultOffsetProfile = {
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
.sizeMaxForSuperslim = 16384 },

defaultDistanceProfile = {
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
.sizeMaxForSuperslim = 16384 };


completePackProfile getCompletePackProfileWithDefaults(packProfile prof) {

	completePackProfile res;
	res.main = prof;

	//meta testsuit 1170029  / 33s
	packProfile defaultSeqlenProfile = {
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

		defaultOffsetProfile = {
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
		.sizeMaxForSuperslim = 16384 },

		defaultDistanceProfile = {
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
		.sizeMaxForSuperslim = 16384 };

	res.seqlen = defaultSeqlenProfile;
	res.offset = defaultOffsetProfile;
	res.distance = defaultDistanceProfile;
	return res;
}

__declspec(thread)  static uint64_t read_size;

__declspec(thread) static blockChunk_t blockChunks[5000];

__declspec(thread) static HANDLE blockchunkMutex;


HANDLE lockBlockchunkMutex() {
	WaitForSingleObject(blockchunkMutex, INFINITE);
}

HANDLE releaseBlockchunkMutex() {
	ReleaseMutex(blockchunkMutex);
}

void threadMultiPack(void* pMyID)
{
	lockBlockchunkMutex();
	blockChunk_t* bc = (blockChunk_t*)pMyID;
	releaseBlockchunkMutex();

	uint8_t packType = multiPackAndReturnPackType(bc->unpacked, bc->packed, bc->profile);
	freeMem(bc->unpacked);
	lockBlockchunkMutex();
	bc->packType = packType;
	bc->size = getMemSize(bc->packed);
	if (bc->chunkSize < read_size) {
		bc->size = 0;
	}
	wprintf(L"\n THREAD MULTIPACK FOR %s FINISHED!", getMemName(bc->unpacked));
	releaseBlockchunkMutex();
}


void threadMultiUnpack(void* pMyID)
{
	lockBlockchunkMutex();
	blockChunk_t* bc = (blockChunk_t*)pMyID;
	releaseBlockchunkMutex();

	bc->unpacked = multiUnpackWithPackType(bc->packed, bc->packType, dummyProfile);
	wprintf(L"\n THREAD MULTIUNPACK FOR %s FINISHED!", getMemName(bc->packed));
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


//----------------------------------------------------------------------------------------

void writeBlock(uint64_t i, FILE* utfil) {
	WaitForSingleObject(blockChunks[i].handle, INFINITE);
	lockBlockchunkMutex();
	blockChunk_t blockChunk = blockChunks[i];
	uint32_t size = blockChunk.size;
	uint8_t packType = blockChunk.packType;
	releaseBlockchunkMutex();

	append_to_tar(utfil, blockChunk.packed, size, packType);

	lockBlockchunkMutex();
	freeMem(blockChunk.packed);
	releaseBlockchunkMutex();
}

void block_pack_file_internal(FILE* infil, const wchar_t* dst, FILE* utfil, completePackProfile prof, bool closeInfil) {

	packProfile profile = prof.main;

	uint64_t src_size = getSizeLeftToRead(infil);
	uint64_t writtenCount = 0;
	printf("\n Entering block_pack_file_internal sizelefttoread=%llu", src_size);
	uint64_t chunkSize, chunkNumber = 0;
	bool closeAfter = false;
	if (utfil == NULL) {
		utfil = openWrite(dst);
		closeAfter = true;
	}
	do {
		read_size = BLOCK_SIZE - (profile.blockSizeMinus + 1) * (uint64_t)10000;

		assert(read_size < 16777215, "too large blocksize must be < 16777215");

		//workaround for a bug that even 2 powers cause bug
		//this bug should be fixed but it is hard
		if (read_size % 256 == 0) {
			read_size--;
		}
		printf("\n Real blocksize used %" PRId64, read_size);
		memfile* chunk = getMemfile(read_size, L"blockpackfile_chunk");
		copy_chunk_to_mem(infil, chunk, read_size);
		chunkSize = getMemSize(chunk);

		lockBlockchunkMutex();
		blockChunks[chunkNumber].chunkSize = chunkSize;
		blockChunks[chunkNumber].unpacked = chunk;
		blockChunks[chunkNumber].packed = getMemfile(chunkSize, L"blockpacker_packedchunk");
		blockChunks[chunkNumber].profile = prof;

		releaseBlockchunkMutex();

		printf("\n STARTING THREAD FOR MULTIPACK %llu", chunkNumber);
		HANDLE handle = _beginthread(threadMultiPack, 0, &blockChunks[chunkNumber]);

		lockBlockchunkMutex();
		blockChunks[chunkNumber].handle = handle;
		releaseBlockchunkMutex();
		uint32_t size = getMemSize(blockChunks[chunkNumber].packed);
		if (chunkSize < read_size) {
			size = 0;
		}
		if (chunkNumber >= BLOCK_PACK_MAX_THREADS) {
			if (chunkNumber > BLOCK_PACK_MAX_THREADS * 4 || chunkNumber % 2 == 0) {
				writeBlock(writtenCount++, utfil);
			}
		}

	} while (blockChunks[chunkNumber++].chunkSize == read_size);
	if (closeInfil) {
		fclose(infil);
	}

	while (writtenCount < chunkNumber) {
		writeBlock(writtenCount++, utfil);
	}
	if (closeAfter) {
		fclose(utfil);
	}
}

void block_pack_file_internal2(FILE* infil, const wchar_t* dst, FILE* utfil, packProfile prof, bool closeInfil) {
	completePackProfile compProf = getCompletePackProfileWithDefaults(prof);
	block_pack_file_internal(infil, dst, utfil, compProf, closeInfil);
}

void block_pack_file(FILE* infil, const wchar_t* dst, packProfile profile) {
	block_pack_file_internal2(infil, dst, NULL, profile, false);
}


void block_pack(const wchar_t* src, const wchar_t* dst, packProfile profile) {
	FILE* infil = openRead(src);
	block_pack_file_internal2(infil, dst, NULL, profile, true);
}


void blockPackFull(const wchar_t* src, const wchar_t* dst, completePackProfile pp) {
	FILE* infil = openRead(src);
	block_pack_file_internal(infil, dst, NULL, pp, true);
}


void blockPackToExistingFile(const wchar_t* src, FILE* utfil, packProfile profile) {
	FILE* infil = openRead(src);
	block_pack_file_internal2(infil, NULL, utfil, profile, true);
}


// ----------------------------------------------------------------

void block_unpack_file_internal(FILE* infil, const wchar_t* src, FILE* utfil, const wchar_t* dst) {
	//todo read packtype here and if bit 4 is set don't read size, just read til the end!
	//will save 16*4 = 64 bytes total in test suit 16
	if (infil == NULL) {
		infil = openRead(src);
	}
	uint64_t totalRead = 0, chunkNumber = 0;
	while (true) {

		uint8_t packType;
		if (fread(&packType, 1, 1, infil) == 0) {
			break;
		}
		memfile* tmp = getEmptyMem(L"blockpacker_unpacktmp");
		if (isKthBitSet(packType, 4)) {
			copy_the_rest_to_mem(infil, tmp);
		}
		else {
			uint32_t size = 0;
			//3 bytes can handle block sizes up to 16777216‬
			//note that these are the sizes of the compressed chunks!
			checkAlloc(tmp, size);
			fread(&size, 3, 1, infil);
			copy_chunk_to_mem(infil, tmp, size);
		}
		lockBlockchunkMutex();
		blockChunks[chunkNumber].packType = packType;
		blockChunks[chunkNumber].packed = tmp;
		printf("\n STARTING THREAD FOR MULTIUNPACK %" PRId64, chunkNumber);
		releaseBlockchunkMutex();

		HANDLE handle = _beginthread(threadMultiUnpack, 0, &blockChunks[chunkNumber]);
		//threadMultiUnpack(&blockChunks[chunkNumber]);

		lockBlockchunkMutex();
		blockChunks[chunkNumber].handle = handle;
		releaseBlockchunkMutex();

		chunkNumber++;
	}
	fclose(infil);
	if (dst != NULL) {
		utfil = openWrite(dst);
	}
	for (int i = 0; i < chunkNumber; i++) {
		WaitForSingleObject(blockChunks[i].handle, INFINITE);

		append_mem_to_file(utfil, blockChunks[i].unpacked);

		lockBlockchunkMutex();
		freeMem(blockChunks[i].unpacked);
		freeMem(blockChunks[i].packed);
		releaseBlockchunkMutex();
	}
	if (dst != NULL) {
		fclose(utfil);
	}
}

void block_unpack_file(FILE* infil, const wchar_t* dst) {
	block_unpack_file_internal(infil, NULL, NULL, dst);
}

void blockUnpackFileToFile(FILE* infil, FILE* utfil) {
	block_unpack_file_internal(infil, NULL, utfil, NULL);
}

void blockUnpackNameToFile(const wchar_t* src, FILE* utfil) {
	block_unpack_file_internal(NULL, src, utfil, NULL);
}


void block_unpack(const wchar_t* src, const wchar_t* dst) {

	FILE* infil = openRead(src);
	block_unpack_file(infil, dst);
}


void blockUnpackAndReplace(wchar_t* src) {
	const wchar_t tmp[100] = { 0 };
	get_temp_filew(tmp, L"blockpacker_unpackandreplace");
	block_unpack(src, tmp);
	myRename(tmp, src);
}



void blockPackAndReplace(const wchar_t* src, packProfile profile) {
	const wchar_t tmp[100] = { 0 };
	get_temp_filew(tmp, L"blockpacker_packandreplace");
	block_pack(src, tmp, profile);
	myRename(tmp, src);
}


void blockPackAndReplaceFull(const wchar_t* src, completePackProfile prof) {
	const wchar_t tmp[100] = { 0 };
	get_temp_filew(tmp, L"blockpacker_packandreplace");
	blockPackFull(src, tmp, prof);
	myRename(tmp, src);
}





