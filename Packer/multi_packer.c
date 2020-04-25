#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>


#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "common_tools.h"
//#include "huffman2.h"
#include "canonical.h"

void copy(FILE* main, const char* s, unsigned long long size) {
	FILE* out = fopen(s, "wb");
	char c;
	if (size == 0) {
		while (fread(&c, 1, 1, main) == 1) {
			fwrite(&c, 1, 1, out);
		}		
	}
	else {
		for (int i = 0; i < size; i++) {
			fread(&c, 1, 1, main);
			fwrite(&c, 1, 1, out);
		}
	}
	fclose(out);
}


void untar(const char* src) {
	FILE* in = fopen(src, "rb");
	uint32_t size1 = 0;
	fread(&size1, 4, 1, in);
	uint32_t size2 = 0;
	fread(&size2, 4, 1, in);
	printf("\n untar size1=%d, size2=%d\n", size1, size2);
	copy(in, "c:/test/huffman_seqlens", size1);
	copy(in, "c:/test/huffman_offsets", size2);
	copy(in, "c:/test/huffman_main", 0); //the rest
	fclose(in);
}

void append_to(FILE* main_file, const char* append_filename) {
	FILE* append_file = fopen(append_filename, "rb");
	unsigned char c;
	while (fread(&c, 1, 1, append_file) == 1) {
		fwrite(&c, 1, 1, main_file);
	}
	fclose(append_file);
}

void tar(const char* dst) {
	FILE* out_file = fopen(dst, "wb");

	FILE* seqlens = fopen("c:/test/huffman_seqlens", "rb");
	uint32_t size1 = get_file_size(seqlens);
	fclose(seqlens);
	fwrite(&size1, 4, 1, out_file);	

	FILE* offsets = fopen("c:/test/huffman_offsets", "rb");
	uint32_t size2 = get_file_size(offsets);
	fclose(offsets);
	fwrite(&size2, 4, 1, out_file);

	append_to(out_file, "c:/test/huffman_seqlens");
	append_to(out_file, "c:/test/huffman_offsets");
	append_to(out_file, "c:/test/huffman_main");

	fclose(out_file);
}

//----------------------------------------------------------------------------------------

void multi_pack(const char* src, const char* dst) {

	const char* packed_name = "c:/test/multipack_temp.bin";
	
	seq_pack(src, packed_name);
	// now we have three files to huffman pack

	printf("\nHuffman packing 3 files...");

	CanonicalEncode(packed_name, "c:/test/huffman_main");
	CanonicalEncode("c:/test/seqlens", "c:/test/huffman_seqlens");
	CanonicalEncode("c:/test/offsets", "c:/test/huffman_offsets");

	tar(dst);
}

void multi_unpack(const char* src, const char* dst) {
	const char* packed_name = "c:/test/multipack_unpack_temp.bin";
    untar(src);

	CanonicalDecode("c:/test/huffman_main", packed_name);
	CanonicalDecode("c:/test/huffman_seqlens", "c:/test/seqlens");
	CanonicalDecode("c:/test/huffman_offsets", "c:/test/offsets");

	seq_unpack(packed_name, dst);

	// finished!!

	return 0;
}