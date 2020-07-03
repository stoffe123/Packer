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


void archiveTar(wchar_t* dir, const wchar_t* dest) {

	uint32_t count = countDirectoryFiles(dir, 0);
	printf("\n mallocing for count %d", count);
	file_t* fileList = malloc(count * sizeof(file_t));
	storeDirectoryFilenames(fileList, dir, 0);
	bubbleSort(fileList, count);

	printf("\n archiveTar count=%d", count);

	FILE* out = openWrite(dest);	
	fwrite(&count, sizeof(uint32_t), 1, out);

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

	//write contents of files	
	for (int i = 0; i < count; i++) {
		append_to_filew(out, fileList[i].name);
	}
	fclose(out);
	free(fileList);
}

void createDirs(wchar_t* wstr, wchar_t* dir) {
	wchar_t* str = wstr + wcslen(dir);
	if (str[0] == '\\' || str[0] == '/') {
		str++;
	}
	int ind = indexOfChar(str, '\\');
	if (ind == -1) {
		ind = indexOfChar(str, '/');
	}
	if (ind > 0) {
		const wchar_t dirName[500] = { 0 };
		substring(dirName, str, 0, ind + 1);
		const wchar_t wholeDir[500] = { 0 };
		concatw(wholeDir, dir, dirName);

		_wmkdir(wholeDir);
		createDirs(wstr, wholeDir);
	}
}

void archiveUntar(const wchar_t* src, wchar_t* dir) {

	FILE* in = openRead(src);
	uint32_t count;

	fread(&count, sizeof(uint32_t), 1, in);

	const wchar_t** filenames = malloc(count * sizeof(uint64_t));
	for (int i = 0; i < count; i++) {
		filenames[i] = malloc(500 * sizeof(wchar_t));
	}
	uint64_t* sizes = malloc(count * sizeof(uint64_t));

	// Read sizes
	//printf("\n");
	for (int i = 0; i < count; i++) {
		fread(&sizes[i], sizeof(uint64_t), 1, in);
		//printf("\n size nr %d: %d", i, sizes[i]);
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
		wprintf(L"\n");
		filename[i] = 0;
		wcscpy(filenames[readNames], dir);
		wcscat(filenames[readNames], filename);
		readNames++;
	}

	// Read files
	for (int i = 0; i < count; i++) {
		createDirs(filenames[i], dir);
		copy_chunkw(in, filenames[i], sizes[i]);
	}
	fclose(in);
	free(filenames);
	free(sizes);
}


void archive_pack(const wchar_t* dir, const wchar_t* dest, packProfile profile) {
	const wchar_t tmp[100] = { 0 };
	get_temp_filew(tmp, L"archivedest");
	archiveTar(dir, tmp);
	block_pack(tmp, dest, profile);
	_wremove(tmp);
}

void archive_unpack(const wchar_t* src, wchar_t* dir) {
	const wchar_t tmp[100] = { 0 };
	get_temp_filew(tmp, L"archiveunp");
	block_unpack(src, tmp);
	archiveUntar(tmp, dir);
	_wremove(tmp);
}


