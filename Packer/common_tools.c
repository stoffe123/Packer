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
#include "file_tools.h"




//Global for all threads
uint64_t filename_count = 1000000;

void myRename(const wchar_t* f_old, const wchar_t* f_new) {
	if (!equalsw(f_old, f_new)) {
		_wremove(f_new);
		_wrename(f_old, f_new);
	}
}

void checkForErr13(errno_t err) {
	if (err == 13) {
		wprintf(L"\n\n This error code indicates that the current user did not have permission to access a file. This error usually occurs if the user does not have read or write permission on a file (e.g. PERSON.RRN) or a directory (e.g. the directory containing .RRN files). It may also occur if the user doesn't have execute permission on a program.");
		wprintf(L"\n\n Did you in code forget to close the file before accessing it via its name?");
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
	long int pos = ftell(f);
	fseek(f, 0, SEEK_END);
	uint64_t res = ftell(f);
	fseek(f, pos, SEEK_SET);
	return res;
}

uint64_t getSizeLeftToRead(const FILE* f) {
    long int pos = ftell(f);
	fseek(f, 0, SEEK_END);
	uint64_t size = ftell(f);
	fseek(f, pos, SEEK_SET);
	return size-pos;
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

void substringAfterLast(const wchar_t* dst, const wchar_t* sourceStr, const wchar_t* find) {
	for (int64_t i = wcslen(sourceStr) - 1; i >= 0; i--) {
		if (sourceStr[i] == find[0]) {
			sourceStr += (i + 1);
			wcscpy(dst, sourceStr);
			return;
		}
	}
	wcscpy(dst, L"");	
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

void assert(uint64_t x, const char* msg) {
	if (!x) {
		printf("\n\n\a ASSERTION FAILURE: %s", msg);
		myExit();
	}
}

void myExit() {
	uint8_t* x = 0;
	x[5] = 7;
}

void assertEqual(uint64_t x, uint64_t y, const char* msg) {
	if (x != y) {
		printf("\n\n\a ASSERTION FAILURE: %d != %d \n %s", x, y, msg);
		myExit();
	}
}

void assertSmallerOrEqual(uint64_t x, uint64_t y, const char* msg) {
	if (x > y) {
		printf("\n\n\a ASSERTION FAILURE: %d <= %d \n %s", x, y, msg);
		myExit();
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
	
	wchar_t number[40] = { 0 };
	get_randw(number);
	concat3w(dst, TEMP_DIRW, s, number);

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
				//wprintf(L"removing Directory: %s\n", sPath);
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

uint8_t getByteAtPos(uint64_t bytes, int pos)
{
	return (bytes >> (8 * pos)) & 0xff;
}

void setByteAtPos(uint64_t* bytes, uint8_t byte, int pos) {
	*bytes &= ~((uint64_t)0xff << (8 * pos)); // Clear the current byte
	*bytes |= ((uint64_t)byte << (8 * pos)); // Set the new byte
}

