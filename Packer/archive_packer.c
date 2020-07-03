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

#pragma comment(lib, "User32.lib")


#include "block_packer.h"

void substring(const wchar_t* dst, const wchar_t* src, uint64_t m, uint64_t n) 
{
	// get length of the destination string
	uint64_t len = n - m;

	// start with m'th char and copy 'len' chars into destination
	wcsncpy(dst, (src + m), len);
}





uint64_t storeDirectoryFilenames(file_t* fileList, const wchar_t* sDir, uint64_t j)
{

	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;

	wchar_t sPath[2048];

	//Specify a file mask. *.* = We want everything! 
	wsprintf(sPath, L"%s\\*.*", sDir);

	if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
	{
		wprintf(L"Path not found: [%s]\n", sDir);
		exit(0);
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
				j = storeDirectoryFilenames(fileList, sPath, j); //Recursion, I love it! 
			}
			else {

				//wprintf(L"\nstoreDirectoryFilenames: %d %s", j, sPath);
				wcscpy(fileList[j].name, sPath);
				fileList[j].size = get_file_size_from_wname(sPath);
				j++;
			}
		}
	} while (FindNextFile(hFind, &fdFile)); //Find the next file. 

	FindClose(hFind); //Always, Always, clean things up! 	
	return j;
}

uint64_t countDirectoryFiles(const wchar_t* sDir, uint64_t j)
{
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;

	wchar_t sPath[2048];

	//Specify a file mask. *.* = We want everything! 
	wsprintf(sPath, L"%s\\*.*", sDir);

	if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
	{
		wprintf(L"Path not found: [%s]\n", sDir);
		exit(1);
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
			wsprintf(sPath, L"%s\\%s", sDir, fdFile.cFileName);

			//Is the entity a File or Folder? 
			if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//wprintf(L"Directory: %s\n", sPath);
				j = countDirectoryFiles(sPath, j); //Recursion, I love it! 
			}
			else {
				j++;
			}
		}
	} while (FindNextFile(hFind, &fdFile)); //Find the next file. 

	FindClose(hFind); //Always, Always, clean things up! 

	return j;
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

void tmpDirNameOf(const wchar_t* tmpBlocked, const wchar_t* name, const wchar_t* dir) {	
	wchar_t* onlyName = name + wcslen(dir);
	concatw(tmpBlocked, TEMP_DIRW, onlyName);
}

void archiveTar(wchar_t* dir, const wchar_t* dest, bool solid, packProfile profile) {

	uint32_t count = countDirectoryFiles(dir, 0);
	printf("\n mallocing for count %d", count);
	file_t* fileList = malloc(count * sizeof(file_t));
	storeDirectoryFilenames(fileList, dir, 0);
	bubbleSort(fileList, count);

	printf("\n archiveTar count=%d", count);

	wchar_t* outFilename = dest;
	if (!solid) {		
		//pack all files and put them in TEMP_DIR
		for (int i = 0; i < count; i++) {
			const wchar_t tmpBlocked[500] = { 0 };
			tmpDirNameOf(tmpBlocked, fileList[i].name, dir);
			block_pack(fileList[i].name, tmpBlocked, profile);
			fileList[i].size = get_file_size_from_wname(tmpBlocked);
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
		const wchar_t headerPackedFilename[100] = { 0 };
		get_temp_filew(headerPackedFilename, L"archivepacker_headerpacked");
		packProfile headerProfile = getPackProfile();
		multi_packw(outFilename, headerPackedFilename, headerProfile, headerProfile, headerProfile, headerProfile);
		out = openWrite(dest);
		fwrite(&count, sizeof(uint32_t), 1, out);
		uint64_t size = get_file_size_from_wname(headerPackedFilename);
		if (size > 65535) {
			printf("\n archivepacker header too big %d", size);
			exit(1);
		}
		fwrite(&size, 2, 1, out);
		append_to_filew(out, headerPackedFilename);
		_wremove(headerPackedFilename);
		_wremove(outFilename);
	}

	//write contents of files	
	for (int i = 0; i < count; i++) {
		if (solid) {
			append_to_filew(out, fileList[i].name);
		}
		else {
			const wchar_t tmpBlocked[500] = { 0 }; 
			tmpDirNameOf(tmpBlocked, fileList[i].name, dir);
			append_to_filew(out, tmpBlocked);
			_wremove(tmpBlocked);
		}		
	}
	fclose(out);
	free(fileList);
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

file_t* readHeader(FILE* in, char* dir, uint64_t count) {
	file_t* filenames = malloc(count * sizeof(file_t));
		
	// Read sizes
	//printf("\n");
	for (int i = 0; i < count; i++) {
		fread(&filenames[i].size, sizeof(uint64_t), 1, in);
		printf("\n size nr %d: %d", i, filenames[i].size);
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
		uint16_t headerSize;
		fread(&headerSize, 2, 1, in);
		const char headerPacked[500] = { 0 };
		get_temp_filew(headerPacked, L"archiveuntar_headerpacked");
		const char headerUnpacked[500] = { 0 };
		get_temp_filew(headerUnpacked, L"archiveuntar_headerunpacked");

		copy_chunkw(in, headerPacked, headerSize);
		multi_unpackw(headerPacked, headerUnpacked);
		FILE* headerFile = openRead(headerUnpacked);
		filenames = readHeader(headerFile, dir, count);
		fclose(headerFile);
		_wremove(headerPacked);
		_wremove(headerUnpacked);
	}
	else {
		filenames = readHeader(in, dir, count);
	}

	// Read files
	for (int i = 0; i < count; i++) {
		createDirs(filenames[i].name, dir);
		copy_chunkw(in, filenames[i].name, filenames[i].size);
		if (!solid) {
			blockUnpackAndReplace(filenames[i].name);
		}
	}
	fclose(in);
	free(filenames);	
}

bool use_solid = false;

void archive_pack(const wchar_t* dir, const wchar_t* dest, packProfile profile) {
	const wchar_t tmp[100] = { 0 };
	get_temp_filew(tmp, L"archivedest");
	
	if (use_solid) {
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

	if (use_solid) {
		block_unpack(src, tmp);
		archiveUntar(tmp, dir, true);
		_wremove(tmp);
	}
	else {
		archiveUntar(src, dir, false);
	}
}


