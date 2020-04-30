#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "common_tools.h"
#include "multi_packer.h"
#include "RLE_packer_advanced.h"
//#include "huffman2.h"
#include "canonical.h"



struct Pack_info {
	unsigned char pack_type;
	const char* dir;
};

char* get_rand() {
	const char* clk = malloc(50);
	_itoa(clock(), clk, 10);
	int r = (rand() % 100) + 100;
	const char* ra = malloc(50);
	_itoa(r, ra, 10);
	return concat(clk, ra);
}

char* get_temp_file(char* dir) {	
	return concat(dir, concat("tmp", get_rand()));
}

char* get_clock_dir() {
	
	const char* dir = concat("multipack", get_rand());
	dir = concat(dir, "/");
	dir = concat("c:/test/", dir);
	make_dir(dir);
	return dir;
}


struct Pack_info untar(const char* src) {
	struct Pack_info pi;
	FILE* in = fopen(src, "rb");

	unsigned char pt;
	fread(&pt, 1, 1, in);
	pi.pack_type = pt;
	//printf("untar packtype=%d", pack_type);
	uint64_t size1 = 0;
	fread(&size1, 8, 1, in);
	uint64_t size2 = 0;
	fread(&size2, 8, 1, in);
	//printf("\n untar size1=%d, size2=%d\n", size1, size2);
	char* base_dir = get_clock_dir();
	pi.dir = base_dir;
	copy_chunk(in, concat(base_dir, "seqlens"), size1);
	copy_chunk(in, concat(base_dir, "offsets"), size2);
	copy_the_rest(in, concat(base_dir, "main"));

	fclose(in);
	return pi;
}

void tar(const char* dst, const char* base_dir, unsigned char pack_type) {
	FILE* out_file = fopen(dst, "wb");

	fwrite(&pack_type, 1, 1, out_file);
	assert(pack_type < 16, concat("pack_type < 16 in multipacker.tar dst=", dst));
	const char* seqlens_filename = concat(base_dir, "seqlens");	
	uint64_t size_seqlens = get_file_size_from_name(seqlens_filename);	
	assert(size_seqlens > 0, concat("size_seqlens > 0 in multipacker.tar dst=", dst));
	fwrite(&size_seqlens, 8, 1, out_file);

	const char* offsets_filename = concat(base_dir, "offsets");	
	uint64_t size_offsets = get_file_size_from_name(offsets_filename);	
	assert(size_offsets > 0, concat("size_offsets > 0 in multipacker.tar dst=", dst));
	fwrite(&size_offsets, 8, 1, out_file);

	append_to_file(out_file, seqlens_filename);
	append_to_file(out_file, offsets_filename);
	append_to_file(out_file, concat(base_dir, "main"));

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

bool CanonicalEncodeAndTest(const char* dir, const char* src) {
	src = concat(dir, src);
	char* tmp = get_temp_file(dir);
	CanonicalEncode(src, tmp);
	int size_org = get_file_size_from_name(src);
	int size_packed = get_file_size_from_name(tmp);
	bool compression_success = (size_packed < size_org);
	if (compression_success) {
		remove(src);
		rename(tmp, src);
	}
	else {
		remove(tmp);
	}
	return compression_success;
}

bool MultiPackAndTest(const char* dir, const char* src) {
	src = concat(dir, src);
	char* tmp = get_temp_file(dir);
	multi_pack(src, tmp, 2, true);
	int size_org = get_file_size_from_name(src);
	int size_packed = get_file_size_from_name(tmp);
	bool compression_success = (size_packed < size_org);
	if (compression_success) {
		remove(src);
		rename(tmp, src);
	}
	remove(tmp);
	return compression_success;
}

void CanonicalDecodeAndReplace(const char* dir, const char* src) {
	src = concat(dir, src);
	const char* tmp = get_temp_file(dir);
	CanonicalDecode(src, tmp);
	remove(src);
	rename(tmp, src);
	remove(tmp);
}

void MultiUnpackAndReplace(const char* dir, const char* src) {
	src = concat(dir, src);
	const char* tmp = get_temp_file(dir);
	multi_unpack(src, tmp, true);
	remove(src);
	rename(tmp, src);
	remove(tmp);
}

//----------------------------------------------------------------------------------------

void multi_pack(const char* src, const char* dst, unsigned char pages, bool skip_recursion) {
	printf("\n-----------------------");
	printf("\nmulti_pack of %s  =>  %s   size:%d   pages:%d", src, dst, get_file_size_from_name(src),  pages);
	unsigned long long src_size = get_file_size_from_name(src);
	unsigned char pack_type = 0;
	char* base_dir = get_clock_dir();
	char* tmp = get_temp_file(base_dir);

	bool got_smaller = RLE_advanced_pack_and_test(src, tmp);
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 3);
	}

	seq_pack_separate(tmp, pages, base_dir);
	// now we have three files to huffman pack

	remove(tmp);

	//try to pack meta files!
    // ----------- Pack main -------------
	got_smaller = CanonicalEncodeAndTest(base_dir, "main");
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 0);
	}
	// ---------- Pack seqlens -----------
	if (skip_recursion) {
		got_smaller = CanonicalEncodeAndTest(base_dir, "seqlens");
	}
	else {
		got_smaller = MultiPackAndTest(base_dir, "seqlens");		
	}
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 1);
	}
	// ---------- Pack offsets -----------
	if (skip_recursion) {
	   got_smaller = CanonicalEncodeAndTest(base_dir, "offsets");
	}
	else {
		got_smaller = MultiPackAndTest(base_dir, "offsets");		
	}
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 2);
	}
	//-------------------------------------
	printf("\nTar writing destination file: %s basedir:%s\nPack_type = %d", dst, base_dir,pack_type);
	tar(dst, base_dir, pack_type);
	printf("\n => result: %s  size:%d",dst, get_file_size_from_name(dst));
	printf("\n-------------------------------------");
	//cleanup
	
}

// ----------------------------------------------------------------

void multi_unpack(const char* src, const char* dst, bool skip_recursion) {

	printf("\n-----------------------------------------------------");
	printf("\n multiunpack of %s  =>  %s    skip_rec:%d", src, dst, skip_recursion);
	struct Pack_info pi = untar(src);
	unsigned char pack_type = pi.pack_type;
	char* base_dir = pi.dir;
	if (isKthBitSet(pack_type, 0)) {
		CanonicalDecodeAndReplace(base_dir, "main");
	}
	if (isKthBitSet(pack_type, 1)) {
		if (skip_recursion) {
			CanonicalDecodeAndReplace(base_dir, "seqlens");
		}
		else {
			MultiUnpackAndReplace(base_dir, "seqlens");
		}
	}
	if (isKthBitSet(pack_type, 2)) {
		if (skip_recursion) {
			CanonicalDecodeAndReplace(base_dir, "offsets");
		}
		else {
			MultiUnpackAndReplace(base_dir, "offsets");
		}
	}
	if (isKthBitSet(pack_type, 3)) {
		char* seq_dst = get_temp_file(base_dir);
		seq_unpack_separate("main", seq_dst, base_dir);
		RLE_advanced_unpack(seq_dst, dst);
		remove(seq_dst);
	}
	else {
		seq_unpack_separate("main", dst, base_dir);
	}
	
}