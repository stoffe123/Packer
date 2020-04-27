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
	copy(in, "c:/test/seqlens", size1);
	copy(in, "c:/test/offsets", size2);
	copy(in, "c:/test/main", 0); //the rest

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
	FILE* seqlens = fopen("c:/test/seqlens", "rb");
	uint32_t size1 = get_file_size(seqlens);
	fclose(seqlens);
	fwrite(&size1, 4, 1, out_file);

	FILE* offsets = fopen("c:/test/offsets", "rb");
	uint32_t size2 = get_file_size(offsets);
	fclose(offsets);
	fwrite(&size2, 4, 1, out_file);

	append_to(out_file, "c:/test/seqlens");
	append_to(out_file, "c:/test/offsets");
	append_to(out_file, "c:/test/main");

	fclose(out_file);
}

void my_rename(const char* f_old, const char* f_new) {
	remove(f_new);
	rename(f_old, f_new);
}


unsigned char isKthBitSet(unsigned char value, unsigned char bit)
{
	return (value & (1 << bit));
}

unsigned char setKthBit(unsigned char value, unsigned char bit)
{
	// kth bit of n is being set by this operation 
	return ((1 << bit) | value);
}

int CanonicalEncodeAndTest(const char* src) {
	CanonicalEncode(src, "tmp383754");
	int size_org = get_file_size_from_name(src);
	int size_packed = get_file_size_from_name("tmp383754");
	int res = (size_packed < size_org);
	if (res) {
		remove(src);
		rename("tmp383754", src);
	}
	else {
		remove("tmp383754");
	}
	return res;
}

void CanonicalDecodeAndReplace(const char* src) {
	CanonicalDecode(src, "tmp675839");
	remove(src);
	rename("tmp675839", src);
}



//----------------------------------------------------------------------------------------

void multi_pack(const char* src, const char* dst, unsigned char pages) {

	unsigned long long src_size = get_file_size_from_name(src);
	unsigned char pack_type = 0;
	seq_pack(src, "c:/test/main", pages);
	// now we have three files to huffman pack

	//try to huffman pack and check sizes!

	unsigned long long size_org, size_packed;

	int worked = CanonicalEncodeAndTest("c:/test/main");
	if (worked) {
		pack_type = setKthBit(pack_type, 0);
	}
	worked = CanonicalEncodeAndTest("c:/test/seqlens");
	if (worked) {
		pack_type = setKthBit(pack_type, 1);
	}
	worked = CanonicalEncodeAndTest("c:/test/offsets");
	if (worked) {
		pack_type = setKthBit(pack_type, 2);
	}
	printf("\npack_type = %d", pack_type);
	tar(dst, pack_type);
}


void multi_unpack(const char* src, const char* dst) {

	unsigned char pack_type = untar(src);

	if (isKthBitSet(pack_type, 0)) {
		CanonicalDecodeAndReplace("c:/test/main");
	}
	if (isKthBitSet(pack_type, 1)) {
		CanonicalDecodeAndReplace("c:/test/seqlens");
	}
	if (isKthBitSet(pack_type, 2)) {
		CanonicalDecodeAndReplace("c:/test/offsets");
	}

	seq_unpack("c:/test/main", dst);

}