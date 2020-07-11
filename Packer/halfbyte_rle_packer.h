#ifndef CANONICAL_HEADER_PACKER_H
#define CANONICAL_HEADER_PACKER_H

#include "memfile.h"

void halfbyte_rle_pack(const char* source_filename, const char* dest_filename, int kind);

void halfbyte_rle_unpack(const char* source_filename, const char* dest_filename, int kind);

memfile* halfbyteRlePack(memfile* mem, int kind);

memfile* halfbyteRleUnpack(memfile* mem, int kind);


#endif