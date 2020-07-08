#ifndef MULTIPACKER_H
#define MULTIPACKER_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

#include "packer_commons.h"
#include "seq_packer.h"  
#include "common_tools.h"
//#include "huffman2.h"
#include "canonical.h"

uint8_t multiPack(const char* , const char* , packProfile profile, packProfile seqlenProfile,
	packProfile offsetProfile, packProfile distancesProfile);

void multiUnpack(const char* src, const char* dst, uint8_t pack_type);

void multi_pack(const char*, const char*, packProfile profile, packProfile seqlenProfile, 
	packProfile offsetProfile, packProfile distancesProfile);

void multi_unpack(const char* src, const char* dst);

void multi_unpackw(const wchar_t* srcw, const wchar_t* dstw);

void multi_packw(const wchar_t*, const wchar_t*, packProfile profile, packProfile seqlenProfile,
	packProfile offsetProfile, packProfile distancesProfile);

void MultiUnpackAndReplacew(const wchar_t* srcw);

bool isCanonicalHeaderPacked(int packType);

int packTypeForCanonicalHeaderPack();

#endif