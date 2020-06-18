#ifndef SEQ_PACKER_H   
#define SEQ_PACKER_H

#include "common_tools.h"
#include "packer_commons.h"

void seq_pack(const char* source_filename, const char* dest_filename, packProfile profile);

void seqPack(const wchar_t* src, const wchar_t* dst, packProfile profile);

void seqUnpack(const wchar_t* src, const wchar_t* dst);

void seq_unpack(const char* source_filename, const char* dest_filename);

void seq_unpack_separate(const char* source_filename, const char* dest_filename, const char* dir);

void seq_pack_separate(const char* source_filename, const char* dir, packProfile profile);



#endif