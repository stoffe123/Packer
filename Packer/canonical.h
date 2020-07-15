#ifndef CANONICAL_H
#define CANONICAL_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "huflocal.h"
#include "huffman.h"
#include "bitarray.h"
#include "bitfile.h"
#include "memfile.h"

int CanonicalEncodeFile(FILE* inFile, FILE* outFile);

int CanonicalDecodeFile(FILE* inFile, FILE* outFile);

int CanonicalEncode(const wchar_t* src, const wchar_t* dst);

int CanonicalDecode(const wchar_t* src, const wchar_t* dst);

memfile* CanonicalEncodeMem(memfile* m);

memfile* CanonicalDecodeMem(memfile* m);


#endif