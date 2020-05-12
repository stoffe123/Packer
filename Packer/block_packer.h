#ifndef BLOCK_PACKER_H
#define BLOCK_PACKER_H

void block_pack(const char* src, const char* dst, unsigned char offset_pages,
	unsigned char seqlen_pages, int seq_ratio, int rle_ratio, int twobyte_ratio);

void block_unpack(const char* src, const char* dst);


#endif
