#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <tchar.h> 
#include <strsafe.h>
#include "seq_packer.h"  
#include "common_tools.h"
#include "multi_packer.h"
#include "packer_commons.h"
#include "block_packer.h"
#include "everyOtherEncoder.h"
#include <inttypes.h>
#include "file_tools.h"
#include "block_packer.h"
#include "archive_packer.h"

#pragma comment(lib, "User32.lib")


static packProfile headerPackProfile = {
	        .rle_ratio = 90,
			.twobyte_ratio = 79,
			.recursive_limit = 20,
			.twobyte_threshold_max = 8721,
			.twobyte_threshold_divide = 1383,
			.twobyte_threshold_min = 1000,
			.seqlenMinLimit3 = 188,
		
			.blockSizeMinus = 155,
			.winsize = 97340,
			.sizeMaxForCanonicalHeaderPack = 253,
			.sizeMinForSeqPack = 20,
			.sizeMinForCanonical = 20,
			.sizeMaxForSuperslim = 16384
};


void substring(const wchar_t* dst, const wchar_t* src, uint64_t m, uint64_t n)
{
	// get length of the destination string
	uint64_t len = n - m;

	// start with m'th char and copy 'len' chars into destination
	wcsncpy(dst, (src + m), len);
}

void diffSizes(fileListAndCount_t dirInfo) {
	file_t* fileList = dirInfo.fileList;
	uint64_t last = 0;
	for (int i = 0; i < dirInfo.count; i++) {
		uint64_t size = fileList[i].size;
		uint64_t diff = size - last;
		printf("\n %ls %lld %lld", fileList[i].name, size, diff);
		last = size;
		fileList[i].size = diff;
	}
}

void unDiffSizes(fileListAndCount_t dirInfo) {
	file_t* fileList = dirInfo.fileList;
	uint64_t last = 0;
	for (int i = 0; i < dirInfo.count; i++) {
		fileList[i].size += last;
		last = fileList[i].size;		
	}
}

memfile* createSizesHeader(wchar_t* dir, fileListAndCount_t dirInfo) 
{ 
	file_t* fileList = dirInfo.fileList;
	memfile* out = getEmptyMem(L"archivepacker_createsizes");
	//write sizes

	for (int k = 0; k < 8; k++) {

		for (int i = 0; i < dirInfo.count; i++) {
			//todo write MSB together and so on...
			//uint64_t size = fileList[i].size;
			uint8_t byte = getByteAtPos(fileList[i].size, k);
			memWrite(&byte, 1, out);
			//printf("\n writing size for %d as %d", i, size);
		}
	}
	return out;
}

memfile* createNamesHeader(wchar_t* dir, fileListAndCount_t dirInfo) {	


	file_t* fileList = dirInfo.fileList;
	uint64_t count = dirInfo.count;
	memfile* out = getEmptyMem(L"archivepacker_names_header_mem");
	//write names separated by 0
	int lengthOfDirName = wcslen(dir);

	printf("\n ********* Storing dir names **********\n");
	for (int i = 0; i < count; i++) {
		//wprintf(L"%s  ,  %d\n", fileList[i].name, fileList[i].size);
		//fprintf(out, L"%s\n", fileList[i].name);

		int j = lengthOfDirName + 1;

		wchar_t cc = fileList[i].name[j];
		uint8_t multiByteStr[2000] = { 0 };
		int size = wcharEncode(&(fileList[i].name[j]), &multiByteStr);

		// the last char of the name is special
	    // 0 = normal end
	    // 1-31  means a pointer to an equal file follows on the next byte
	    // 1 = 0-255 , 2 = 256-511 , 3 = 512-767  etc
		uint64_t index = findEqualFileIndex(dirInfo, i);
		if (index < 256) {
			//TODO support for higher indexes by paging
			multiByteStr[size++] = 1;
			multiByteStr[size] = index;			
		}
		memWrite(&multiByteStr, size, out);

		//wprintf(L"\n");
		//use 8-bit size and special value for 16-bit size
		if (i < (count - 1)) {
			fputcc(0, out);
		}
	}
	return out;
}

/*
creates the dirs in fullPath
that are not in existingDir(which should be a prefix of fullPath)

TODO: extract to file tools 
*/
void createMissingDirs(wchar_t* fullPath, wchar_t* existingDir) {
	wchar_t* partAfterExistingDir = fullPath + wcslen(existingDir);
	if (partAfterExistingDir[0] == '\\' || partAfterExistingDir[0] == '/') {
		partAfterExistingDir++;
	}
	int64_t ind = indexOfChar(partAfterExistingDir, '\\');
	if (ind == -1) {
		ind = indexOfChar(partAfterExistingDir, '/');
	}
	if (ind > 0) {
		const wchar_t dirToCreate[2000] = { 0 };
		substring(dirToCreate, partAfterExistingDir, 0, ind + 1);
		const wchar_t dirToCreateFullPath[2000] = { 0 };
		concatw(dirToCreateFullPath, existingDir, dirToCreate);

		int res = _wmkdir(dirToCreateFullPath);
		//if res = err  etc
		createMissingDirs(fullPath, dirToCreateFullPath);
	}
}

void tmpDirNameOf(const wchar_t* tmpBlocked, const wchar_t* name, const wchar_t* dir) {
	wchar_t* onlyName = name + wcslen(dir);
	concatw(tmpBlocked, TEMP_DIRW, onlyName);
	createMissingDirs(tmpBlocked, TEMP_DIRW);
}

memfile* createPackedSizesHeader(wchar_t* dir, fileListAndCount_t dirInfo) {
	
	memfile* sizesHeader = createSizesHeader(dir, dirInfo);

	return multiPackAndStorePackType(sizesHeader, headerPackProfile, headerPackProfile,
		headerPackProfile, headerPackProfile);			
}

memfile* createPackedNamesHeader(wchar_t* dir, fileListAndCount_t dirInfo) {
		
	memfile* namesHeader = createNamesHeader(dir, dirInfo);
	//myExit();
	
	return multiPackAndStorePackType(namesHeader, headerPackProfile, headerPackProfile,
		headerPackProfile, headerPackProfile);
}

void writeArchiveHeader(FILE* out, fileListAndCount_t dirInfo, wchar_t* dir, uint8_t archiveType) {

	bool solid = (archiveType == 0);
	if (archiveType == TYPE_SEPARATED) {
		diffSizes(dirInfo);
	}	
	memfile* sizesHeader = createPackedSizesHeader(dir, dirInfo);
	memfile* namesHeader = createPackedNamesHeader(dir, dirInfo);

	uint64_t headerNamesPackedSize = getMemSize(namesHeader)
		, headerSizesPackedSize = getMemSize(sizesHeader);
	printf("\n Archive header sizes packed to %lld", headerSizesPackedSize);		
	printf("\n Archive header names packed to %lld", headerNamesPackedSize);
	printf("\n Total packed header size is  %lld", headerNamesPackedSize + headerSizesPackedSize);
	printf("\n NOW WRITING ARCHIVETYPE %d", archiveType);
	fwrite(&archiveType, 1, 1, out);

	if (headerNamesPackedSize > UINT32_MAX || headerSizesPackedSize > UINT32_MAX) {
		printf("\n\n Archivepacker header too big!");
		exit(1);
	}
	//max size of header is UINT32_max
	writeDynamicSize(headerSizesPackedSize, out);
	writeDynamicSize(headerNamesPackedSize, out);
		
	append_mem_to_file(out, sizesHeader);
	append_mem_to_file(out, namesHeader);
}

uint64_t determineNumberOfBlobs(fileListAndCount_t dirInfo) {

	file_t* fileList = dirInfo.fileList;
	uint64_t count = dirInfo.count;
	uint64_t blobCount = 0;

	wchar_t ext[20] = { 0 };
	wchar_t next_ext[20] = { 0 };

	bool breakAndCreateBlob;
	for (int i = 0; i < count; i++) {
		wchar_t* filename = fileList[i].name;
		//printf("\n%ls of size: %lld", filename, fileList[i].size);
		getFileExtension(ext, filename);
		if (i == count - 1) {
			breakAndCreateBlob = true;
		}
		else {
			getFileExtension(next_ext, fileList[i + 1].name);
			breakAndCreateBlob = !equalsw(next_ext, ext);
		}
		if (breakAndCreateBlob) {
			//pack and flush file and start over	
			blobCount++;					
		}
	}
	return blobCount;
}

void archivePackSemiSeparated(FILE* out, packProfile profile, fileListAndCount_t dirInfo) {

	file_t* fileList = dirInfo.fileList;
	uint64_t count = dirInfo.count;
	printf("\n Start archive pack semiseparated case");
	// write sizes header	
	wchar_t ext[50] = { 0 };
	wchar_t next_ext[50] = { 0 };

	const wchar_t blobFilename[100] = { 0 };
	get_temp_filew(blobFilename, L"archivepack_blob");
	FILE* blobFile = openWrite(blobFilename);	
	uint64_t blobSizes[1000] = { 0 };
	uint64_t blobCount = 0;
	bool breakAndCreateBlob;
	for (int i = 0; i < count; i++) {
		wchar_t* filename = fileList[i].name;
		printf("\n%ls of size: %lld", filename, fileList[i].size);
		getFileExtension(ext, filename);
		if (i == count - 1) {
			breakAndCreateBlob = true;
		}
		else {
			getFileExtension(next_ext, fileList[i + 1].name);
			breakAndCreateBlob = !equalsw(next_ext, ext);
		}	
		appendFileToFile(blobFile, filename);
		if (breakAndCreateBlob) {
			//pack and flush file and start over
			fclose(blobFile);		
			blockPackAndReplace(blobFilename, profile);
			uint64_t size = getFileSizeFromName(blobFilename);
			printf("\n writing blob nr %llu size as %llu", blobCount, size);
			fwrite(&size, sizeof(uint64_t), 1, out);
			appendFileToFile(out, blobFilename);
			blobSizes[blobCount++] = getFileSizeFromName(blobFilename);
			printf("\n Blob nr %lld has size %lld and for extension '%ls'", blobCount - 1, blobSizes[blobCount - 1], ext);					    			
			if (i < count - 1) {
				blobFile = openWrite(blobFilename); // start over
			}
		}
	}
	fclose(out);
}

void archivePackInternal(wchar_t* dir, const wchar_t* dest, packProfile profile) {

	//archiveType   0 = solid   1 = semi-separated   2 = separated
	printf("\n Starting archive pack for %ls archiveType %lld", dir, profile.archiveType);
	uint8_t archiveType = profile.archiveType;
	bool solid = (archiveType == TYPE_SOLID);
	fileListAndCount_t dirInfo = storeDirectoryFilenames(dir, archiveType != TYPE_SEPARATED);
	file_t* fileList = dirInfo.fileList;
	int64_t count = dirInfo.count;

	
	printf("\n Count = %lld", count);

	if (archiveType == TYPE_SEPARATED) {
		
		//pack all files and put them in TEMP_DIR
		for (int i = 0; i < count; i++) {
			printf("\n Non-solid case packing %ls of size: %lld", fileList[i].name, fileList[i].size);
			const wchar_t tmpBlocked[500] = { 0 };
			tmpDirNameOf(tmpBlocked, fileList[i].name, dir);
			block_pack(fileList[i].name, tmpBlocked, profile);
			fileList[i].size = getFileSizeFromName(tmpBlocked);
		}
		quickSortOnSizes(fileList, count);
	}
	else {
		quickSortCompareEndings(fileList, count);
	}
	
	FILE* out = openWrite(dest);
	writeArchiveHeader(out, dirInfo, dir, archiveType);

	if (archiveType == TYPE_SEMISEPARATED) {
		archivePackSemiSeparated(out, profile, dirInfo);
		fclose(out);
		return;
	}

	//_wremove(headerPackedFilename1);
	//Write contents of files	

	//used for non-solid
	const wchar_t tmpBlockedFilename[1000] = { 0 }; //put here for perfomance
	
	//used for solid
	const wchar_t blobFilename[100] = { 0 };
	FILE* blobFile = NULL;
	if (solid) {
	   get_temp_filew(blobFilename, L"archivepack_blob");
	   blobFile = openWrite(blobFilename);
	}
	
	printf("\n Phase 2 of archive pack count=%lld", count);
	for (int i = 0; i < count; i++) {
		const wchar_t* filename = fileList[i].name;
	    printf("\n Appending %ls with size %lld", filename, fileList[i].size);
		if (solid) {
			appendFileToFile(blobFile, filename);
		}
		else {			
			tmpDirNameOf(tmpBlockedFilename, filename, dir);
			appendFileToFile(out, tmpBlockedFilename);
			_wremove(tmpBlockedFilename);
		}
	}
	if (solid) {
		fclose(blobFile);
		blockPackAndReplace(blobFilename, profile);
		appendFileToFile(out, blobFilename);
		_wremove(blobFilename);
	}
	fclose(out);	
	free(fileList);
}


fileListAndCount_t readSizesHeader(memfile* in, int archiveType) {
	fileListAndCount_t dirInfo;	
	rewindMem(in);	
	uint64_t count = getMemSize(in) / 8;
	dirInfo.fileList = malloc(count * sizeof(file_t));	
	for (int i = 0; i < count; i++) {
		dirInfo.fileList[i].size = 0;
	}
	if (dirInfo.fileList == NULL) {
		printf("\n out of memory in archive_packer.readHeader!!");
		myExit();
	}
		
	for (int k = 0; k < 8; k++) {
		for (int i = 0; i < count; i++) {			
			setByteAtPos(&(dirInfo.fileList[i].size), fgetcc(in), k);
		}
	}
	dirInfo.count = count;
	if (archiveType == TYPE_SEPARATED) {
		unDiffSizes(dirInfo);
	}
	return dirInfo;
}

int wcharEncode(wchar_t* wcharBuf, uint8_t* codedBuf) {
	int wcharBufPos = 0;
	int codedBufPos = 0;
	wchar_t ch;
	while ((ch = wcharBuf[wcharBufPos++]) != 0) {
		if (ch <= 127) {
			codedBuf[codedBufPos++] = (uint8_t)ch;
		}
		else {
			codedBuf[codedBufPos++] = 128;
			codedBuf[codedBufPos++] = (ch / 256);
			codedBuf[codedBufPos++] = (ch % 256);
			//TODO wchar_t could be 32-bit
		}
	}	
	codedBuf[codedBufPos] = 0;
	return codedBufPos;
}

int wcharDecode(FILE* in, wchar_t* wcharBuf) {
	int wcharBufPos = 0;
	int codedBufPos = 0;
	int ch;
	while ((ch = fgetc(in)) > 0) {
		if (ch <= 127) {
			wcharBuf[wcharBufPos++] = (wchar_t)ch;
		}
		else {			
			int msb = fgetc(in);
			int lsb = fgetc(in);
			wcharBuf[wcharBufPos++] = (wchar_t)(msb * 256 + lsb);
		}
	}
	wcharBuf[wcharBufPos++] = 0;
	return codedBufPos;
}


void readNamesHeader(FILE* in, char* dir, fileListAndCount_t* list) {
	file_t* filenames = list->fileList;
	if (filenames == NULL) {
		printf("\n out of memory in archive_packer.readHeader!!");
		myExit();
		return;
	}

	// Read names
	int readNames = 0;
	wchar_t temp_wstr[2000] = { 0 };
	temp_wstr[0] = '/';
	while (readNames < list->count) {
		wcharDecode(in, &temp_wstr[1]);
		
		wcscpy(filenames[readNames].name, dir);
		wcscat(filenames[readNames].name, temp_wstr);
		printf("\n result: %ls", filenames[readNames].name);
	
		readNames++;
	}	
}


fileListAndCount_t readPackedSizesHeader(FILE* in, uint32_t headerSize, int archiveType) {
	
	printf("\n Read packed sizes Header size: %d", headerSize);
		
	memfile* headerPackedMem = getEmptyMem(L"archiveunpack_headersizespacked");
	copy_chunk_to_mem(in, headerPackedMem, headerSize);
	memfile* headerUnpacked = multiUnpack(headerPackedMem);
	//everyOtherDecode(headerUnpacked1, headerUnpacked2);
	
	fileListAndCount_t res =  readSizesHeader(headerUnpacked, archiveType);

	return res;
}

void readPackedNamesHeader(FILE* in, wchar_t* dir, uint32_t headerSize, fileListAndCount_t* fileList) {	
	
	printf("\n Read packed Names Header size: %d", headerSize);

	//const wchar_t headerUnpacked1[100] = { 0 };
	//get_temp_filew(headerUnpacked1, L"archiveuntar_headerunpackedhalf");
	const wchar_t headerUnpacked[200] = { 0 };
	get_temp_filew(headerUnpacked, L"archiveuntar_headernamesunpacked");

	//const wchar_t headerUnpacked1[200] = { 0 };
	//get_temp_filew(headerUnpacked1, L"archiveuntar_headernamesunpacked1");


	//TODO should pack to memfile instead
	multiUnpackBlockToFile(in, headerUnpacked, headerSize);
	
	//everyOtherDecode(headerUnpacked1, headerUnpacked);
	
	FILE* headerFile = openRead(headerUnpacked);
	readNamesHeader(headerFile, dir, fileList);

	fclose(headerFile);
	_wremove(headerUnpacked);		
}

void archiveUnpackSemiSeparated(FILE* in, fileListAndCount_t dirInfo, wchar_t* dir) {

	uint64_t nrOfBlobs = determineNumberOfBlobs(dirInfo);

	const wchar_t masterFilename[200] = { 0 };
	get_temp_filew(masterFilename, L"archive_unpack_semisep_master");

	FILE* masterFile = openWrite(masterFilename);


	for (uint64_t blobNr = 0; blobNr < nrOfBlobs; blobNr++) {
		uint64_t size;
		fread(&size, sizeof(uint64_t), 1, in);
		
		const wchar_t blobFilename[200] = { 0 };
		get_temp_filew(blobFilename, L"archive_unpack_blob_semisep");
				
		printf("\narchiveUnpackSemiSeparated : size of blob nr %llu is %llu", blobNr, size);
		copyFileChunkToFile(in, blobFilename, size);
		blockUnpackNameToFile(blobFilename, masterFile);
		_wremove(blobFilename);
	}
	fclose(masterFile);
	fclose(in);
	masterFile = openRead(masterFilename);
	file_t* filenames = dirInfo.fileList;

	//TODO DRY this with code below
	for (int i = 0; i < dirInfo.count; i++) {
		//TODO: do the cat of dir and name here instead of passing dir to readPackedNamesHeader above
		createMissingDirs(filenames[i].name, dir);
		printf("\n Reading: %ls sized:%" PRId64, filenames[i].name, filenames[i].size);
		copyFileChunkToFile(masterFile, filenames[i].name, filenames[i].size);
	}
	_wremove(masterFilename);
}

void archiveUnpackInternal(const wchar_t* src, wchar_t* dir) {

	printf("\n *** Archive Unpack *** ");		
	uint8_t archiveType;

	FILE* in = openRead(src);	
	fread(&archiveType, 1, 1, in);
	printf("\n Archive type: %d", archiveType);
	
	uint64_t headerSizesPackedSize = readDynamicSize(in);
	uint64_t headerNamesPackedSize = readDynamicSize(in); 
				
	fileListAndCount_t fileList = readPackedSizesHeader(in, headerSizesPackedSize, archiveType);
	readPackedNamesHeader(in, dir, headerNamesPackedSize, &fileList);	

	printf("\n Unpacked and read headers successfully! Now unpacking files...");
	
	file_t* filenames = fileList.fileList;
	int64_t count = fileList.count;
	const wchar_t blockUnpackFilename[200] = { 0 };

	if (archiveType == TYPE_SEMISEPARATED) {
		archiveUnpackSemiSeparated(in, fileList, dir);
		return;
	}
	else if (archiveType == TYPE_SOLID) {
		get_temp_filew(blockUnpackFilename, L"blockunpacksolidblob");
		block_unpack_file(in, blockUnpackFilename);
		fclose(in);
		in = openRead(blockUnpackFilename);
	}

	for (int i = 0; i < count; i++) {
		//TODO: do the cat of dir and name here instead of passing dir to readPackedNamesHeader above
		createMissingDirs(filenames[i].name, dir);
		printf("\n Reading: %ls sized:%" PRId64, filenames[i].name, filenames[i].size);
		copyFileChunkToFile(in, filenames[i].name, filenames[i].size);
		if (archiveType == TYPE_SEPARATED) {			
			blockUnpackAndReplace(filenames[i].name);
		}
	}
	fclose(in);
	free(filenames);
	if (archiveType == TYPE_SOLID) {
		_wremove(blockUnpackFilename);
	}
}


void archive_pack(const wchar_t* dir, const wchar_t* dest, packProfile profile) {
    archivePackInternal(dir, dest, profile);			
}

void archive_unpack(const wchar_t* src, wchar_t* dir) {
	archiveUnpackInternal(src, dir);	
}


