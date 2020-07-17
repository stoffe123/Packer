#ifndef MULTIPACKER_H
#define MULTIPACKER_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

#include "packer_commons.h"
#include "seq_packer.h"  
#include "common_tools.h"
#include "canonical.h"

uint8_t multiPackFiles(const wchar_t* , const wchar_t* , packProfile profile, packProfile seqlenProfile,
	packProfile offsetProfile, packProfile distancesProfile);

memfile* multiUnpackAndReplace(memfile* src);

memfile* multiUnpackAndReplaceWithPackType(memfile* src, uint8_t packType);

void multi_unpackw(const wchar_t* srcw, const wchar_t* dstw);

void multi_packw(const wchar_t*, const wchar_t*, packProfile profile, packProfile seqlenProfile,
	packProfile offsetProfile, packProfile distancesProfile);

bool isCanonicalHeaderPacked(int packType);

int packTypeForHalfbyteRlePack(int kind);

int packTypeRlePlusTwobyte();

uint8_t multiPackAndReturnPackType(memfile* src, memfile* dst, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile, packProfile distancesProfile);

memfile* multiPack2(memfile* src, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile, packProfile distancesProfile);

memfile* multiUnpackWithPackType(memfile* m, uint8_t pack_type);

memfile* multiUnpack(memfile* m);

memfile* multiPackAndStorePackType(memfile* src, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile, packProfile distancesProfile);

#endif