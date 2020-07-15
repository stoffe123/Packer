#ifndef SEQ_PACKER_H   
#define SEQ_PACKER_H

#include "common_tools.h"
#include "packer_commons.h"
#include "memfile.h"
#include "seq_packer_commons.h"

seqPackBundle seqPackSep(memfile* mem, packProfile profile);

memfile* seqUnpack(seqPackBundle m);

#endif