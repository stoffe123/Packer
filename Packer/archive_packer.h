#ifndef ARCHIVE_PACKER_H
#define ARCHIVE_PACKER_H



void archive_pack(wchar_t *dir, const wchar_t* dest, packProfile_t profile);

void archive_unpack(wchar_t* src,  wchar_t* dir);

#endif
