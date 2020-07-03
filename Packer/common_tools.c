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



//Global for all threads
uint64_t filename_count = 1000000;

uint64_t get_file_size(const FILE* f) {
	fseek(f, 0, SEEK_END);
	long long res = ftell(f);
	fseek(f, 0, SEEK_SET);
	return res;
}

uint64_t get_file_size_from_name(const char* name) {
	const FILE* f = fopen(name, "r");
	if (f == NULL) {
		printf("\n get_file_size_from_name: can't find file: %s", name);
		exit(0);
	}
	long long res = get_file_size(f);
	fclose(f);
	return res;
}

uint64_t get_file_size_from_wname(wchar_t* name) {
	//wprintf(L"\n get_file_size_from_wname:%s", name);
	FILE* f;
	errno_t err = _wfopen_s(&f, name, L"rb");
	if (err != 0) {
		wprintf(L"\n get_file_size_from_wname: can't find file: %s", name);
		exit(0);
	}
	uint64_t res = get_file_size(f);
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


void copy_chunk(FILE* source_file, const char* dest_filename, uint64_t size_to_copy) {
	FILE* out = fopen(dest_filename, "wb");

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

bool contains(const wchar_t* s, const wchar_t* find) {
	wchar_t* b = wcsstr(s, find);
	return (b != NULL);
}

uint64_t indexOfChar(const wchar_t* s, const wchar_t find) {
	for (int i = 0; i < wcslen(s); i++) {
		if (s[i] == find) {
			return i;
		}
	}
	return -1;
}

void substringAfter(const wchar_t* dst, const wchar_t* s, const wchar_t* find) {
	wchar_t* b = wcsstr(s, find);
	wcscpy(dst, L"");
	if (b != NULL) {
		wcscpy(dst, b + 1);
	}
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
		wprintf(L"\n Common_tools.c : can't find infile %s", filename);
		exit(1);
	}
	return in;
}

void copy_chunkw(FILE* source_file, wchar_t* dest_filename, uint64_t size_to_copy) {
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

void copy_the_rest(FILE* in, const char* dest_filename) {
	FILE* out = fopen(dest_filename, "wb");
	if (out == NULL) {
		printf("\n Common_tools.c : can't find dest %s", dest_filename); exit(0);
	}
	uint8_t ch;
	while (fread(&ch, 1, 1, in) == 1) {
		fwrite(&ch, 1, 1, out);
	}
	fclose(out);
}

void copy_file(const char* src, const char* dst) {
	FILE* f = fopen(src, "rb");
	copy_the_rest(f, dst);
	fclose(f);
}

void append_to_file(FILE* main_file, const char* append_filename) {
	FILE* append_file = fopen(append_filename, "rb");
	if (append_file == NULL) {
		printf("\n Failed to open file: %s", append_filename);
		exit(1);
	}
	int ch;
	while ((ch = fgetc(append_file)) != EOF) {
		fputc(ch, main_file);
	}
	fclose(append_file);
}

void append_to_filew(FILE* main_file, wchar_t* append_filename) {
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

void get_rand(const char* s) {
	int_to_string(s, filename_count++);
}

void get_randw(const wchar_t* s) {
	int_to_stringw(s, filename_count++);
}

void getTempFile(const char* dst, const char* s) {

	lockTempfileMutex();
	char number[40] = { 0 };
	get_rand(number);
	concat3(dst, TEMP_DIR, s, number);
	releaseTempfileMutex();
	
}

void get_temp_filew(const wchar_t* dst, const wchar_t* s) {
	lockTempfileMutex();
	wchar_t number[40] = { 0 };
	get_randw(number);
	concat3w(dst, TEMP_DIRW, s, number);
	releaseTempfileMutex();
}

void get_clock_dir(const char* dir) {
	lockClockdirMutex();
	const char number[30] = { 0 };
	get_rand(number);
	concat(dir, TEMP_DIR, number);
	releaseClockdirMutex();
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

bool dirs_equalw(const wchar_t* dir1, const wchar_t* dir2) {
	uint32_t count = countDirectoryFiles(dir1, 0);
	uint32_t count2 = countDirectoryFiles(dir2, 0);

	if (count != count2) {
		printf("\n dirs_equal: number of files differed... %d %d", count, count2);
		return false;
	}
	file_t* fileList1 = malloc(count * sizeof(file_t));
	storeDirectoryFilenames(fileList1, dir1, 0);
	bubbleSort(fileList1, count);

	file_t* fileList2 = malloc(count * sizeof(file_t));
	storeDirectoryFilenames(fileList2, dir2, 0);
	bubbleSort(fileList2, count);
	bool result = true;
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
			result = false;
			break;
		}
		if (fileList1[i].size != fileList2[i].size) {
			wprintf(L"\n file nr %d differed by size %d <=> %d", i, fileList1[i].size, fileList2[i].size);
			result = false;
			break;
		}
	}
	if (result) {
		for (int i = 0; i < count; i++) {
			if (!files_equalw(fileList1[i].name, fileList2[i].name)) {
				result = false;
				break;
			}
		}
	}
	free(fileList1);
	free(fileList2);
	return result;
}

bool files_equal(const char* f1, const char* f2) {
	wchar_t u1[500], u2[500];
	toUni(u1, f1);
	toUni(u2, f2);
	return files_equalw(u1,u2);
}

bool files_equalw( wchar_t* name1,  wchar_t* name2) {
	FILE* f1 = openRead(name1);	
	FILE* f2 = openRead(name2);

	long f1_size = get_file_size(f1);
	long f2_size = get_file_size(f2);
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
