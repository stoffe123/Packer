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

uint8_t multiPackAndReturnPackType(memfile* src, memfile* dst, completePackProfile profile);

memfile* multiPackAndStorePackType(memfile* src, completePackProfile profile);

//not used at the moment
uint8_t multiPackFiles(const wchar_t*, const wchar_t*, completePackProfile profile);

//not used at the moment
void multi_packw(const wchar_t*, const wchar_t*, completePackProfile profile);

//not used at the moment
memfile* multiPack(memfile* src, completePackProfile profile);



// UNPACK

memfile* multiUnpackAndReplace(memfile* src, packProfile profile);

memfile* multiUnpackAndReplaceWithPackType(memfile* src, uint8_t packType, packProfile profile);

void multi_unpackw(const wchar_t* srcw, const wchar_t* dstw, packProfile profile);

memfile* multiUnpackWithPackType(memfile* m, uint8_t pack_type, packProfile profile);

memfile* multiUnpack(memfile* m, packProfile profile);

memfile* multiUnpackBlock(FILE* in, uint64_t bytesToRead, packProfile profile);

void multiUnpackBlockToFile(FILE* in, wchar_t* dstFilename, uint64_t bytesToRead, packProfile profile);



// MISC

bool isHalfByteRlePacked(int packType);

int packTypeForHalfbyteRlePack(int kind);

int packTypeRlePlusTwobyte();

int getHalfbyteKindFromPackType(int packType);

bool isHalfByteRlePacked(int packType);


#endif