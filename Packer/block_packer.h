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


void block_pack(const wchar_t*, const wchar_t*, packProfile pp);

void block_unpack(const wchar_t*, const wchar_t*);

void blockUnpackAndReplace(wchar_t* src);

HANDLE lockTempfileMutex();

HANDLE releaseTempfileMutex();

HANDLE lockClockdirMutex();

HANDLE releaseClockdirMutex();

#endif
