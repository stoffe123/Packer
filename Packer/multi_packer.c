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
		const char seqlens_name[100] = { 0 };
		const char offsets_name[100] = { 0 };
		
		concat(seqlens_name, pi.dir, "seqlens");
		concat(offsets_name, pi.dir, "offsets");
		

		copy_chunk(in, seqlens_name, size1);
		copy_chunk(in, offsets_name, size2);
	}
	const char main_name[100] = { 0 };
	concat(main_name, pi.dir, "main");
	copy_the_rest(in, main_name);
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

		const char seqlens_name[100] = { 0 };
		const char offsets_name[100] = { 0 };
		concat(seqlens_name, base_dir, "seqlens");
		concat(offsets_name, base_dir, "offsets");

		uint32_t size_seqlens = get_file_size_from_name(seqlens_name);

		// 4 byte (32 bit) file size can handle meta files up to 4,19 GB
		fwrite(&size_seqlens, sizeof(size_seqlens), 1, out_file);

		uint32_t size_offsets = get_file_size_from_name(offsets_name);
		fwrite(&size_offsets, sizeof(size_offsets), 1, out_file);

		append_to_file(out_file, seqlens_name);
		append_to_file(out_file, offsets_name);
	}
	const char main_name[100] = { 0 };
	concat(main_name, base_dir, "main");
	append_to_file(out_file, main_name);

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
	printf("\n ratio with RLE  %f (limit %f)", packed_ratio, RLE_limit);

	bool compression_success = (packed_ratio < RLE_limit);
	if (!compression_success) {
		remove(dst);
		copy_file(src, dst);
	}
	return compression_success;
}


bool TwoBytePackAndTest(const char* src, packProfile_t profile) {

	char tmp[100] = { 0 };
	get_temp_file2(tmp, "multi_twobyted");
	two_byte_pack(src, tmp, profile);
	int size_org = get_file_size_from_name(src);
	int size_packed = get_file_size_from_name(tmp);
	double packed_ratio = (double)size_packed / (double)size_org;
	printf("\n Two byte packed %s  got ratio %.2f (limit %d)", src, packed_ratio, profile.twobyte_ratio);
	bool compression_success = (packed_ratio < ((double)profile.twobyte_ratio/100.0));  
	if (compression_success) {
		remove(src);
		rename(tmp, src);
	}
	else {
		remove(tmp);
	}
	return compression_success;
}

void SeqUnpackAndReplace(const char* src) {
	//printf("\n Seq unpacking (in place) %s", src);
	const char tmp[100] = { 0 };
	get_temp_file2(tmp, "multi_sequnpacked");
	seq_unpack(src, tmp);
	remove(src);
	rename(tmp, src);
}

void MultiUnpackAndReplace(const char* src) {
	//printf("\n Seq unpacking (in place) %s", src);
	const char tmp[100] = { 0 };
	get_temp_file2(tmp, "multi_sequnpacked");
	multi_unpack(src, tmp);
	remove(src);
	rename(tmp, src);
}

void TwoByteUnpackAndReplace(const char* src) {

	const char tmp[100] = { 0 };
	get_temp_file2(tmp, "multi_twobytedunp");
	two_byte_unpack(src, tmp);
	remove(src);
	rename(tmp, src);
}

//----------------------------------------------------------------------------------------

void multi_pack(const char* src, const char* dst, packProfile_t profile) {
 
	printf("\n* Multi pack *");
	printProfile(&profile);
	unsigned long long src_size = get_file_size_from_name(src);
	unsigned char pack_type = 0;
	bool do_store = src_size < 6;
	if (!do_store) {
		char temp_filename[100] = { 0 };
		get_temp_file2(temp_filename, "multi_rlepacked");

		bool got_smaller = RLE_pack_and_test(src, temp_filename, profile.rle_ratio);
		if (got_smaller) {
			pack_type = setKthBit(pack_type, 5);
		}

		char base_dir[100] = { 0 };
		get_clock_dir(base_dir);
		got_smaller = TwoBytePackAndTest(temp_filename, profile);
		//got_smaller = false;
		if (got_smaller) {
			pack_type = setKthBit(pack_type, 6);
		}
		uint64_t temp_filename_size = get_file_size_from_name(temp_filename);	
			seq_pack_separate(temp_filename, base_dir, profile.offset_pages, profile.seqlen_pages);
		
		const char seqlens_name[100] = { 0 };
		const char offsets_name[100] = { 0 };
		const char main_name[100] = { 0 };
		concat(seqlens_name, base_dir, "seqlens");
		concat(offsets_name, base_dir, "offsets");
		concat(main_name, base_dir, "main");

		//try to pack meta files!

		// ---------- Pack seqlens -----------
		if (temp_filename_size > 0 && get_file_size_from_name(seqlens_name) > profile.recursive_limit) {
			packProfile_t seqlenProfile;
			seqlenProfile.offset_pages = 76;
			seqlenProfile.seqlen_pages = 2;
			seqlenProfile.rle_ratio = 65;
			seqlenProfile.twobyte_ratio = 75;
			seqlenProfile.seq_ratio = 34;
			got_smaller = MultiPackAndTest(seqlens_name, seqlenProfile);
			if (got_smaller) {
				pack_type = setKthBit(pack_type, 1);
			}

			// ------------- Pack offsets --------------
			packProfile_t offsetProfile;
			offsetProfile.offset_pages = 105;
			offsetProfile.seqlen_pages = 57;
			offsetProfile.rle_ratio = 80;
			offsetProfile.twobyte_ratio = 79;
			offsetProfile.seq_ratio = 94;
			got_smaller = MultiPackAndTest(offsets_name, offsetProfile);
			if (got_smaller) {
				pack_type = setKthBit(pack_type, 2);
			}
		}

		uint64_t packed_size = get_file_size_from_name(offsets_name) +
			get_file_size_from_name(seqlens_name) +
			get_file_size_from_name(main_name);

		double seqPackRatio = ((double)packed_size) /
			((double)temp_filename_size);

		bool seqPacked = seqPackRatio < ((double)profile.seq_ratio / (double)100);
		/*
		if (get_file_size_from_name(temp_filename) < 512) {
			seqPacked = seqPackRatio < 1.0;
		}
		*/
		printf("\n Seqpacked %s and got ratio %.2f (limit %d)", temp_filename, seqPackRatio, profile.seq_ratio);
		if (seqPacked) {
			pack_type = setKthBit(pack_type, 7);
		}
		else {
			copy_file(temp_filename, main_name);
		}
		remove(temp_filename);
		got_smaller = CanonicalEncodeAndTest(main_name);
		if (got_smaller) {
			pack_type = setKthBit(pack_type, 0);
		}
		else {
			profile.twobyte_threshold = 0;
			got_smaller = TwoBytePackAndTest(main_name, profile);
			//got_smaller = false;
			if (got_smaller) {
				//this case will probably never happen
				pack_type = setKthBit(pack_type, 3);
			}

		}

		printf("\nTar writing destination file: %s basedir:%s\nPack_type = %d", dst, base_dir, pack_type);
		packed_size = get_file_size_from_name(main_name) +
			(seqPacked ? get_file_size_from_name(offsets_name) +
				get_file_size_from_name(seqlens_name) + 8 : 0);

		uint64_t source_size = get_file_size_from_name(src);
		do_store = packed_size >= source_size;
		if (!do_store) {
			tar(dst, base_dir, pack_type);			
		} 
		remove(seqlens_name);
		remove(offsets_name);
		remove(main_name);		
	}
	if (do_store) {
		printf("\n  CHOOOSING STORE in multi_packer !!! ");
		pack_type = 0;
		store(src, dst, pack_type);
	}
	printf("\n => result: %s  size:%d", dst, get_file_size_from_name(dst));
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
	char base_dir[100] = { 0 };
	get_clock_dir(base_dir);
	pi.dir = base_dir;
	untar(in, pi);
	unsigned char pack_type = pi.pack_type;

	const char seqlens_name[100] = { 0 };
	const char offsets_name[100] = { 0 };
	const char main_name[100] = { 0 };
	concat(seqlens_name, base_dir, "seqlens");
	concat(offsets_name, base_dir, "offsets");
	concat(main_name, base_dir, "main");

	if (isKthBitSet(pack_type, 0)) { //main was huffman coded
		CanonicalDecodeAndReplace(main_name);
	}
	else if (isKthBitSet(pack_type, 3)) { 
		TwoByteUnpackAndReplace(main_name);
	}
	bool seqPacked = isKthBitSet(pack_type, 7);
	if (seqPacked) {
		if (isKthBitSet(pack_type, 1)) {
			MultiUnpackAndReplace(seqlens_name);
		}
		if (isKthBitSet(pack_type, 2)) {			
			MultiUnpackAndReplace(offsets_name);
		}		
	}
	const char seq_dst[100] = { 0 };
	get_temp_file2(seq_dst, "multi_seqsepunp");
	if (seqPacked) {
		printf("\n SEQ unpack separate of %smain", base_dir);
		seq_unpack_separate(main_name, seq_dst, base_dir);
	}
	else {
		copy_file(main_name, seq_dst);
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

	remove(seqlens_name);
	remove(offsets_name);
	remove(main_name);
}