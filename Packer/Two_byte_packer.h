#ifndef TWO_BYTE_PACKER_H
#define TWO_BYTE_PACKER_H

#include "common_tools.h"
#include "packer_commons.h"
#include "memfile.h"

memfile* twoBytePack(memfile* m, packProfile prof);

memfile* twoByteUnpack(memfile* m);

void two_byte_packw(const wchar_t* src, const wchar_t* dest, packProfile prof);

void two_byte_unpackw(const wchar_t* src, const wchar_t* dest);


#endif