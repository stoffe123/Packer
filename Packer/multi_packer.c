#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "common_tools.h"
#include "multi_packer.h"
#include "packer_commons.h"
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
	uint32_t size1 = 0;
	if (isKthBitSet(pi.pack_type, 7)) {
		fread(&size1, sizeof(size1), 1, in);
		uint32_t size2 = 0;
		fread(&size2, sizeof(size2), 1, in);
		//printf("\n untar size1=%d, size2=%d\n", size1, size2);

		copy_chunk(in, concat(pi.dir, "seqlens"), size1);
		copy_chunk(in, concat(pi.dir, "offsets"), size2);
	}
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

	if (isKthBitSet(pack_type, 7)) {

		const char* seqlens_filename = concat(base_dir, "seqlens");
		uint32_t size_seqlens = get_file_size_from_name(seqlens_filename);

		// 4 byte (32 bit) file size can handle meta files up to 4,19 GB
		fwrite(&size_seqlens, sizeof(size_seqlens), 1, out_file);

		const char* offsets_filename = concat(base_dir, "offsets");
		uint32_t size_offsets = get_file_size_from_name(offsets_filename);

		fwrite(&size_offsets, sizeof(size_offsets), 1, out_file);

		append_to_file(out_file, seqlens_filename);
		append_to_file(out_file, offsets_filename);
	}
	append_to_file(out_file, concat(base_dir, "main"));

	fclose(out_file);
}

void my_rename(const char* f_old, const char* f_new) {
	remove(f_new);
	rename(f_old, f_new);
}

bool RLE_pack_and_test(const char* src, const char* dst, int ratio) {

	RLE_simple_pack(src, dst);
	int size_org = get_file_size_from_name(src);
	int size_packed = get_file_size_from_name(dst);

	double packed_ratio = (double)size_packed / (double)size_org;
	double RLE_limit = (double)ratio / 100.0;
	printf("\n ratio with RLE  %f (limit %f)", ratio, RLE_limit);

	bool compression_success = (packed_ratio < RLE_limit);
	if (!compression_success) {
		remove(dst);
		copy_file(src, dst);
	}
	return compression_success;
}




bool TwoBytePackAndTest(const char* dir, const char* src, int ratio) {

	char* tmp = get_temp_file2("multi_twobyted");
	two_byte_pack(src, tmp, 100);
	int size_org = get_file_size_from_name(src);
	int size_packed = get_file_size_from_name(tmp);
	double packed_ratio = (double)size_packed / (double)size_org;
	printf("\n Two byte packed %s  got ratio  %f", src, ratio);
	bool compression_success = (packed_ratio < ((double)ratio/100.0));  
	if (compression_success) {
		remove(src);
		rename(tmp, src);
	}
	else {
		remove(tmp);
	}
	return compression_success;
}


void SeqUnpackAndReplace(const char* dir, const char* src) {
	src = concat(dir, src);
	//printf("\n Seq unpacking (in place) %s", src);
	const char* tmp = get_temp_file2("multi_sequnpacked");
	seq_unpack(src, tmp);
	remove(src);
	rename(tmp, src);
}

void TwoByteUnpackAndReplace(const char* src) {

	const char* tmp = get_temp_file2("multi_twobytedunp");
	two_byte_unpack(src, tmp);
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
	unsigned char seqlen_pages, int rle_ratio, int twobyte_ratio, int seq_ratio) {

	printf("\nMulti_pack  %s  =>  %s\nsize org: %d    pages: (%d,%d) seqpackratio %d", src, dst, get_file_size_from_name(src), offset_pages, seqlen_pages, seq_ratio);
	unsigned long long src_size = get_file_size_from_name(src);
	unsigned char pack_type = 0;
	char* temp_filename = get_temp_file2("multi_rlepacked");

	bool got_smaller = RLE_pack_and_test(src, temp_filename, rle_ratio);
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 5);
	}
	const char* base_dir = get_clock_dir();
	got_smaller = TwoBytePackAndTest(base_dir, temp_filename, twobyte_ratio);
	//got_smaller = false;
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 6);
	}

	seq_pack_separate(temp_filename, base_dir, offset_pages, seqlen_pages);
	const char* main_filename = concat(base_dir, "main");
	const char* seqlens_filename = concat(base_dir, "seqlens");
	const char* offsets_filename = concat(base_dir, "offsets");

	//try to pack meta files!

	// ---------- Pack seqlens -----------
	got_smaller = SeqPackAndTest(seqlens_filename, 65, 3, 33);
	//printf(concat(base_dir, "seqlens"));	

	if (got_smaller) {
		pack_type = setKthBit(pack_type, 1);
	}

	got_smaller = CanonicalEncodeAndTest(seqlens_filename);
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 2);
	}

	// ------------- Pack offsets --------------
	got_smaller = SeqPackAndTest(offsets_filename, 115, 68, 97);
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 3);
	}

	got_smaller = CanonicalEncodeAndTest(offsets_filename);
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 4);
	}

	uint64_t packed_size = get_file_size_from_name(offsets_filename) +
		get_file_size_from_name(seqlens_filename) +
		get_file_size_from_name(main_filename);

	double seqPackRatio = (double)packed_size /
		(double)get_file_size_from_name(temp_filename);

	bool seqPacked = seqPackRatio < ((double)seq_ratio / (double)100);
	printf("\n Seqpacked %s and got ratio %f.2 (limit %d)", temp_filename, seqPackRatio, seq_ratio);
	if (seqPacked) {
		pack_type = setKthBit(pack_type, 7);
	}
	else {
		copy_file(temp_filename, main_filename);
	}
	remove(temp_filename);
	got_smaller = CanonicalEncodeAndTest(main_filename);
	if (got_smaller) {
		pack_type = setKthBit(pack_type, 0);
	}

	printf("\nTar writing destination file: %s basedir:%s\nPack_type = %d", dst, base_dir, pack_type);
	packed_size = get_file_size_from_name(main_filename) +
		(seqPacked ? get_file_size_from_name(offsets_filename) +
			get_file_size_from_name(seqlens_filename) + 8 : 0);

	uint64_t source_size = get_file_size_from_name(src);
	if (packed_size < source_size) {
		tar(dst, base_dir, pack_type);
	}
	else {
		printf("\n  CHOOOSING STORE in multi_packer !!! ");
		pack_type = 0;
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
	if (pt == 0) {
		printf("\n  UNSTORE!!  ");
		unstore(in, dst);
		return;
	}
	char* base_dir = get_clock_dir();
	pi.dir = base_dir;
	untar(in, pi);
	unsigned char pack_type = pi.pack_type;

	if (isKthBitSet(pack_type, 0)) { //main was huffman coded
		CanonicalDecodeAndReplace(concat(base_dir, "main"));
	}
	bool seqPacked = isKthBitSet(pack_type, 7);
	if (seqPacked) {
		if (isKthBitSet(pack_type, 2)) {
			CanonicalDecodeAndReplace(concat(base_dir, "seqlens"));
		}
		if (isKthBitSet(pack_type, 1)) {
			SeqUnpackAndReplace(base_dir, "seqlens");
		}
		if (isKthBitSet(pack_type, 4)) {
			CanonicalDecodeAndReplace(concat(base_dir, "offsets"));
		}
		if (isKthBitSet(pack_type, 3)) {
			SeqUnpackAndReplace(base_dir, "offsets");
		}
	}

	const char* seq_dst = get_temp_file2("multi_seqsepunp");
	if (seqPacked) {
		printf("\n SEQ unpack separate of %smain", base_dir);
		seq_unpack_separate("main", seq_dst, base_dir);
	}
	else {
		copy_file(concat(base_dir, "main"), seq_dst);
	}
	if (isKthBitSet(pack_type, 6)) {
		TwoByteUnpackAndReplace(seq_dst);
	}
	if (isKthBitSet(pack_type, 5)) {
		RLE_simple_unpack(seq_dst, dst);
	}
	else {
		copy_file(seq_dst, dst);
	}
	remove(seq_dst);

	remove_meta_files(base_dir);
}