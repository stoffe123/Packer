#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "common_tools.h"
#include "multi_packer.h"
#include "packer_commons.h"
#include "Two_byte_packer.h"
#include "Two_byte_unpacker.h"
#include "RLE_packer_advanced.h"
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
	printf("\n ratio with RLE simple  %f (limit %f)", packed_ratio, RLE_limit);

	bool compression_success = (packed_ratio < RLE_limit);
	if (!compression_success) {
		remove(dst);
		copy_file(src, dst);
	}
	return compression_success;
}


bool TwoBytePackAndTest(const char* src, packProfile_t profile) {

	const char* tmp[100] = { 0 };
	get_temp_file2(tmp, "multi_twobyted");
	two_byte_pack(src, tmp, profile);		
	bool compression_success = testPack(src, tmp, "Two byte", profile.twobyte_ratio);
	if (compression_success) {

		if (DOUBLE_CHECK_PACK) {
			//test if compression worked!
			const char tmp2[100] = { 0 };
			get_temp_file2(tmp2, "multi_maksure2bytre");
			two_byte_unpack(tmp, tmp2);
			bool sc = files_equal(tmp2, src);
			if (!sc) {
				printf("\n\n\n ** Failed to two byte pack: %s", src);
				exit(1);
			}
			remove(tmp2);
		}		
		my_rename(tmp, src);
	}
	else {
		remove(tmp);
	}
	return compression_success;
}

bool RLEAdvancedPackAndTest(const char* src, packProfile_t profile) {
	char tmp[100] = { 0 };
	get_temp_file2(tmp, "multi_rleadv");
	RLE_advanced_pack(src, tmp, profile);
	bool compression_success = testPack(src, tmp, "RLE Advanced", 100);
	if (compression_success) {

		if (DOUBLE_CHECK_PACK) {
			//test if compression worked!
			const char tmp2[100] = { 0 };
			get_temp_file2(tmp2, "multi_maksure");
			RLE_advanced_unpack(tmp, tmp2);
			bool sc = files_equal(tmp2, src);
			if (!sc) {
				printf("\n\n\n ** Failed to RLE advance pack: %s", src);
				exit(1);
			}
			remove(tmp2);
		}		
		my_rename(tmp, src);
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
	my_rename(tmp, src);
}

void MultiUnpackAndReplace(const char* src) {
	//printf("\n Seq unpacking (in place) %s", src);
	const char tmp[100] = { 0 };
	get_temp_file2(tmp, "multi_sequnpacked");
	multi_unpack(src, tmp);	
	my_rename(tmp, src);
}

void TwoByteUnpackAndReplace(const char* src) {
	const char tmp[100] = { 0 };
	get_temp_file2(tmp, "multi_twobytedunp");
	two_byte_unpack(src, tmp);	
	my_rename(tmp, src);
}

void RLEAdvancedUnpackAndReplace(const char* src) {

	const char tmp[100] = { 0 };
	get_temp_file2(tmp, "multi_rleadvunp");
	RLE_advanced_unpack(src, tmp);	
	my_rename(tmp, src);
}

//----------------------------------------------------------------------------------------

void multi_pack(const char* src, const char* dst, packProfile_t profile,
	packProfile_t seqlensProfile, packProfile_t offsetsProfile) {
 
	printf("\n* Multi pack * %s => %s", src,dst);
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
		if (got_smaller) {
			pack_type = setKthBit(pack_type, 6);
		}
		uint64_t before_seqpack_size = get_file_size_from_name(temp_filename);	
		seq_pack_separate(temp_filename, base_dir, profile.offset_pages, profile.seqlen_pages);

		const char seqlens_name[100] = { 0 };
		const char offsets_name[100] = { 0 };
		const char main_name[100] = { 0 };
		concat(seqlens_name, base_dir, "seqlens");
		concat(offsets_name, base_dir, "offsets");
		concat(main_name, base_dir, "main");

		if (DOUBLE_CHECK_PACK) {
			//test if seq_compression worked!
			const char tmp2[100] = { 0 };
			get_temp_file2(tmp2, "multi_maksureseqpack");
			seq_unpack_separate(main_name, tmp2, base_dir);
			bool sc = files_equal(tmp2, temp_filename);
			if (!sc) {
				printf("\n\n\n ** Failed to seq pack seperate: %s", temp_filename);
				exit(1);
			}
			remove(tmp2);
		}

		//try to pack meta files!

		
		if (get_file_size_from_name(seqlens_name) > profile.recursive_limit) {

			// ---------- Pack the meta files (seqlens/offsets) recursively
			got_smaller = MultiPackAndTest(seqlens_name, seqlensProfile, seqlensProfile, offsetsProfile);
			if (got_smaller) {
				pack_type = setKthBit(pack_type, 1);
			}
		}
		if (get_file_size_from_name(offsets_name) > profile.recursive_limit) {

			got_smaller = MultiPackAndTest(offsets_name, offsetsProfile, seqlensProfile, offsetsProfile);
			if (got_smaller) {
				pack_type = setKthBit(pack_type, 2);
			}
		}
		char main_rle_packed[100] = { 0 };
		get_temp_file2(main_rle_packed, "multi.main_rle_packed");
		RLE_advanced_pack(main_name, main_rle_packed);
		uint64_t meta_size = get_file_size_from_name(offsets_name) +
			get_file_size_from_name(seqlens_name);
		uint64_t size_after_seqpack = meta_size + get_file_size_from_name(main_name);

		uint64_t size_after_seq_and_rle = meta_size + get_file_size_from_name(offsets_name);

		double seqPackRatio = ((double)size_after_seqpack) /
			((double)before_seqpack_size);

		bool seqPacked = seqPackRatio < ((double)profile.seq_ratio / (double)100);
		
		printf("\n Seqpacked %s and got ratio %.1f (limit %d)", temp_filename, seqPackRatio * 100.0, profile.seq_ratio);
		char seqpacked_fallback[100] = { 0 };
		if (seqPacked) {
			pack_type = setKthBit(pack_type, 7);
		    remove(temp_filename);
		}
		else {		
			get_temp_file2(seqpacked_fallback, "multi_seqpackedfallback");
			copy_file(main_name, seqpacked_fallback);
			my_rename(temp_filename, main_name);
		}
		char canonicalled[100];
		get_temp_file2(canonicalled, "multi_canonicalled");
		uint64_t size_before_canonical = get_file_size_from_name(main_name);
		CanonicalEncode(main_name, canonicalled);
		uint64_t size_after_canonical = get_file_size_from_name(canonicalled);
		if (size_after_canonical < size_before_canonical) {
			if (seqPacked ||
				(!seqPacked && size_after_canonical < math_min(size_after_seq_and_rle, size_after_seqpack))) {
				pack_type = setKthBit(pack_type, 0);
				my_rename(canonicalled, main_name);
			}
			else { // Canonical didn't work

				if (!seqPacked) {
					if (seqPackRatio < 1) {
						printf("\n ** regret myself since huffman failed/was worse than seqpack, take back seqpack w ratio %f", seqPackRatio);
						pack_type = setKthBit(pack_type, 7);
						seqPacked = true;
						if (size_after_seq_and_rle >= size_after_seqpack) {
							my_rename(seqpacked_fallback, main_name);
						}
						else {
							my_rename(main_rle_packed, main_name);
							pack_type = setKthBit(pack_type, 4);
						}
					}
					//no use try RLE advanced here gave no improvement
				}
			}
		}
		if (!isKthBitSet(pack_type, 0)) {
			//two byte packing here just made compression worse.. not sure why...
		}
		printf("\nTar writing destination file: %s basedir:%s\nPack_type = %d", dst, base_dir, pack_type);
		size_after_seqpack = get_file_size_from_name(main_name) +
			(seqPacked ? get_file_size_from_name(offsets_name) +
				get_file_size_from_name(seqlens_name) + 8 : 0);

		uint64_t source_size = get_file_size_from_name(src);
		do_store = size_after_seqpack >= source_size;
		if (!do_store) {
			tar(dst, base_dir, pack_type);			
		} 
		remove(seqpacked_fallback);
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
	unsigned char pack_type;
	fread(&pack_type, 1, 1, in);
	pi.pack_type = pack_type;
	if (pack_type == 0) {
		printf("\n  UNSTORE!!  ");
		unstore(in, dst);
		return;
	}
	char base_dir[100] = { 0 };
	get_clock_dir(base_dir);
	pi.dir = base_dir;
	untar(in, pi);

	const char seqlens_name[100] = { 0 };
	const char offsets_name[100] = { 0 };
	const char main_name[100] = { 0 };
	concat(seqlens_name, base_dir, "seqlens");
	concat(offsets_name, base_dir, "offsets");
	concat(main_name, base_dir, "main");

	if (isKthBitSet(pack_type, 0)) { //main was huffman coded
		CanonicalDecodeAndReplace(main_name);
	}
	else {
		if (isKthBitSet(pack_type, 3)) {
			TwoByteUnpackAndReplace(main_name);
		}
		if (isKthBitSet(pack_type, 4)) {
			printf("\n RLE advance unpack");
			RLEAdvancedUnpackAndReplace(main_name);
		}			
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
		my_rename(main_name, seq_dst);
	}
	if (isKthBitSet(pack_type, 6)) {
		TwoByteUnpackAndReplace(seq_dst);
	}
	if (isKthBitSet(pack_type, 5)) {
		RLE_simple_unpack(seq_dst, dst);
	}
	else {
		my_rename(seq_dst, dst);
	}
	remove(seq_dst);

	remove(seqlens_name);
	remove(offsets_name);
	remove(main_name);
}