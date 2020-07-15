#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <stringapiset.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "common_tools.h"
#include "block_packer.h"
#include "archive_packer.h"



//Global for all threads
uint64_t filename_count = 1000000;

void myRename(const wchar_t* f_old, const wchar_t* f_new) {
	if (!equalsw(f_old, f_new)) {
		_wremove(f_new);
		_wrename(f_old, f_new);
	}
}


size_t to_narrow(const wchar_t* src, char* dest) {
	size_t i;
	wchar_t code;

	i = 0;

	while (src[i] != '\0') {
		code = src[i];
		if (code < 128)
			dest[i] = (char)code;
		else {
			dest[i] = '?';
			if (code >= 0xD800 && code <= 0xD8FF)
				// lead surrogate, skip the next code unit, which is the trail
				i++;
		}
		i++;
	}
	dest[i] = '\0';
	return i - 1;
}

uint64_t getFileSize(const FILE* f) {
	fseek(f, 0, SEEK_END);
	long long res = ftell(f);
	fseek(f, 0, SEEK_SET);
	return res;
}

uint64_t getFileSizeFromName(wchar_t* name) {
	FILE* f;
	errno_t err = _wfopen_s(&f, name, L"rb");
	if (err != 0) {
		wprintf(L"\n get_file_size_from_wname: can't find file: %s", name);
		exit(0);
	}
	uint64_t res = getFileSize(f);
	fclose(f);
	return res;
}

unsigned char isKthBitSet(unsigned char value, unsigned char bit)
{
	return (value & (1 << bit));
}

unsigned char setKthBit(unsigned char value, unsigned char bit)
{
	// kth bit of n is being set by this operation 
	return ((1 << bit) | value);
}

unsigned char clearKthBit(unsigned char value, unsigned char bit)
{
	// kth bit of n is being set by this operation 
	return (value & (~(1 << bit)));
}

unsigned char setKthBitToVal(unsigned char value, unsigned char bit, unsigned char bitValue) {
	if (bitValue == 0) {
		return clearKthBit(value, bit);
	}
	else {
		return setKthBit(value, bit);
	}
}


bool contains(const wchar_t* s, const wchar_t* find) {
	wchar_t* b = wcsstr(s, find);
	return (b != NULL);
}

int64_t indexOfChar(const wchar_t* s, const wchar_t find) {
	for (int i = 0; i < wcslen(s); i++) {
		if (s[i] == find) {
			return i;
		}
	}
	return -1;
}

void substringAfterLast(const wchar_t* dst, const wchar_t* s, const wchar_t* find) {
	for (int64_t i = wcslen(s) - 1; i >= 0; i--) {
		if (s[i] == find[0]) {
			s += (i + 1);
			wcscpy(dst, s);
			return;
		}
	}
	wcscpy(dst, L"");	
}

FILE* openWrite(const wchar_t* filename) {
	FILE* out;
	errno_t err = _wfopen_s(&out, filename, L"wb");
	if (err != 0) {
		wprintf(L"\n Common_tools.c : can't create outfile %s", filename);
		exit(1);
	}
	return out;
}

FILE* openRead(const wchar_t* filename) {
	FILE* in;
	errno_t err = _wfopen_s(&in, filename, L"rb");
	if (err != 0) {
		wprintf(L"\n Common_tools.c : can't find infile '%s'", filename);
		exit(1);
	}
	return in;
}

void copyFileChunkToFile(FILE* source_file, wchar_t* dest_filename, uint64_t size_to_copy) {
	FILE* out = openWrite(dest_filename);
	
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
	wprintf(L"\n append_to_filew: %s", append_filename);
	FILE* append_file = openRead(append_filename); 		
	int ch;
	while ((ch = fgetc(append_file)) != EOF) {
		fputc(ch, main_file);
	}
	fclose(append_file);
}

void assert(uint64_t x, const char* msg) {
	if (!x) {
		printf("\n\n\a ASSERTION FAILURE: %s", msg);
		exit(0);
	}
}

void assertEqual(uint64_t x, uint64_t y, const char* msg) {
	if (x != y) {
		printf("\n\n\a ASSERTION FAILURE: %d != %d \n %s", x, y, msg);
		exit(0);
	}
}

void assertSmallerOrEqual(uint64_t x, uint64_t y, const char* msg) {
	if (x > y) {
		printf("\n\n\a ASSERTION FAILURE: %d <= %d \n %s", x, y, msg);
		exit(0);
	}
}

void WRITE(FILE* ut, uint64_t c)
{
	fwrite(&c, 1, 1, ut);
}

void get_randw(const wchar_t* s) {
	int_to_stringw(s, filename_count++);
}

void get_temp_filew(const wchar_t* dst, const wchar_t* s) {
	lockTempfileMutex();
	wchar_t number[40] = { 0 };
	get_randw(number);
	concat3w(dst, TEMP_DIRW, s, number);
	releaseTempfileMutex();
}

void concat(const char* dst, const char* s1, const char* s2) {
	const size_t s1_length = strlen(s1);
	strcpy(dst, s1);
	strcpy(dst + s1_length, s2);
}

void concatw(wchar_t* dst, wchar_t* s1, wchar_t* s2) {
	wcscpy(dst, s1);
	wcscat(dst, s2);
}

void concat3w(wchar_t* dst, wchar_t* s1, wchar_t* s2, wchar_t* s3) {
	wcscpy(dst, s1);
	wcscat(dst, s2);
	wcscat(dst, s3);
}

void concat4w(wchar_t* dst, wchar_t* s1, wchar_t* s2, wchar_t* s3, wchar_t* s4) {
	wcscpy(dst, s1);
	wcscat(dst, s2);
	wcscat(dst, s3);
	wcscat(dst, s4);
}

void concat3(const char* dst, const char* s1, const char* s2, const char* s3) {
	const size_t s1_length = strlen(s1);
	const size_t s2_length = strlen(s2);
	strcpy(dst, s1);
	strcpy(dst + s1_length, s2);
	strcpy(dst + s1_length + s2_length, s3);
}

void concat_int(const char* dst, const char* s1, int i) {
	const char number[30] = { 0 };
	int_to_string(number, i);
	concat(dst, s1, number);
}

void concat_intw(const wchar_t* dst, const wchar_t* s1, int i) {
	const wchar_t number[30] = { 0 };
	int_to_stringw(number, i);
	concatw(dst, s1, number);
}

void int_to_string(const char* s, int64_t i) {
	_itoa(i, s, 10);
}

void int_to_stringw(const wchar_t* s, int64_t i) {
	_itow(i, s, 10);
}

bool equals(const char* s1, const char* s2) {
	return strcmp(s1, s2) == 0;
}

bool equalsw(const wchar_t* s1, const wchar_t* s2) {
	return wcscmp(s1, s2) == 0;
}


void toUni(const wchar_t* dst, const char* str) {	
	MultiByteToWideChar(CP_ACP, 0, str, -1, dst, 500);	
}

void deleteAllFilesInDir(const wchar_t* sDir) {
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
				wprintf(L"removing Directory: %s\n", sPath);
				deleteAllFilesInDir(sPath); //Recursion, I love it! 
				RemoveDirectory(sPath);
			}
			else {				
			    _wremove(sPath);								
			}
		}
	} while (FindNextFile(hFind, &fdFile)); //Find the next file. 
	FindClose(hFind); //Always, Always, clean things up! 	
}

/*
count and store should be done in one go

sorting could be done more effecient
or even sort when inserting instead of afterwards

*/

bool dirsEqual(const wchar_t* dir1, const wchar_t* dir2) {
	fileListAndCount_t res = storeDirectoryFilenames(dir1);
	file_t* fileList1 = res.fileList;
	uint64_t count = res.count;

	res = storeDirectoryFilenames(dir2);
	file_t* fileList2 = res.fileList;
	uint64_t count2 = res.count;

	if (count != count2) {
		printf("\n dirs_equal: number of files differed... %d %d", count, count2);
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
		if (!equalsw(n1 , n2)) {
			wprintf(L"\n file nr %d differed by name %s <=> %s", i, n1, n2);
			dirsAreEqual = false;
			break;
		}
		if (fileList1[i].size != fileList2[i].size) {
			wprintf(L"\n file nr %d differed by size %d <=> %d", i, fileList1[i].size, fileList2[i].size);
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
