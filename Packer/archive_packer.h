#ifndef ARCHIVE_PACKER_H
#define ARCHIVE_PACKER_H

#define TYPE_SOLID 0
#define TYPE_SEMISEPARATED 1
#define TYPE_SEPARATED 2

void archive_pack(wchar_t *dir, const wchar_t* dest, completePackProfile profile);

void archive_unpack(wchar_t* src,  wchar_t* dir);

#endif
