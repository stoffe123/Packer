#ifndef TWO_BYTE_UNPACKER_H
#define TWO_BYTE_UNPACKER_H

#include "common_tools.h"
#include "packer_commons.h"

void two_byte_unpackw(const wchar_t* src, const wchar_t* dest);

void two_byte_unpack(const char* src, const char* dest);

#endif