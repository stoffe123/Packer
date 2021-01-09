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



memfile* createSizesHeader(wchar_t* dir, file_t* fileList, uint32_t count) {
	memfile* out = getEmptyMem(L"archivepacker_createsizes");

	//write sizes

	for (int k = 0; k < 8; k++) {

		for (int i = 0; i < count; i++) {
			//todo write MSB together and so on...
			//uint64_t size = fileList[i].size;
			uint8_t byte = getByteAtPos(fileList[i].size, k);
			memWrite(&byte, 1, out);
			//printf("\n writing size for %d as %d", i, size);
		}
	}
	return out;
}

memfile* createNamesHeader(wchar_t* dir, file_t * fileList, uint32_t count) {	

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

memfile* createPackedSizesHeader(wchar_t* dir, file_t* fileList, int64_t count) {
	
	memfile* sizes = createSizesHeader(dir, fileList, count);

	return multiPackAndStorePackType(sizes, headerPackProfile, headerPackProfile,
		headerPackProfile, headerPackProfile);			
}

memfile* createPackedNamesHeader(wchar_t* dir, file_t* fileList, int64_t count) {
		
	memfile* header = createNamesHeader(dir, fileList, count);	
	//myExit();
	
	return multiPackAndStorePackType(header, headerPackProfile, headerPackProfile,
		headerPackProfile, headerPackProfile);
}

void writeArchiveHeader(FILE* out, file_t* fileList, wchar_t* dir, int64_t count, uint8_t archiveType) {
	
	memfile* sizes = createPackedSizesHeader(dir, fileList, count);
	memfile* header = createPackedNamesHeader(dir, fileList, count);

	uint64_t headerNamesPackedSize = getMemSize(header)
		, headerSizesPackedSize = getMemSize(sizes);
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
	
	

	append_mem_to_file(out, sizes);
	append_mem_to_file(out, header);
}

void archivePackInternal(wchar_t* dir, const wchar_t* dest, packProfile profile) {

	printf("\n Starting archive pack for %ls archiveType %lld", dir, profile.archiveType);
	bool solid = (profile.archiveType == 0);
	
	fileListAndCount_t dirFilesAndCount = storeDirectoryFilenames2(dir, solid);
	file_t* fileList = dirFilesAndCount.fileList;
	int64_t count = dirFilesAndCount.count;
	quickSortCompareEndings(fileList, count);
	printf("\n Count = %lld", count);

	if (!solid) {
		//pack all files and put them in TEMP_DIR
		for (int i = 0; i < count; i++) {
			printf("\n Non-solid case packing %ls of size: %" PRId64, fileList[i].name, fileList[i].size);
			const wchar_t tmpBlocked[500] = { 0 };
			tmpDirNameOf(tmpBlocked, fileList[i].name, dir);
			block_pack(fileList[i].name, tmpBlocked, profile);
			fileList[i].size = getFileSizeFromName(tmpBlocked);
		}
	}
	else {
		printf("\n  IT WAS SOLID CASE!");
	}
	
	FILE* out = openWrite(dest);
	writeArchiveHeader(out, fileList, dir, count, profile.archiveType);

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
	
	printf("\n Phase 2 of archive pack count=%" PRId64, count);
	for (int i = 0; i < count; i++) {
		const wchar_t* filename = fileList[i].name;
	    printf("\n Appending %ls with size %" PRId64, filename, fileList[i].size);
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


fileListAndCount_t readSizesHeader(memfile* in) {
	fileListAndCount_t res;	
	rewindMem(in);
	uint64_t count = getMemSize(in) / 8;
	res.fileList = malloc(count * sizeof(file_t));	
	for (int i = 0; i < count; i++) {
		res.fileList[i].size = 0;
	}
	if (res.fileList == NULL) {
		printf("\n out of memory in archive_packer.readHeader!!");
		myExit();
	}
		
	for (int k = 0; k < 8; k++) {
		for (int i = 0; i < count; i++) {			
			setByteAtPos(&(res.fileList[i].size), fgetcc(in), k);
		}
	}
	res.count = count;
	return res;
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


fileListAndCount_t readPackedSizesHeader(FILE* in, uint32_t headerSize) {
	
	printf("\n Read packed sizes Header size: %d", headerSize);
		
	memfile* headerPackedMem = getEmptyMem(L"archiveunpack_headersizespacked");
	copy_chunk_to_mem(in, headerPackedMem, headerSize);
	memfile* headerUnpacked = multiUnpack(headerPackedMem);
	//everyOtherDecode(headerUnpacked1, headerUnpacked2);
	
	fileListAndCount_t res =  readSizesHeader(headerUnpacked);

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

void archiveUnpackInternal(const wchar_t* src, wchar_t* dir) {

	printf("\n *** Archive Unpack *** ");	
	uint8_t archiveType;

	FILE* in = openRead(src);	
	fread(&archiveType, 1, 1, in);
	printf("\n Archive type: %d", archiveType);
	bool solid = (archiveType == 0);
	uint64_t headerSizesPackedSize = readDynamicSize(in);
	uint64_t headerNamesPackedSize = readDynamicSize(in); 
				
	fileListAndCount_t fileList = readPackedSizesHeader(in, headerSizesPackedSize);
	readPackedNamesHeader(in, dir, headerNamesPackedSize, &fileList);	

	printf("\n Unpacked and read headers successfully! Now unpacking files...");
	printf(solid ? "\nSolid case" : "\nNon-solid case");
	

	file_t* filenames = fileList.fileList;
	int64_t count = fileList.count;
	// Read files
	const wchar_t blockUnpackFilename[200] = { 0 };
	if (solid) {	
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
		if (!solid) {
			//TODO: flag for block pack or just multipack (for small files)  ?
			blockUnpackAndReplace(filenames[i].name);
		}
	}
	fclose(in);
	free(filenames);
	if (solid) {
		_wremove(blockUnpackFilename);
	}

}


void archive_pack(const wchar_t* dir, const wchar_t* dest, packProfile profile) {
    archivePackInternal(dir, dest, profile);			
}

void archive_unpack(const wchar_t* src, wchar_t* dir) {
	archiveUnpackInternal(src, dir);	
}


