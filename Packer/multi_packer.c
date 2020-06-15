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

typedef struct packCandidate_t {
	const char* filename;
	unsigned char packType;
	uint64_t size;
} packCandidate_t;


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

packCandidate_t getPackCandidate2(const char* filename, unsigned char packType, uint64_t filesize) {
	packCandidate_t cand;
	cand.filename = filename;
	cand.packType = packType;
	if (filesize == 0) {
		filesize = get_file_size_from_name(filename);
	}
	cand.size = filesize;
	return cand;
}

packCandidate_t getPackCandidate(const char* filename, unsigned char packType) {
	return getPackCandidate2(filename, packType, 0);
}



/* ----------------------------------------------------------------------------------------

 Bit layout packtype
 0 - canonical
 1 - seqlens multipack 
 2 - offset multipack
 3 - 
 4 - 
 5 - RLE simple
 6 - Two byte 
 7 - 
 */

void multi_pack(const char* src, const char* dst, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile) {

	int canonicalRecursiveLimit = 1;
 
	packCandidate_t packCandidates[100];
	int candidatesIndex = 0;

	printf("\n* Multi pack * %s => %s", src,dst);
	//printProfile(&profile);
	unsigned long long source_size = get_file_size_from_name(src);
	unsigned char pack_type = 0;
		
	packProfile twobyte100Profile = getPackProfile(0, 0);
	twobyte100Profile.twobyte_ratio = 100;
	twobyte100Profile.twobyte_threshold_divide = 1;
	twobyte100Profile.twobyte_threshold_max = 3;
	twobyte100Profile.twobyte_threshold_min = 3;

	bool do_store = source_size < 6;
	if (!do_store) {
		char before_seqpack[100] = { 0 };
		get_temp_file2(before_seqpack, "multi_rlepacked");

		packProfile prof = getPackProfile(0, 0);
		prof.twobyte_ratio = 90;
		prof.rle_ratio = 90;
		prof.recursive_limit = 10;
		prof.twobyte_threshold_divide = 100;
		prof.twobyte_threshold_max = 100;
		prof.twobyte_threshold_min = 20;

		char slim_multipacked[100];
		if (profile.seqlen_pages + profile.offset_pages > 0 && source_size > 10) {	
			get_temp_file2(slim_multipacked, "multi_multipacked");
			multi_pack(src, slim_multipacked, prof, prof, prof);
			packCandidates[candidatesIndex++] = getPackCandidate(slim_multipacked, 0);
		}



		bool got_smaller = RLE_pack_and_test(src, before_seqpack, profile.rle_ratio);
		if (got_smaller) {
			pack_type = setKthBit(pack_type, 5);
		}
	
		char just_two_byte[100] = { 0 };
		get_temp_file2(just_two_byte, "multi_just_two_byte");		
		two_byte_pack(src, just_two_byte, twobyte100Profile);
		packCandidates[candidatesIndex++] = getPackCandidate(just_two_byte, 0b1000000);

		if (source_size > canonicalRecursiveLimit) {
			char just_canonical[100] = { 0 };
			get_temp_file2(just_canonical, "multi_just_canonical");
			CanonicalEncode(src, just_canonical);
			packCandidates[candidatesIndex++] = getPackCandidate(just_canonical, 1);
		}
		
		char base_dir[100] = { 0 };
		get_clock_dir(base_dir);
		got_smaller = TwoBytePackAndTest(before_seqpack, profile);
		if (got_smaller) {
			pack_type = setKthBit(pack_type, 6);
		}
		uint64_t before_seqpack_size = get_file_size_from_name(before_seqpack);	
		
		if (source_size > canonicalRecursiveLimit) {
			char canonical_instead_of_seqpack[100] = { 0 };
			get_temp_file2(canonical_instead_of_seqpack, "multi_canonical_instead_of_seqpack");
			CanonicalEncode(before_seqpack, canonical_instead_of_seqpack);
			packCandidates[candidatesIndex++] = getPackCandidate(canonical_instead_of_seqpack, setKthBit(pack_type, 0));
		}
		
		seq_pack_separate(before_seqpack, base_dir, profile);

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
			doDoubleCheck(tmp2, before_seqpack, "seq");
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
		uint64_t meta_size = get_file_size_from_name(offsets_name) +
			get_file_size_from_name(seqlens_name);
		uint64_t size_after_multipack = meta_size + get_file_size_from_name(main_name);

		double seqPackRatio = ((double)size_after_multipack) /
			((double)before_seqpack_size);

		printf("\n Seqpacked %s and got ratio %.3f (limit %d)", before_seqpack, seqPackRatio * 100.0, profile.seq_ratio);
		if (seqPackRatio < 1) {
			pack_type = setKthBit(pack_type, 7);
			remove(before_seqpack);
		}
		else {
			my_rename(before_seqpack, main_name);
		}
		
		if (get_file_size_from_name(main_name) > canonicalRecursiveLimit) {
			char canonicalled[100];
			get_temp_file2(canonicalled, "multi_canonicalled");
			uint64_t size_before_canonical = get_file_size_from_name(main_name);
			CanonicalEncode(main_name, canonicalled);
			uint64_t size_after_canonical = get_file_size_from_name(canonicalled);
			if (size_after_canonical < size_before_canonical) {
				pack_type = setKthBit(pack_type, 0);
				my_rename(canonicalled, main_name);
			}
			remove(canonicalled);
		}
		uint64_t size_including_seq = get_file_size_from_name(main_name) + meta_size;
		

		packCandidate_t bestCandidate = packCandidates[0];
		for (int i = 1; i < candidatesIndex; i++) {
			if (packCandidates[i].size < bestCandidate.size) {
				bestCandidate = packCandidates[i];				
			}
		}
		if (bestCandidate.size < size_including_seq && bestCandidate.size + 1 < source_size) {
			pack_type = bestCandidate.packType;
			if (equals(bestCandidate.filename, slim_multipacked)) {
				my_rename(slim_multipacked, dst);
				return;
			}
			my_rename(bestCandidate.filename, main_name);
		}
		
		printf("\nTar writing destination file: %s basedir:%s\nPack_type = %d", dst, base_dir, pack_type);
		
		size_after_multipack = get_file_size_from_name(main_name) +
			(isKthBitSet(pack_type, 7) ? meta_size + 8 : 0);

		do_store = size_after_multipack + 1 >= source_size;
		if (!do_store) {
            
			tar(dst, base_dir, pack_type);			
		} 
		for (int i = 0; i < candidatesIndex; i++) {
			remove(packCandidates[i].filename);				
		}
		remove(seqlens_name);
		remove(offsets_name);
		remove(main_name);		
	}
	if (do_store) {
		
		pack_type = 0;
		
		printf("\n  CHOOOSING STORE in multi_packer !!! ");
		store(src, dst, pack_type);	
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