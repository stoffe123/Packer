#ifndef PACKER_H
#define PACKER_H

int rle_pack(const char* source_filename, const char* dest_filename);

void rle_unpack(const char* source_filename, const char* dest_filename);

#endif