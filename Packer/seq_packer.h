#ifndef SEQ_PACKER_H   /* Include guard */
#define SEQ_PACKER_H


void seq_pack(const char* source_filename, const char* dest_filename, unsigned char pages);

void seq_pack_separate(const char* source_filename, unsigned char pages, const char* dir);


#endif