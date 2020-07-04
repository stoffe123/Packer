#ifndef ARCHIVE_PACKER_H
#define ARCHIVE_PACKER_H

fileListAndCount_t storeDirectoryFilenames(const wchar_t* sDir);

void archive_pack(wchar_t *dir, const wchar_t* dest, packProfile profile);

void archive_unpack(wchar_t* src,  wchar_t* dir);

#endif
