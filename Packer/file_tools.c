#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <tchar.h> 
#include <strsafe.h>
#include "file_tools.h"

static uint64_t fileListInitialAllocSize = 8192;

fileListAndCount_t storeDirectoryFilenamesInternal(const wchar_t* sDir, fileListAndCount_t f, bool storeSizes)
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
				f = storeDirectoryFilenamesInternal(sPath, f, storeSizes); //Recursion, I love it! 				
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
				if (storeSizes) {
					f.fileList[f.count].size = getFileSizeFromName(sPath);
				} // if not solid the size of the packed file is written later anyway...
				f.count++;

			}
		}
	} while (FindNextFile(hFind, &fdFile)); //Find the next file. 

	FindClose(hFind); //Always, Always, clean things up! 	
	return f;
}

fileListAndCount_t storeDirectoryFilenames(const wchar_t* dir, bool storeSizes) {

	fileListAndCount_t f;
	f.fileList = malloc(fileListInitialAllocSize * sizeof(file_t));
	f.count = 0;
	f.allocSize = fileListInitialAllocSize;
	return storeDirectoryFilenamesInternal(dir, f, storeSizes);
}


bool filesEqual(wchar_t* name1, wchar_t* name2) {
	FILE* f1 = openRead(name1);
	FILE* f2 = openRead(name2);

	long f1_size = getFileSize(f1);
	long f2_size = getFileSize(f2);
	bool result = true;
	if (f1_size != f2_size) {
		wprintf(L"\n\a >>>>>>>>>>>> FILES NOT EQUAL!!!! <<<<<<<<<<<<<<<< %s and %s", name1, name2);
		printf("\n Lengths differ   %d  %d", f1_size, f2_size);
		result = false;
	}
	unsigned char tmp1, tmp2;

	size_t bytes = 0;
	int count = 0;
	while (fread(&tmp1, 1, 1, f1) && fread(&tmp2, 1, 1, f2)) {
		if (tmp1 != tmp2) {
			printf("\n Contents differ at position  %d ", count);
			printf("\n File1:");
			printf("%c", tmp1);
			//print_string_rep(tmp1);
			printf("\n File2:");
			//print_string_rep(tmp2);
			printf("%c", tmp2);
			return false;
		}
		count++;
	}
	fclose(f1);
	fclose(f2);
	return result;
}

/*
count and store should be done in one go

sorting could be done more effecient
or even sort when inserting instead of afterwards

*/


bool dirsEqual(const wchar_t* dir1, const wchar_t* dir2) {
	fileListAndCount_t dirStruct;
	dirStruct = storeDirectoryFilenames(dir1, true);
	file_t* fileList1 = dirStruct.fileList;
	uint64_t count = dirStruct.count;

	dirStruct = storeDirectoryFilenames(dir2, true);
	file_t* fileList2 = dirStruct.fileList;
	uint64_t count2 = dirStruct.count;

	if (count != count2) {
		printf("\n dirs_equal: number of files differed... %lld %lld", count, count2);
		return false;
	}
	quickSort(fileList1, count);
	quickSort(fileList2, count);

	bool dirsAreEqual = true;
	for (int i = 0; i < count; i++) {
		wchar_t* n1 = fileList1[i].name;
		wchar_t* n2 = fileList2[i].name;
		n1 += wcslen(dir1);
		n2 += wcslen(dir2);
		if (n1[0] == '/') {
			n1++;
		}
		if (n2[0] == '/') {
			n2++;
		}
		if (!equalsw(n1, n2)) {
			printf("\n file nr %d differed by name %ls <=> %ls", i, n1, n2);
			dirsAreEqual = false;
			break;
		}
		if (fileList1[i].size != fileList2[i].size) {
			printf("\n\n File nr %d:'%ls' differed by size %lld <=> %lld",
				i, fileList1[i].name, fileList1[i].size, fileList2[i].size);
			dirsAreEqual = false;
			break;
		}
	}
	if (dirsAreEqual) {
		for (int i = 0; i < count; i++) {
			if (!filesEqual(fileList1[i].name, fileList2[i].name)) {
				dirsAreEqual = false;
				break;
			}
		}
	}
	free(fileList1);
	free(fileList2);
	return dirsAreEqual;
}

void getFileExtension(const wchar_t* dst, const wchar_t* sourceStr) {
	uint64_t len = wcslen(sourceStr);
	for (int64_t i = len - 1; i >= 0; i--) {
		if (sourceStr[i] == L'.') {
			sourceStr += (i + 1);
			wcscpy(dst, sourceStr);
			return;
		}
		//max len for extensions
		if (len - i > 10 || sourceStr[i] == L'\\' || sourceStr[i] == 'L/') {
			break;
		}
	}
	wcscpy(dst, L"");
}


FILE* openWrite(const wchar_t* filename) {
	FILE* out;
	//_wremove(filename);
	errno_t err = _wfopen_s(&out, filename, L"wb");
	if (err != 0) {
		wprintf(L"\n Common_tools.openWrite : can't create outfile %s \nError code %d", filename, err);
		checkForErr13(err);
		exit(1);
	}
	return out;
}



FILE* openRead(const wchar_t* filename) {
	FILE* in;
	errno_t err = _wfopen_s(&in, filename, L"rb");
	if (err != 0) {
		wprintf(L"\n Common_tools.openRead : can't find infile '%s'", filename);
		checkForErr13(err);
		exit(1);
	}
	return in;
}

void copyFileChunkToFile(FILE* source_file, wchar_t* dest_filename, uint64_t size_to_copy) {

	//printf("\n entering copyFileChunkToFile size=%d ", size_to_copy);
	FILE* out = openWrite(dest_filename);
	//printf("\n starting  ..");
	uint8_t ch;
	for (int i = 0; i < size_to_copy; i++) {
		size_t bytes_got = fread(&ch, 1, 1, source_file);
		if (bytes_got == 0) {
			break;
		}
		fwrite(&ch, 1, 1, out);
	}
	fclose(out);
}



void appendFileToFile(FILE* main_file, wchar_t* append_filename) {
	//wprintf(L"\n append_to_filew: %s", append_filename);
	FILE* append_file = openRead(append_filename);
	int ch;
	while ((ch = fgetc(append_file)) != EOF) {
		fputc(ch, main_file);
	}
	fclose(append_file);
}


uint64_t getFileSize(const FILE* f) {
	uint64_t pos = ftell(f);
	fseek(f, 0, SEEK_END);
	uint64_t res = ftell(f);
	fseek(f, pos, SEEK_SET);
	return res;
}

uint64_t getSizeLeftToRead(const FILE* f) {
	uint64_t pos = ftell(f);
	fseek(f, 0, SEEK_END);
	uint64_t size = ftell(f);
	fseek(f, pos, SEEK_SET);
	return size - pos;
}


uint64_t getFileSizeFromName(wchar_t* name) {
	FILE* f;
	errno_t err = _wfopen_s(&f, name, L"rb");
	if (err != 0) {
		wprintf(L"\n\n getFileSizeFromName: can't open file: %s Errno:%d", name, err);
		checkForErr13(err);
		myExit();
	}
	uint64_t res = getFileSize(f);
	if (f != 0) {
		fclose(f);
	}
	return res;
}