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
	bool compression_success = testPack(src, dst, "RLE simple", ratio);
	if (!compression_success) {
		remove(dst);
		copy_file(src, dst);//src has to remain!
	}
	return compression_success;
}


bool TwoBytePackAndTest(const char* src, packProfile profile) {
	return packAndTest("twobyte", src, profile, profile, profile);
}

bool RLEAdvancedPackAndTest(const char* src, packProfile profile) {
	return packAndTest("rle advanced", src, profile, profile, profile);
}

void unpackAndReplace(const char* kind, const char* src) {
	const char tmp[100] = { 0 };
	const char tmp2[100] = { 0 };
	concat(tmp2, "multiunp_", kind);
	get_temp_file2(tmp, tmp2);
	unpackByKind(kind, src, tmp);
	my_rename(tmp, src);
}

void RLEAdvancedUnpackAndReplace(const char* src) {
	unpackAndReplace("rle advanced", src);
}

void TwoByteUnpackAndReplace(const char* src) {
	unpackAndReplace("twobyte", src);
}

void MultiUnpackAndReplace(const char* src) {
	unpackAndReplace("multi", src);
}

void CanonicalDecodeAndReplace(const char* src) {
	unpackAndReplace("canonical", src);
}

//----------------------------------------------------------------------------------------

void multi_pack(const char* src, const char* dst, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile) {
 
	printf("\n* Multi pack * %s => %s", src,dst);
	//printProfile(&profile);
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
		seq_pack_separate(temp_filename, base_dir, profile);

		const char seqlens_name[100] = { 0 };
		const char offsets_name[100] = { 0 };
		const char main_name[100] = { 0 };
		concat(seqlens_name, base_dir, "seqlens");
		concat(offsets_name, base_dir, "offsets");
		concat(main_name, base_dir, "main");

		if (DOUBLE_CHECK_PACK) {
			const char tmp2[100] = { 0 };
			get_temp_file2(tmp2, "multi_maksureseqpack");
			seq_unpack_separate(main_name, tmp2, base_dir);
			doDoubleCheck(tmp2, temp_filename, "seq");
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
		if (size_after_canonical < size_before_canonical &&
			(seqPacked ||
				(!seqPacked && size_after_canonical < math_min(size_after_seq_and_rle, size_after_seqpack)))) {
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
		if (!isKthBitSet(pack_type, 0)) {

			packProfile twobyte2Profile;
			twobyte2Profile.twobyte_ratio = profile.twobyte2_ratio;
			twobyte2Profile.twobyte_threshold_max = profile.twobyte2_threshold_max;
			twobyte2Profile.twobyte_threshold_divide = profile.twobyte2_threshold_divide;
			twobyte2Profile.twobyte_threshold_min = profile.twobyte2_threshold_min;
			got_smaller = TwoBytePackAndTest(main_name, twobyte2Profile);
			if (got_smaller) {
				pack_type = setKthBit(pack_type, 3);
			}
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
		remove(canonicalled);
		remove(main_rle_packed);
		remove(seqpacked_fallback);
		remove(seqlens_name);
		remove(offsets_name);
		remove(main_name);		
	}
	if (do_store) {

		pack_type = 0;
		char tempsrc[100];
		get_temp_file2(tempsrc, "multi_tempsrc");
		copy_file(src, tempsrc);
		RLE_advanced_pack(src, tempsrc);
		if (get_file_size_from_name(tempsrc) < get_file_size_from_name(src)) {
			printf("\n  RLE_advanced worked!!!!! instead of store");
			pack_type = setKthBit(pack_type, 4);
		}
		else {
			packProfile prof = getPackProfile(0, 0);
			prof.twobyte_ratio = 100;
			prof.twobyte_threshold_divide = 1;
			prof.twobyte_threshold_max = 3;
			prof.twobyte_threshold_min = 3;
			two_byte_pack(src, tempsrc, prof);
			if (get_file_size_from_name(tempsrc) < get_file_size_from_name(src)) {
				printf("\n  Two-byte worked!!!!! instead of store");
				pack_type = setKthBit(pack_type, 6);
				
			}
		}
		if (pack_type == 0) {

			printf("\n  CHOOOSING STORE in multi_packer !!! ");
			store(src, dst, pack_type);
		}
		else {
			store(tempsrc, dst, pack_type);
		}
	}
	//printf("\n => result: %s  size:%d", dst, get_file_size_from_name(dst));
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