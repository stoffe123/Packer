#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>


#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "common_tools.h"
#include "RLE_packer_advanced.h"
//#include "huffman2.h"
#include "canonical.h"

static const char* tmpFileName = "c:/test/tmp584583959";

static const char* tmpFileName2 = "c:/test/tmp12389589";

unsigned char untar(const char* src) {
	FILE* in = fopen(src, "rb");

	unsigned char pack_type;
	fread(&pack_type, 1, 1, in);
	//printf("untar packtype=%d", pack_type);
	uint64_t size1 = 0;
	fread(&size1, 8, 1, in);
	uint64_t size2 = 0;
	fread(&size2, 8, 1, in);
	//printf("\n untar size1=%d, size2=%d\n", size1, size2);
	copy_chunk(in, "c:/test/seqlens", size1);
	copy_chunk(in, "c:/test/offsets", size2);
	copy_the_rest(in, "c:/test/main");

	fclose(in);
	return pack_type;
}

void tar(const char* dst, unsigned char pack_type) {
	FILE* out_file = fopen(dst, "wb");

	fwrite(&pack_type, 1, 1, out_file);
	//printf("\ntar packtype=%d", pack_type);
	FILE* seqlens = fopen("c:/test/seqlens", "rb");
	uint64_t size1 = get_file_size(seqlens);
	fclose(seqlens);
	fwrite(&size1, 8, 1, out_file);

	FILE* offsets = fopen("c:/test/offsets", "rb");
	uint64_t size2 = get_file_size(offsets);
	fclose(offsets);
	fwrite(&size2, 8, 1, out_file);

	append_to_file(out_file, "c:/test/seqlens");
	append_to_file(out_file, "c:/test/offsets");
	append_to_file(out_file, "c:/test/main");

	fclose(out_file);
}

void my_rename(const char* f_old, const char* f_new) {
	remove(f_new);
	rename(f_old, f_new);
}

bool RLE_advanced_pack_and_test(const char* src, const char* dst) {
	RLE_advanced_pack(src, dst);
	int size_org = get_file_size_from_name(src);
	int size_packed = get_file_size_from_name(dst);
	double ratio = (double)size_packed / (double)size_org;
	printf("\n ratio with RLE_advanced:%f", ratio);
	bool compression_success = (ratio < 0.93);
	if (!compression_success) {
		remove(dst);
		copy_file(src, dst);
	}
	return compression_success;
}

bool CanonicalEncodeAndTest(const char* src) {
	CanonicalEncode(src, tmpFileName);
	int size_org = get_file_size_from_name(src);
	int size_packed = get_file_size_from_name(tmpFileName);
	bool compression_success = (size_packed < size_org);
	if (compression_success) {
		remove(src);
		rename(tmpFileName, src);
	}
	else {
		remove(tmpFileName);
	}
	return compression_success;
}

void CanonicalDecodeAndReplace(const char* src) {
	CanonicalDecode(src, tmpFileName);
	remove(src);
	rename(tmpFileName, src);
}

//----------------------------------------------------------------------------------------

void multi_pack(const char* src, const char* dst, unsigned char pages) {

	unsigned long long src_size = get_file_size_from_name(src);
	unsigned char pack_type = 0;
	
	bool worked = RLE_advanced_pack_and_test(src, tmpFileName2);
	if (worked) {
		pack_type = setKthBit(pack_type, 3);
	}

	seq_pack(tmpFileName2, "c:/test/main", pages);
	// now we have three files to huffman pack

	remove(tmpFileName2);

	//try to huffman pack and check sizes!

	unsigned long long size_org, size_packed;

	worked = CanonicalEncodeAndTest("c:/test/main");
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
	if (isKthBitSet(pack_type, 3)) {
		seq_unpack("c:/test/main", tmpFileName2);
		RLE_advanced_unpack(tmpFileName2, dst);
		remove(tmpFileName2);
	}
	else {
		seq_unpack("c:/test/main", dst);
	}

}