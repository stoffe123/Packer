#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <tchar.h> 
#include <strsafe.h>
#pragma comment(lib, "User32.lib")

#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "common_tools.h"
#include "multi_packer.h"
#include "packer_commons.h"
#include "block_packer.h"

typedef struct file_t {
	uint64_t size;
	wchar_t name[2000];
} file_t;

//for tar
static file_t fileList[10000];

// for untar
static char filenames[2000][500];
static uint64_t* sizes[2000];

void storeDirectoryFilenames(const wchar_t* sDir, uint64_t count)
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
	int j = 0;
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
				wprintf(L"Directory: %s\n", sPath);
				//ListDirectoryContents(sPath); //Recursion, I love it! 
			}
			else {

				wprintf(L"\nstoreDirectoryFilenames: %d %s", j, sPath);
				wcscpy(fileList[j].name, sPath);
				fileList[j].size = get_file_size_from_wname(sPath);
				j++;
			}
		}
	} while (FindNextFile(hFind, &fdFile)); //Find the next file. 

	FindClose(hFind); //Always, Always, clean things up! 

}


uint64_t countDirectoryFiles(const wchar_t* sDir)
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
	uint64_t count = 0;
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
				wprintf(L"Directory: %s\n", sPath);
				//ListDirectoryContents(sPath); //Recursion, I love it! 
			}
			else {
				count++;
			}
		}
	} while (FindNextFile(hFind, &fdFile)); //Find the next file. 

	FindClose(hFind); //Always, Always, clean things up! 

	return count;
}
void archiveTar(char* dir, const char* dest) {

	uint32_t count = countDirectoryFiles(dir);
	storeDirectoryFilenames(dir, count);

	FILE* out = fopen(dest, "wb");
	fwrite(&count, sizeof(count), 1, out);

	//write sizes
	for (int i = 0; i < count; i++) {
		uint64_t size = fileList[i].size;
		fwrite(&size, sizeof(size), 1, out);
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
				wprintf(L"%c", ch);
				fputwc(ch, out);
			}
			j++;
		}
		wprintf(L"\n");
		fputwc(0, out);
	}

	//write contents of files	
	for (int i = 0; i < count; i++) {
		append_to_filew(out, fileList[i].name);
	}
	fclose(out);
}


void archiveUntar(const char* src, wchar_t* dir) {

	FILE* in = fopen(src, "rb");
	uint32_t count;

	fread(&count, sizeof(count), 1, in);

	// Read sizes
	for (int i = 0; i < count; i++) {
		fread(&sizes[i], sizeof(sizes[i]), 1, in);
		printf("\n size nr %d: %d", i, sizes[i]);
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
		wcscpy(filenames[readNames], dir);
		wcscat(filenames[readNames], filename);
		readNames++;
	}

	// Read files
	for (int i = 0; i < count; i++) {
		copy_chunkw(in, filenames[i], sizes[i]);
	}
	fclose(in);
}


void archive_pack(const wchar_t* dir, const char* dest, packProfile_t profile) {
	const char tmp[100] = { 0 };
	get_temp_file2(tmp, "archivedest");
	archiveTar(dir, tmp);
	block_pack(tmp, dest, profile);
	remove(tmp);
}

void archive_unpack(const char* src, wchar_t* dir) {
	const char tmp[100] = { 0 };
	get_temp_file2(tmp, "archiveunp");
	block_unpack(src, tmp);
	archiveUntar(tmp, dir);
	remove(tmp);
}


