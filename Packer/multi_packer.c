#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "common_tools.h"
#include "multi_packer.h"
//#include "huffman2.h"
#include "canonical.h"




typedef struct pack_info_t {
	unsigned char pack_type;
	const char* dir;
} pack_info_t;


void unstore(FILE* in, const char* dst) {
	copy_the_rest(in, dst);
	fclose(in);
}


void untar(FILE* in, pack_info_t pi) {
		
	// 4 byte (32 bit) file size can handle meta files up to 4,19 GB
	uint64_t size1 = 0;
	fread(&size1, 4, 1, in);
	uint64_t size2 = 0;
	fread(&size2, 4, 1, in);
	//printf("\n untar size1=%d, size2=%d\n", size1, size2);
	
	copy_chunk(in, concat(pi.dir, "seqlens"), size1);
	copy_chunk(in, concat(pi.dir, "offsets"), size2);
	copy_the_rest(in, concat(pi.dir, "main"));

	fclose(in);	
}

void store(const char* src, const char* dst, unsigned char pack_type) {
	FILE* out_file = fopen(dst, "wb");
	fwrite(&pack_type, 1, 1, out_file);
	append_to_file(out_file, src);
	fclose(out_file);
}

void tar(const char* dst, const char* base_dir, unsigned char pack_type) {
	FILE* out_file = fopen(dst, "wb");

	fwrite(&pack_type, 1, 1, out_file);
	//assert(pack_type < 64, concat("pack_type < 16 in multipacker.tar dst=", dst));
	const char* seqlens_filename = concat(base_dir, "seqlens");
	uint64_t size_seqlens = get_file_size_from_name(seqlens_filename);
	

	// 4 byte (32 bit) file size can handle meta files up to 4,19 GB
	fwrite(&size_seqlens, 4, 1, out_file);

	const char* offsets_filename = concat(base_dir, "offsets");
	uint64_t size_offsets = get_file_size_from_name(offsets_filename);
	
	fwrite(&size_offsets, 4, 1, out_file);

	append_to_file(out_file, seqlens_filename);
	append_to_file(out_file, offsets_filename);
	append_to_file(out_file, concat(base_dir, "main"));

	fclose(out_file);
}

void my_rename(const char* f_old, const char* f_new) {
	remove(f_new);
	rename(f_old, f_new);
}

bool RLE_pack_and_test(const char* src, const char* dst) {

	RLE_simple_pack(src, dst);
	int size_org = get_file_size_from_name(src);
	int size_packed = get_file_size_from_name(dst);

	double ratio = (double)size_packed / (double)size_org;
	double RLE_limit = 0.96;
	printf("\n ratio with RLE  %f (limit %f)", ratio, RLE_limit);

	bool compression_success = (ratio < RLE_limit);
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
	//printf("\n CanonicalEncode:%s  (%f)", src, (double)size_packed / (double)size_org);
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

bool SeqPackAndTest(const char* dir, const char* src) {
	src = concat(dir, src);
	char* tmp = get_temp_file();
	seq_pack(src, tmp, 80, 10);
	int size_org = get_file_size_from_name(src);
	int size_packed = get_file_size_from_name(tmp);
	double ratio = (double)size_packed / (double)size_org;
	printf("\n SeqPacked:%s  got ratio: %f", src, ratio);
	bool compression_success = (ratio < 0.89);
	if (compression_success) {
		remove(src);
		rename(tmp, src);
	}
	else {
		remove(tmp);
	}
	return compression_success;
}

bool TwoBytePackAndTest(const char* dir, const char* src) {
	
	char* tmp = get_temp_file(dir);
	two_byte_pack(src, tmp, 100);
	int size_org = get_file_size_from_name(src);
	int size_packed = get_file_size_from_name(tmp);
	double ratio = (double)size_packed / (double)size_org;
	printf("\n Two byte packed %s  got ratio  %f", src, ratio);
	bool compression_success = (ratio < 0.80); // 0.89);
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
	else {
		remove(tmp);
	}
	return compression_success;
}

void CanonicalDecodeAndReplace(const char* dir, const char* src) {
	src = concat(dir, src);
	printf("\n Canonical unpacking (in place) %s", src);
	const char* tmp = get_temp_file(dir);
	CanonicalDecode(src, tmp);
	remove(src);
	rename(tmp, src);	
}

void SeqUnpackAndReplace(const char* dir, const char* src) {
	src = concat(dir, src);
	//printf("\n Seq unpacking (in place) %s", src);
	const char* tmp = get_temp_file(dir);
	seq_unpack(src, tmp);
	remove(src);
	rename(tmp, src);
}

void TwoByteUnpackAndReplace(const char* dir, const char* src) {

	const char* tmp = get_temp_file(dir);
	two_byte_unpack(src, tmp);
	remove(src);
	rename(tmp, src);
}

void MultiUnpackAndReplace(const char* dir, const char* src) {
	src = concat(dir, src);
	const char* tmp = get_temp_file(dir);
	multi_unpack(src, tmp, true);
	remove(src);
	rename(tmp, src);
}

void remove_meta_files(const char* base_dir) {
	remove(concat(base_dir, "seqlens"));
	remove(concat(base_dir, "offsets"));
	remove(concat(base_dir, "main"));
}

//----------------------------------------------------------------------------------------

void multi_pack(const char* src, const char* dst, unsigned char offset_pages,
	unsigned char seqlen_pages) {
	printf("\n------------------------------------------------------------------------------");
	printf("\nMulti_pack  %s  =>  %s\nsize org: %d    pages: (%d,%d)", src, dst, get_file_size_from_name(src), offset_pages, seqlen_pages);
	unsigned long long src_size = get_file_size_from_name(src);
	unsigned char pack_type = 0;
	char* tmp = get_temp_file();

	bool got_smaller = RLE_pack_and_test(src, tmp);
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 5);
	}
	const char* base_dir = get_clock_dir();
	got_smaller = TwoBytePackAndTest(base_dir, tmp);
    //got_smaller = false;
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 6);
	}

	seq_pack_separate(tmp, base_dir, offset_pages, seqlen_pages);
	// now we have three meta files to try to pack with seqlen+huffman

	remove(tmp);

	//try to pack meta files!
	// ----------- Pack main -------------
	got_smaller = CanonicalEncodeAndTest(base_dir, "main");
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 0);
	}

	// ---------- Pack seqlens -----------
	got_smaller = SeqPackAndTest(base_dir, "seqlens");
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 1);
	}

	got_smaller = CanonicalEncodeAndTest(base_dir, "seqlens");
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 2);
	}

	// ------------- Pack offsets --------------
	got_smaller = SeqPackAndTest(base_dir, "offsets");
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 3);
	}

	got_smaller = CanonicalEncodeAndTest(base_dir, "offsets");
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 4);
	}
	//-------------------------------------
	printf("\nTar writing destination file: %s basedir:%s\nPack_type = %d", dst, base_dir, pack_type);
	uint64_t packed_size = get_file_size_from_name(concat(base_dir, "offsets")) +
		get_file_size_from_name(concat(base_dir, "seqlens")) +
		get_file_size_from_name(concat(base_dir, "main"));
	uint64_t source_size = get_file_size_from_name(src);
	if (packed_size + 8 < source_size) {
		tar(dst, base_dir, pack_type);
	}
	else {
		pack_type = setKthBit(pack_type, 7);
		store(src, dst, pack_type);
	}
	printf("\n => result: %s  size:%d", dst, get_file_size_from_name(dst));

	remove_meta_files(base_dir);
}

// ----------------------------------------------------------------

void multi_unpack(const char* src, const char* dst) {

	printf("\n Multiunpack of %s  =>  %s", src, dst);
	pack_info_t pi;
	FILE* in = fopen(src, "rb");
	unsigned char pt;
	fread(&pt, 1, 1, in);
	pi.pack_type = pt;
	if (isKthBitSet(pt, 7)) {
		unstore(in, dst);
		return;
	}
	char* base_dir = get_clock_dir();
	pi.dir = base_dir;
	untar(in, pi);
	unsigned char pack_type = pi.pack_type;
	
	if (isKthBitSet(pack_type, 0)) { //main was huffman coded
		CanonicalDecodeAndReplace(base_dir, "main");
	}
	if (isKthBitSet(pack_type, 2)) {
		CanonicalDecodeAndReplace(base_dir, "seqlens");
	}
	if (isKthBitSet(pack_type, 1)) {
		SeqUnpackAndReplace(base_dir, "seqlens");
	}
	if (isKthBitSet(pack_type, 4)) {
		CanonicalDecodeAndReplace(base_dir, "offsets");
	}
	if (isKthBitSet(pack_type, 3)) {
		SeqUnpackAndReplace(base_dir, "offsets");
	}
	if (isKthBitSet(pack_type, 5)) { //RLE unpack
		char* seq_dst = get_temp_file(base_dir);
		printf("\n SEQ unpack separate of %smain", base_dir);
		seq_unpack_separate("main", seq_dst, base_dir);

		if (isKthBitSet(pack_type, 6)) {
			TwoByteUnpackAndReplace(base_dir, seq_dst);
		}

		RLE_simple_unpack(seq_dst, dst);
		remove(seq_dst);
	}
	else {
		
		printf("\n SEQ unpack separate of %smain", base_dir);
		seq_unpack_separate("main", dst, base_dir);
		if (isKthBitSet(pack_type, 6)) {
			TwoByteUnpackAndReplace(base_dir, dst);
		}
	}
	remove_meta_files(base_dir);
}