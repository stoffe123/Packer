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

#pragma comment(lib, "User32.lib")


#include "block_packer.h"

static uint64_t fileListInitialAllocSize = 8192;

static bool useSolid = true;

static packProfile headerPackProfile = {
	.rle_ratio = 90,
			.twobyte_ratio = 79,
			.recursive_limit = 20,
			.twobyte_threshold_max = 8721,
			.twobyte_threshold_divide = 1383,
			.twobyte_threshold_min = 1000,
			.seqlenMinLimit3 = 188,
			.seqlenMinLimit4 = 42151,
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



fileListAndCount_t storeDirectoryFilenamesInternal(const wchar_t* sDir, fileListAndCount_t f)
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
				f = storeDirectoryFilenamesInternal(sPath, f); //Recursion, I love it! 				
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

fileListAndCount_t storeDirectoryFilenames(const wchar_t* dir) {

	fileListAndCount_t f;
	f.fileList = malloc(fileListInitialAllocSize * sizeof(file_t));
	f.count = 0;
	f.allocSize = fileListInitialAllocSize;
	return storeDirectoryFilenamesInternal(dir, f);
}

void createHeader(FILE* out, wchar_t* dir, file_t* fileList, uint32_t count) {

	//write sizes
	for (int i = 0; i < count; i++) {
		uint64_t size = fileList[i].size;
		fwrite(&size, sizeof(uint64_t), 1, out);
		//printf("\n writing size for %d as %d", i, size);
	}

	//write names separated by /n
	int lengthOfDirName = wcslen(dir);

	printf("\n ********* storing names!!! **********\n");
	for (int i = 0; i < count; i++) {
		//wprintf(L"%s  ,  %d\n", fileList[i].name, fileList[i].size);
		//fprintf(out, L"%s\n", fileList[i].name);

		int j = 0;
		wchar_t ch;

		int skip = lengthOfDirName;
		while ((ch = fileList[i].name[j]) != 0) {
			if (skip >= 0) {
				skip--;
			}
			else {
				//wprintf(L"%c", ch);
				fputwc(ch, out);
			}
			j++;
		}
		//wprintf(L"\n");
		fputwc(0, out);
	}
}

/*
creates the dirs in fullPath
that are not in existingDir(which should be a prefix of fullPath)

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
		const wchar_t dirToCreate[500] = { 0 };
		substring(dirToCreate, partAfterExistingDir, 0, ind + 1);
		const wchar_t dirToCreateFullPath[500] = { 0 };
		concatw(dirToCreateFullPath, existingDir, dirToCreate);

		_wmkdir(dirToCreateFullPath);
		createDirs(fullPath, dirToCreateFullPath);
	}
}

void tmpDirNameOf(const wchar_t* tmpBlocked, const wchar_t* name, const wchar_t* dir) {
	wchar_t* onlyName = name + wcslen(dir);
	concatw(tmpBlocked, TEMP_DIRW, onlyName);
	createDirs(tmpBlocked, TEMP_DIRW);
}

void archiveTar(wchar_t* dir, const wchar_t* dest, bool solid, packProfile profile) {

	fileListAndCount_t res = storeDirectoryFilenames(dir);
	file_t* fileList = res.fileList;
	uint32_t count = res.count;
	quickSortCompareEndings(fileList, count);

	printf("\n archiveTar count=%d", count);

	wchar_t* outFilename = dest;
	if (!solid) {
		//pack all files and put them in TEMP_DIR
		for (int i = 0; i < count; i++) {
			const wchar_t tmpBlocked[500] = { 0 };
			tmpDirNameOf(tmpBlocked, fileList[i].name, dir);
			block_pack(fileList[i].name, tmpBlocked, profile);
			fileList[i].size = getFileSizeFromName(tmpBlocked);
		}
		const wchar_t tmp[100] = { 0 };
		get_temp_filew(tmp, L"archivepacker_header");
		outFilename = tmp;
	}
	FILE* out = openWrite(outFilename);
	if (solid) {
		fwrite(&count, sizeof(uint32_t), 1, out);
	}
	createHeader(out, dir, fileList, count);

	if (!solid) {
		fclose(out);
		const wchar_t headerPackedFilename1[100] = { 0 };
		get_temp_filew(headerPackedFilename1, L"archivepacker_everyother");
		const wchar_t headerPackedFilename2[100] = { 0 };
		get_temp_filew(headerPackedFilename2, L"archivepacker_multipacked");
		//everyOtherEncode(outFilename, headerPackedFilename1);
		multi_packw(outFilename, headerPackedFilename2, headerPackProfile, headerPackProfile, headerPackProfile, headerPackProfile);
		out = openWrite(dest);
		fwrite(&count, sizeof(uint32_t), 1, out);
		uint32_t headerSize = getFileSizeFromName(outFilename);
		uint32_t headerPackedSize = getFileSizeFromName(headerPackedFilename2);
		printf("\n archive header packed from %d to %d", headerSize, headerPackedSize);
		if (headerPackedSize > UINT32_MAX) {
			printf("\n archivepacker header too big %d", headerPackedSize);
			exit(1);
		}
		//max size of header is UINT32_max
		fwrite(&headerPackedSize, sizeof(uint32_t), 1, out);
		appendFileToFile(out, headerPackedFilename2);
		_wremove(headerPackedFilename2);
		_wremove(headerPackedFilename1);
		_wremove(outFilename);
	}

	//write contents of files	
	const wchar_t tmpBlocked[500] = { 0 }; //put here for perfomance
	for (int i = 0; i < count; i++) {
		const wchar_t* name = fileList[i].name;
		if (solid) {
			wprintf(L"\n append_to_filew: %s", name);
			appendFileToFile(out, name);
		}
		else {
			tmpDirNameOf(tmpBlocked, name, dir);
			appendFileToFile(out, tmpBlocked);
			_wremove(tmpBlocked);
		}
	}
	fclose(out);
	free(fileList);
}



file_t* readHeader(FILE* in, char* dir, uint64_t count) {
	file_t* filenames = malloc(count * sizeof(file_t));

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
			wprintf(L"%c", ch);
			filename[i++] = ch;
		}
		wprintf(L"\n");
		filename[i] = 0;
		wcscpy(filenames[readNames].name, dir);
		wcscat(filenames[readNames].name, filename);
		readNames++;
	}
	return filenames;
}

void archiveUntar(const wchar_t* src, wchar_t* dir, bool solid) {

	FILE* in = openRead(src);
	uint32_t count;

	fread(&count, sizeof(uint32_t), 1, in);
	file_t* filenames;
	if (!solid) {
		uint32_t headerSize;
		fread(&headerSize, sizeof(uint32_t), 1, in);
		const char headerPacked[500] = { 0 };
		get_temp_filew(headerPacked, L"archiveuntar_headerpacked");
		const char headerUnpacked1[500] = { 0 };
		get_temp_filew(headerUnpacked1, L"archiveuntar_headerunpackedhalf");
		const char headerUnpacked2[500] = { 0 };
		get_temp_filew(headerUnpacked2, L"archiveuntar_headerunpacked");

		copyFileChunkToFile(in, headerPacked, headerSize);
		multi_unpackw(headerPacked, headerUnpacked2);
		//everyOtherDecode(headerUnpacked1, headerUnpacked2);
		FILE* headerFile = openRead(headerUnpacked2);
		filenames = readHeader(headerFile, dir, count);
		fclose(headerFile);
		_wremove(headerPacked);
		_wremove(headerUnpacked1);
		_wremove(headerUnpacked2);
	}
	else {
		filenames = readHeader(in, dir, count);
	}

	// Read files
	for (int i = 0; i < count; i++) {
		createDirs(filenames[i].name, dir);
		copyFileChunkToFile(in, filenames[i].name, filenames[i].size);
		if (!solid) {
			blockUnpackAndReplace(filenames[i].name);
		}
	}
	fclose(in);
	free(filenames);
}

void archive_pack(const wchar_t* dir, const wchar_t* dest, packProfile profile) {
	const wchar_t tmp[100] = { 0 };
	get_temp_filew(tmp, L"archivedest");

	if (useSolid) {
		archiveTar(dir, tmp, true, profile);
		block_pack(tmp, dest, profile);
		_wremove(tmp);
	}
	else {
		//separate packing
		archiveTar(dir, dest, false, profile);
	}
}

void archive_unpack(const wchar_t* src, wchar_t* dir) {
	const wchar_t tmp[100] = { 0 };
	get_temp_filew(tmp, L"archiveunp");

	if (useSolid) {
		block_unpack(src, tmp);
		archiveUntar(tmp, dir, true);
		_wremove(tmp);
	}
	else {
		archiveUntar(src, dir, false);
	}
}


