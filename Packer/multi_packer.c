#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>


#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "common_tools.h"
//#include "huffman2.h"
#include "canonical.h"

static const unsigned long BUF_SIZE = 32768;

static unsigned char buf[32768];

void copy(FILE* main, const char* s, unsigned long long size) {
	FILE* out = fopen(s, "wb");
	unsigned long long total_read = 0, bytes_to_read, bytes_got;
	do {
		if (size > 0 && total_read + BUF_SIZE > size) {
			bytes_to_read = (size - total_read);
		}
		else {
			bytes_to_read = BUF_SIZE;
		}
		bytes_got = fread(&buf, 1, bytes_to_read, main);
		fwrite(&buf, 1, bytes_got, out);
		total_read += bytes_got;
	} while (bytes_got == BUF_SIZE);

	fclose(out);
}


unsigned char untar(const char* src) {
	FILE* in = fopen(src, "rb");

	unsigned char pack_type;
	fread(&pack_type, 1, 1, in);
	//printf("untar packtype=%d", pack_type);
	uint32_t size1 = 0;
	fread(&size1, 4, 1, in);
	uint32_t size2 = 0;
	fread(&size2, 4, 1, in);
	//printf("\n untar size1=%d, size2=%d\n", size1, size2);
	copy(in, pack_type ? "c:/test/huffman_seqlens" : "c:/test/seqlens", size1);
	copy(in, pack_type ? "c:/test/huffman_offsets" : "c:/test/offsets", size2);
	copy(in, pack_type ? "c:/test/huffman_main" : "c:/test/main", 0); //the rest
	
	fclose(in);
	return pack_type;
}

void append_to(FILE* main_file, const char* append_filename) {
	FILE* append_file = fopen(append_filename, "rb");
	unsigned long long bytes_got;
	do {
		bytes_got = fread(&buf, 1, BUF_SIZE, append_file);
		fwrite(&buf, 1, bytes_got, main_file);
	} while (bytes_got == BUF_SIZE);
	fclose(append_file);
}

void tar(const char* dst, unsigned char pack_type) {
	FILE* out_file = fopen(dst, "wb");

	fwrite(&pack_type, 1, 1, out_file);
	//printf("\ntar packtype=%d", pack_type);
	FILE* seqlens = fopen(pack_type ? "c:/test/huffman_seqlens" : "c:/test/seqlens", "rb");
	uint32_t size1 = get_file_size(seqlens);
	fclose(seqlens);
	fwrite(&size1, 4, 1, out_file);

	FILE* offsets = fopen(pack_type ? "c:/test/huffman_offsets" : "c:/test/offsets", "rb");
	uint32_t size2 = get_file_size(offsets);
	fclose(offsets);
	fwrite(&size2, 4, 1, out_file);

	append_to(out_file, pack_type ? "c:/test/huffman_seqlens" : "c:/test/seqlens");
	append_to(out_file, pack_type ? "c:/test/huffman_offsets" : "c:/test/offsets");
	append_to(out_file, pack_type ? "c:/test/huffman_main" : "c:/test/main");

	fclose(out_file);
}

//----------------------------------------------------------------------------------------

void multi_pack(const char* src, const char* dst, unsigned char pages) {

	unsigned long long src_size = get_file_size_from_name(src);
	char huffman = (src_size > 10000);
	seq_pack(src, "c:/test/main", pages);
	// now we have three files to huffman pack

	//printf("\nHuffman packing 3 files...");

	if (huffman) {
		CanonicalEncode("c:/test/main", "c:/test/huffman_main");
		CanonicalEncode("c:/test/seqlens", "c:/test/huffman_seqlens");
		CanonicalEncode("c:/test/offsets", "c:/test/huffman_offsets");
	}

	tar(dst, huffman);
}

void multi_unpack(const char* src, const char* dst) {
	
	unsigned char pack_type = untar(src);

	if (pack_type) {
		CanonicalDecode("c:/test/huffman_main", "c:/test/main");
		CanonicalDecode("c:/test/huffman_seqlens", "c:/test/seqlens");
		CanonicalDecode("c:/test/huffman_offsets", "c:/test/offsets");
	}
	seq_unpack("c:/test/main", dst);
}