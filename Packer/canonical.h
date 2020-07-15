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

int CanonicalEncodeFile(memfile* inFile, memfile* outFile);

int CanonicalDecodeFile(memfile* inFile, memfile* outFile);

memfile* canonicalDecode(memfile* m);

memfile* canonicalEncode(memfile* m);

#endif