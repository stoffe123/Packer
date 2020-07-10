#ifndef RLE_SIMPLE_PACKER_H
#define RLE_SIMPLE_PACKER_H
#include "memfile.h"

memfile* RleSimplePack(memfile* src);

void RLE_simple_pack(const char* src, const char* dst);

memfile* RleSimpleUnpack(memfile* m);

void RLE_simple_unpack(const char* src, const char* dst);


void RLE_simple_packw(const wchar_t* srcw, const wchar_t* dstw);

void RLE_simple_unpackw(const wchar_t* src, const wchar_t* dst);

#endif