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

#pragma comment(lib, "User32.lib")


#include "block_packer.h"

static uint64_t fileListInitialAllocSize = 8192;

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



fileListAndCount_t storeDirectoryFilenamesInternal(const wchar_t* sDir, fileListAndCount_t f, bool useSolid)
{
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;

	wchar_t sPath[4096];

	//Specify a file mask. *.* = We want everything! 
	wsprintf(sPath, L"%s\\*.*", sDir);

	if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
	{
		wprintf(L"Path not found: [%s]\n", sDir);
		myExit();
	}

	do
	{
		//Find first file will always return "."
		//    and ".." as the first two directories. 
		if (wcscmp(fdFile.cFileName, L".") != 0
			&& wcscmp(fdFile.cFileName, L"..") != 0)
		{
			//Build up our file path using the passed in 
			//  [sDir] and the file/foldername we just found: 
			/* if (sDir[wcslen(sDir) - 1] == '/') {
				wsprintf(sPath, L"%s%s", sDir, fdFile.cFileName);
			}
			else {*/
			wsprintf(sPath, L"%s\\%s", sDir, fdFile.cFileName);
			//}

			//Is the entity a File or Folder? 
			if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//wprintf(L"Directory: %s\n", sPath);
				f = storeDirectoryFilenamesInternal(sPath, f, useSolid); //Recursion, I love it! 				
			}
			else {
				if (f.count >= f.allocSize) {
					f.allocSize += fileListInitialAllocSize;					
					file_t* newMem = realloc(f.fileList, f.allocSize * sizeof(file_t));					
					if (newMem != NULL) {
						f.fileList = newMem;
					}
					else {
						printf("\n\n Out of memory in archive_packer!");
						myExit();
					}
				}
				//wprintf(L"\nstoreDirectoryFilenames: %d %s", j, sPath);
				wcscpy(f.fileList[f.count].name, sPath);  // use filelist name instead of sPath all the way!
				if (useSolid) {
					f.fileList[f.count].size = getFileSizeFromName(sPath);
				} // if not solid the size of the packed file is written later anyway...
				f.count++;

			}
		}
	} while (FindNextFile(hFind, &fdFile)); //Find the next file. 

	FindClose(hFind); //Always, Always, clean things up! 	
	return f;
}

fileListAndCount_t storeDirectoryFilenames(const wchar_t* dir, bool useSolid) {

	fileListAndCount_t f;
	f.fileList = malloc(fileListInitialAllocSize * sizeof(file_t));
	f.count = 0;
	f.allocSize = fileListInitialAllocSize;
	return storeDirectoryFilenamesInternal(dir, f, useSolid);
}

void createSizesHeader(const wchar_t* filename, wchar_t* dir, file_t* fileList, uint32_t count) {
	FILE* out = openWrite(filename);

	//write sizes

	for (int k = 0; k < 8; k++) {

		for (int i = 0; i < count; i++) {
			//todo write MSB together and so on...
			//uint64_t size = fileList[i].size;
			uint8_t byte = getByteAtPos(fileList[i].size, k);
			fwrite(&byte, 1, 1, out);
			//printf("\n writing size for %d as %d", i, size);
		}
	}
	fclose(out);
}

void createNamesHeader(wchar_t* headerFilename, wchar_t* dir, file_t * fileList, uint32_t count) {
	FILE* out = openWrite(headerFilename);
	//write names separated by 0
	int lengthOfDirName = wcslen(dir);

	printf("\n ********* Storing dir names **********\n");
	for (int i = 0; i < count; i++) {
		//wprintf(L"%s  ,  %d\n", fileList[i].name, fileList[i].size);
		//fprintf(out, L"%s\n", fileList[i].name);

		int j = lengthOfDirName;
		wchar_t ch;
		while ((ch = fileList[i].name[j]) != 0) {			
			//wprintf(L"%c", ch);
			fputwc(ch, out);			
			j++;
		}
		//wprintf(L"\n");
		//use 8-bit size and special value for 16-bit size
		fputwc(0, out);
	}
	fclose(out);
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

void createPackedSizesHeader(wchar_t* headerPackedFilename, wchar_t* dir, file_t* fileList, int64_t count) {
	const wchar_t headerFilename[200] = { 0 };
	get_temp_filew(headerFilename, L"archiveunpack_headersizes");
	
	createSizesHeader(headerFilename, dir, fileList, count);

	//everyOtherEncode(outFilename, headerPackedFilename1);
	multi_packw(headerFilename, headerPackedFilename, headerPackProfile, headerPackProfile,
		headerPackProfile, headerPackProfile);		
	_wremove(headerFilename);
}

void createPackedNamesHeader(wchar_t* headerPackedFilename, wchar_t* dir, file_t* fileList, int64_t count) {
	const wchar_t headerFilename[200] = { 0 };
	get_temp_filew(headerFilename, L"archiveunpack_headernames");
	
	createNamesHeader(headerFilename, dir, fileList, count);	

	//TODO  have a bit for 16 or 8 bit chars in header
	//everyOtherEncode(outFilename, headerPackedFilename1);
	multi_packw(headerFilename, headerPackedFilename, headerPackProfile, headerPackProfile,
		headerPackProfile, headerPackProfile);
	_wremove(headerFilename);
}

void writeArchiveHeader(FILE* out, file_t* fileList, wchar_t* dir, int64_t count, uint8_t archiveType) {
	const wchar_t headerSizesPackedFilename[200] = { 0 };
	get_temp_filew(headerSizesPackedFilename, L"archivepacker_headersizespacked");
	const wchar_t headerNamesPackedFilename[200] = { 0 };
	get_temp_filew(headerNamesPackedFilename, L"archivepacker_headernamespacked");

	createPackedSizesHeader(headerSizesPackedFilename, dir, fileList, count);
	createPackedNamesHeader(headerNamesPackedFilename, dir, fileList, count);

	uint32_t headerNamesPackedSize = getFileSizeFromName(headerNamesPackedFilename)
		, headerSizesPackedSize = getFileSizeFromName(headerSizesPackedFilename);
	printf("\n Archive header sizes packed to %u", headerSizesPackedSize);		
	printf("\n Archive header names packed to %u", headerNamesPackedSize);
	printf("\n Total packed header size is  %u", headerNamesPackedSize + headerSizesPackedSize);

	printf("\n NOW WRITING ARCHIVETYPE %d", archiveType);
	fwrite(&archiveType, 1, 1, out);

	if (headerNamesPackedSize > UINT32_MAX || headerSizesPackedSize > UINT32_MAX) {
		printf("\n\n Archivepacker header too big!");
		exit(1);
	}
	//max size of header is UINT32_max
	fwrite(&headerSizesPackedSize, sizeof(uint32_t), 1, out);
	fwrite(&headerNamesPackedSize, sizeof(uint32_t), 1, out);

	appendFileToFile(out, headerSizesPackedFilename);
	_wremove(headerSizesPackedFilename);
	appendFileToFile(out, headerNamesPackedFilename);
	_wremove(headerNamesPackedFilename);
}

void archivePackInternal(wchar_t* dir, const wchar_t* dest, packProfile profile) {

	printf("\n Starting archive pack archiveType = %" PRId64, profile.archiveType);
	bool solid = (profile.archiveType == 0);
	
	fileListAndCount_t dirFilesAndCount = storeDirectoryFilenames(dir, solid);
	file_t* fileList = dirFilesAndCount.fileList;
	int64_t count = dirFilesAndCount.count;
	quickSortCompareEndings(fileList, count);
	printf("\n Count = %d", count);

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
	    wprintf(L"\n Appending %s with size %" PRId64, filename, fileList[i].size);
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



fileListAndCount_t readSizesHeader(FILE* in) {
	fileListAndCount_t res;	
	uint64_t count = getFileSize(in) / 8;
	res.fileList = malloc(count * sizeof(file_t));	
	for (int i = 0; i < count; i++) {
		res.fileList[i].size = 0;
	}
	if (res.fileList == NULL) {
		printf("\n out of memory in archive_packer.readHeader!!");
		myExit();
	}
	
	uint8_t byte; 
	for (int k = 0; k < 8; k++) {
		for (int i = 0; i < count; i++) {
			fread(&byte, 1, 1, in);
			setByteAtPos(&(res.fileList[i].size), byte, k);
		}
	}
	res.count = count;
	return res;
}

fileListAndCount_t readNamesHeader(FILE* in, char* dir, fileListAndCount_t list) {
	file_t* filenames = list.fileList;
	if (filenames == NULL) {
		printf("\n out of memory in archive_packer.readHeader!!");
		myExit();
	}

	// Read names
	int readNames = 0;
	while (readNames < list.count) {
		wchar_t filename[500] = { 0 };
		wchar_t ch;
		int i = 0;
		while ((ch = fgetwc(in)) != 0) {
			//wprintf(L"%c", ch);
			filename[i++] = ch;
		}
		//wprintf(L"\n");
		filename[i] = 0;
		wcscpy(filenames[readNames].name, dir);
		wcscat(filenames[readNames].name, filename);
		readNames++;
	}
	return list;
}


fileListAndCount_t readPackedSizesHeader(FILE* in, uint32_t headerSize) {
	
	printf("\n Read packed sizes Header size: %d", headerSize);
	const wchar_t headerPacked[200] = { 0 };
	get_temp_filew(headerPacked, L"archiveuntar_headersizespacked");
	//const wchar_t headerUnpacked1[100] = { 0 };
	//get_temp_filew(headerUnpacked1, L"archiveuntar_headerunpackedhalf");
	const wchar_t headerUnpacked[200] = { 0 };
	get_temp_filew(headerUnpacked, L"archiveuntar_headersizesunpacked");

	//TODO unnecessary to copy to a new file.. instead make a method to pack n bytes from IN-stream
	copyFileChunkToFile(in, headerPacked, headerSize);
	multi_unpackw(headerPacked, headerUnpacked);
	//everyOtherDecode(headerUnpacked1, headerUnpacked2);
	FILE* headerFile = openRead(headerUnpacked);
	
	fileListAndCount_t res =  readSizesHeader(headerFile);

	fclose(headerFile);
	_wremove(headerPacked);
	_wremove(headerUnpacked);
	return res;
}

fileListAndCount_t readPackedNamesHeader(FILE* in, wchar_t* dir, uint32_t headerSize, fileListAndCount_t fileList) {	
	
	printf("\n Read packed Names Header size: %d", headerSize);

	//const wchar_t headerUnpacked1[100] = { 0 };
	//get_temp_filew(headerUnpacked1, L"archiveuntar_headerunpackedhalf");
	const wchar_t headerUnpacked[200] = { 0 };
	get_temp_filew(headerUnpacked, L"archiveuntar_headernamesunpacked");

	//TODO should pack to memfile instead
	multiUnpackBlockToFile(in, headerUnpacked, headerSize);
	//everyOtherDecode(headerUnpacked1, headerUnpacked2);
	
	FILE* headerFile = openRead(headerUnpacked);
	fileListAndCount_t res = readNamesHeader(headerFile, dir, fileList);
	fclose(headerFile);

	_wremove(headerUnpacked);	
	return res;
}

void archiveUnpackInternal(const wchar_t* src, wchar_t* dir) {

	printf("\n *** Archive Unpack *** ");
	uint32_t headerSizesPackedSize, headerNamesPackedSize;
	uint8_t archiveType;

	FILE* in = openRead(src);	
	fread(&archiveType, 1, 1, in);
	printf("\n Archive type: %d", archiveType);
	bool solid = (archiveType == 0);
	fread(&headerSizesPackedSize, sizeof(uint32_t), 1, in);
	fread(&headerNamesPackedSize, sizeof(uint32_t), 1, in);
				
	fileListAndCount_t fileList = readPackedSizesHeader(in, headerSizesPackedSize);
	fileList = readPackedNamesHeader(in, dir, headerNamesPackedSize, fileList);	

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


