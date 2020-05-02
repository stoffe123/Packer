#ifndef SEQ_PACKER_H   /* Include guard */
#define SEQ_PACKER_H


void seq_pack(const char* source_filename, const char* dest_filename, unsigned char offset_pages, unsigned char seqlen_pages);

void seq_pack_separate(const char* source_filename, const char* dir, unsigned char offset_pages, unsigned char seqlen_pages);


#endif