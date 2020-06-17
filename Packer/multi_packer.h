#ifndef MULTIPACKER_H
#define MULTIPACKER_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

#include "packer_commons.h"
#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "common_tools.h"
//#include "huffman2.h"
#include "canonical.h"

uint8_t multiPack(const char* , const char* , packProfile profile, packProfile seqlenProfile, packProfile offsetProfile);

void multiUnpack(const char* src, const char* dst, int packType);

void multi_pack(const char*, const char*, packProfile profile, packProfile seqlenProfile, packProfile offsetProfile);

void multi_unpack(const char* src, const char* dst);

#endif