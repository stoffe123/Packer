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


static int compareEndings(const wchar_t* s1, const wchar_t* s2) {

	int res;
	char ext1[500] = { 0 };
	char ext2[500] = { 0 };

	substringAfter(ext1, s1, L".");
	substringAfter(ext2, s2, L".");

	if (equalsw(ext1, L"txt")) {
		strcpy(ext1, "zzz");
	}
	if (equalsw(ext2, L"txt")) {
		strcpy(ext2, "zzz");
	}
	if (equalsw(ext1, L"htm")) {
		strcpy(ext1, "zzy");
	}
	if (equalsw(ext2, L"htm")) {
		strcpy(ext2, "zzy");
	}
	if (equalsw(ext1, L"html")) {
		strcpy(ext1, "zzx");
	}
	if (equalsw(ext2, L"html")) {
		strcpy(ext2, "zzx");
	}

	if (equalsw(ext1, L"wav")) {
		strcpy(ext1, "aab");
	}
	if (equalsw(ext2, L"wav")) {
		strcpy(ext2, "aab");
	}

	if (equalsw(ext1, L"exe")) {
		strcpy(ext1, "aaa");
	}
	if (equalsw(ext2, L"exe")) {
		strcpy(ext2, "aaa");
	}



	if (wcslen(ext1) + wcslen(ext2) > 0) {

		res = wcscmp(ext1, ext2);
		if (res) {
			return -res;
		}
	}
	uint64_t size1 = get_file_size_from_wname(s1);
	uint64_t size2 = get_file_size_from_wname(s2);
	if (size1 < size2) {
		return 1;
	}
	if (size1 > size2) {
		return -1;
	}
	return 0;	
}

static void sort(file_t f[], int size)
{
	int i, j, imin;
	file_t temp;
	for (i = 0; i < size; i++) {
		/* sök index för det minsta bland elementen nr i, i+1, … */
		imin = i;
		for (j = i + 1; j < size; j++) {
			if (compareEndings(f[j].name, f[imin].name) < 0) {
				imin = j;
			}
		}
		/* byt element så det minsta hamnar i pos i */
		if (imin != i) {
			temp = f[i]; f[i] = f[imin]; f[imin] = temp;
			//temp = g[i]; g[i] = g[imin]; g[imin] = temp;
		}
	} /* end for i */
}


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
	sort(fileList, count);
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
void archiveTar(wchar_t* dir, const wchar_t* dest) {

	uint32_t count = countDirectoryFiles(dir);
	storeDirectoryFilenames(dir, count);

	FILE* out = openWrite(dest);	
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


void archiveUntar(const wchar_t* src, wchar_t* dir) {

	FILE* in = openRead(src);
	uint32_t count;

	fread(&count, sizeof(count), 1, in);

	const char** filenames = malloc(count * sizeof(uint64_t));
	for (int i = 0; i < count; i++) {
		filenames[i] = malloc(500 * sizeof(char));
	}
	uint64_t* sizes = malloc(count * sizeof(uint64_t));

	// Read sizes
	printf("\n");
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


