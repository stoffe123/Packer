#ifndef CANONICAL_H
#define CANONICAL_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "huflocal.h"
#include "huffman.h"
#include "bitarray.h"
#include "bitfile.h"

int CanonicalEncodeFile(FILE* inFile, FILE* outFile);

int CanonicalDecodeFile(FILE* inFile, FILE* outFile);

int CanonicalEncode(const char* inFile, const char* outFile);

int CanonicalDecode(const char* inFile, const char* outFile);

#endif