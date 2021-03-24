#ifndef BLOCK_PACKER_H
#define BLOCK_PACKER_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <windows.h>
#include <string.h>
#include <conio.h>
#include <process.h>
#include "packer_commons.h"

// PACK

void block_pack(const wchar_t*, const wchar_t*, packProfile pp);

void blockPackFull(const wchar_t*, const wchar_t*, completePackProfile pp);

void blockPackAndReplace(const wchar_t* src, packProfile profile);

void blockPackToExistingFile(const wchar_t* src, FILE* utfil, packProfile profile);

void block_pack_file(FILE* infil, const wchar_t* dst, packProfile profile);

void blockPackAndReplaceFull(const wchar_t* src, completePackProfile prof);



// UNPACK

void block_unpack_file(FILE* infil, const wchar_t* dst, completePackProfile profile);

void blockUnpackFileToFile(FILE* infil, FILE* utfil, completePackProfile profile);

void blockUnpackNameToFile(const wchar_t* src, FILE* utfil, completePackProfile profile);

void block_unpack(const wchar_t*, const wchar_t*, completePackProfile profile);

void blockUnpackAndReplace(wchar_t* src, completePackProfile profile);



#endif
