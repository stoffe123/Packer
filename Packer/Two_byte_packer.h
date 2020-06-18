#ifndef TWO_BYTE_PACKER_H
#define TWO_BYTE_PACKER_H

#include "common_tools.h"
#include "packer_commons.h"

void twoBytePack(const wchar_t* src, const wchar_t* dest, packProfile prof);

void two_byte_pack(const char* src, const char* dest, packProfile prof);

void twoByteUnpack(const wchar_t* src, const wchar_t* dest);

void two_byte_unpack(const char* src, const char* dest);

#endif