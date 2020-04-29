#ifndef MULTIPACKER_H
#define MULTIPACKER_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>


#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "common_tools.h"
//#include "huffman2.h"
#include "canonical.h"

void multi_pack(const char* src, const char* dst, unsigned char pages, bool skip_recursion);

void multi_unpack(const char* src, const char* dst);


#endif