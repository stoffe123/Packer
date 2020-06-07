#ifndef SEQ_PACKER_H   
#define SEQ_PACKER_H

#include "common_tools.h"
#include "packer_commons.h"

void seq_pack(const char* source_filename, const char* dest_filename, packProfile profile);

void seq_pack_separate(const char* source_filename, const char* dir, packProfile profile);


#endif