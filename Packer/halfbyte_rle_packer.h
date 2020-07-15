#ifndef CANONICAL_HEADER_PACKER_H
#define CANONICAL_HEADER_PACKER_H

#include "memfile.h"

memfile* halfbyteRlePack(memfile* mem, int kind);

memfile* halfbyteRleUnpack(memfile* mem, int kind);


#endif