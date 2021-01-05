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
						printf("\n out of memory in archive_packer!");
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

void createHeader(FILE* out, wchar_t* dir, file_t* fileList, uint32_t count) {

	//write sizes
	for (int i = 0; i < count; i++) {
		//todo write MSB together and so on...
		uint64_t size = fileList[i].size;
		fwrite(&size, sizeof(uint64_t), 1, out);
		//printf("\n writing size for %d as %d", i, size);
	}

	//write names separated by 0
	int lengthOfDirName = wcslen(dir);

	printf("\n ********* storing names!!! **********\n");
	for (int i = 0; i < count; i++) {
		//wprintf(L"%s  ,  %d\n", fileList[i].name, fileList[i].size);
		//fprintf(out, L"%s\n", fileList[i].name);

		int j = lengthOfDirName;
		wchar_t ch;

		//TODO: set j to lengthOfDirName directly instead!!		
		while ((ch = fileList[i].name[j]) != 0) {			
			//wprintf(L"%c", ch);
			fputwc(ch, out);			
			j++;
		}
		//wprintf(L"\n");
		//use 8-bit size and special value for 16-bit size
		fputwc(0, out);
	}
}

/*
creates the dirs in fullPath
that are not in existingDir(which should be a prefix of fullPath)

TODO: extract to file tools 
*/
void createDirs(wchar_t* fullPath, wchar_t* existingDir) {
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
		createDirs(fullPath, dirToCreateFullPath);
	}
}

void tmpDirNameOf(const wchar_t* tmpBlocked, const wchar_t* name, const wchar_t* dir) {
	wchar_t* onlyName = name + wcslen(dir);
	concatw(tmpBlocked, TEMP_DIRW, onlyName);
	createDirs(tmpBlocked, TEMP_DIRW);
}

uint64_t createPackedHeader(wchar_t* headerPackedFilename, wchar_t* dir, file_t* fileList, uint32_t count) {
	const wchar_t headerFilename[500] = { 0 };
	get_temp_filew(headerFilename, L"archiveunpack_headerFilename");

	FILE* headerFile = openWrite(headerFilename);
	createHeader(headerFile, dir, fileList, count);
	fclose(headerFile);

	//const wchar_t headerPackedFilename1[100] = { 0 };
	//get_temp_filew(headerPackedFilename1, L"archivepacker_everyother");

	//TODO  have a bit for 16 or 8 bit chars in header
	//everyOtherEncode(outFilename, headerPackedFilename1);
	multi_packw(headerFilename, headerPackedFilename, headerPackProfile, headerPackProfile,
		headerPackProfile, headerPackProfile);
	uint64_t headerPackedSize = getFileSizeFromName(headerPackedFilename);
	uint64_t headerSize = getFileSizeFromName(headerFilename);
	printf("\n archive header packed from %" PRId64 " to %" PRId64, headerSize, headerPackedSize);
	_wremove(headerFilename);
	return headerPackedSize;
}

void archivePackInternal(wchar_t* dir, const wchar_t* dest, packProfile profile) {

	bool solid = (profile.archiveType == 0);
	fileListAndCount_t res = storeDirectoryFilenames(dir, solid);
	file_t* fileList = res.fileList;
	uint32_t count = res.count;
	quickSortCompareEndings(fileList, count);
	printf("\n archiveTar count=%d", count);

	if (!solid) {
		//pack all files and put them in TEMP_DIR
		for (int i = 0; i < count; i++) {
			const wchar_t tmpBlocked[500] = { 0 };
			tmpDirNameOf(tmpBlocked, fileList[i].name, dir);
			block_pack(fileList[i].name, tmpBlocked, profile);
			fileList[i].size = getFileSizeFromName(tmpBlocked);
		}		
	}
	const wchar_t headerPackedFilename[100] = { 0 };
	get_temp_filew(headerPackedFilename, L"archivepacker_headerpacked");
	uint64_t headerPackedSize = createPackedHeader(headerPackedFilename, dir, fileList, count);

	FILE* out = openWrite(dest);	
	fwrite(&(profile.archiveType), 1, 1, out);
	fwrite(&count, sizeof(uint32_t), 1, out);
		
	if (headerPackedSize > UINT32_MAX) {
		printf("\n\n Archivepacker header too big %" PRId64, headerPackedSize);
		exit(1);
	}
	//max size of header is UINT32_max
	fwrite(&headerPackedSize, sizeof(uint32_t), 1, out);
	appendFileToFile(out, headerPackedFilename);
	_wremove(headerPackedFilename);

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
	
	for (int i = 0; i < count; i++) {
		const wchar_t* filename = fileList[i].name;
	    wprintf(L"\n%s", filename);
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



file_t* readHeader(FILE* in, char* dir, uint64_t count) {
	file_t* filenames = malloc(count * sizeof(file_t));
	if (filenames == NULL) {
		printf("\n out of memory in archive_packer.readHeader!!");
		myExit();
	}

	// Read sizes
	//printf("\n");
	for (int i = 0; i < count; i++) {
		fread(&filenames[i].size, sizeof(uint64_t), 1, in);
		//printf("\n size nr %d: %d", i, filenames[i].size);
	}

	// Read names
	int readNames = 0;
	while (readNames < count) {
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
	return filenames;
}


//TODO  count not needed here since we know the size of the header
file_t* readPackedHeader(FILE* in, wchar_t* dir, uint32_t count) {
	uint32_t headerSize;
	fread(&headerSize, sizeof(uint32_t), 1, in);
	printf("\n Header size: %d", headerSize);
	const char headerPacked[100] = { 0 };
	get_temp_filew(headerPacked, L"archiveuntar_headerpacked");
	//const char headerUnpacked1[100] = { 0 };
	//get_temp_filew(headerUnpacked1, L"archiveuntar_headerunpackedhalf");
	const char headerUnpacked[100] = { 0 };
	get_temp_filew(headerUnpacked, L"archiveuntar_headerunpacked");

	copyFileChunkToFile(in, headerPacked, headerSize);
	multi_unpackw(headerPacked, headerUnpacked);
	//everyOtherDecode(headerUnpacked1, headerUnpacked2);
	FILE* headerFile = openRead(headerUnpacked);
	
	file_t* res =  readHeader(headerFile, dir, count);
	fclose(headerFile);
	_wremove(headerPacked);
	_wremove(headerUnpacked);
	return res;
}

void archiveUnpackInternal(const wchar_t* src, wchar_t* dir) {

	printf("\n *** Archive Unpack *** ");
	FILE* in = openRead(src);
	uint32_t count;
	int archiveType;
	fread(&archiveType, 1, 1, in);
	printf("\n Archive type: %d", archiveType);
	bool solid = (archiveType == 0);
	fread(&count, sizeof(uint32_t), 1, in);
	printf("\n Count: %d", count);
				
	file_t* filenames = readPackedHeader(in, dir, count);
	
	// Read files
	const char blockUnpackFilename[100] = { 0 };
	if (solid) {	
		get_temp_filew(blockUnpackFilename, L"blockUnpack");
		block_unpack_file(in, blockUnpackFilename);
		fclose(in);
		in = openRead(blockUnpackFilename);
	}

	for (int i = 0; i < count; i++) {
		createDirs(filenames[i].name, dir);
		copyFileChunkToFile(in, filenames[i].name, filenames[i].size);
		if (!solid) {
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


