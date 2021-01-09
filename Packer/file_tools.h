#ifndef FILE_TOOLS_H 
#define FILE_TOOLS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "packer_commons.h"

fileListAndCount_t storeDirectoryFilenames(const wchar_t* dir, bool storeSizes);

bool filesEqual(wchar_t* name1, wchar_t* name2);

#endif
