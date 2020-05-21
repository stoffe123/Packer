#ifndef ARCHIVE_PACKER_H
#define ARCHIVE_PACKER_H



void archive_pack(wchar_t *dir, const char* dest, packProfile_t profile);

void archive_unpack(char* src,  wchar_t* dir);

#endif
