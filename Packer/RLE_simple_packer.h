#ifndef RLE_SIMPLE_PACKER_H
#define RLE_SIMPLE_PACKER_H
#include <Windows.h>


void RLE_simple_pack(const char* src, const char* dest);

void RLE_simple_pack_separate(const char* src, const char* dest, const char* base_dir);

DWORD WINAPI thread_RLE_simple_pack(LPVOID lpParam);


#endif