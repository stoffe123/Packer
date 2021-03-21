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

// PACK

//not used at the moment
uint8_t multiPackFiles(const wchar_t*, const wchar_t*, completePackProfile profile);

//not used at the moment
void multi_packw(const wchar_t*, const wchar_t*, completePackProfile profile);


uint8_t multiPackAndReturnPackType(memfile* src, memfile* dst, completePackProfile profile);

memfile* multiPack2(memfile* src, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile, packProfile distancesProfile);


memfile* multiPackAndStorePackType(memfile* src, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile, packProfile distancesProfile);

// UNPACK


memfile* multiUnpackAndReplace(memfile* src);

memfile* multiUnpackAndReplaceWithPackType(memfile* src, uint8_t packType);

void multi_unpackw(const wchar_t* srcw, const wchar_t* dstw);

memfile* multiUnpackWithPackType(memfile* m, uint8_t pack_type);

memfile* multiUnpack(memfile* m);

memfile* multiUnpackBlock(FILE* in, uint64_t bytesToRead);

void multiUnpackBlockToFile(FILE* in, wchar_t* dstFilename, uint64_t bytesToRead);



// MISC

bool isHalfByteRlePacked(int packType);

int packTypeForHalfbyteRlePack(int kind);

int packTypeRlePlusTwobyte();

int getHalfbyteKindFromPackType(int packType);

bool isHalfByteRlePacked(int packType);


#endif