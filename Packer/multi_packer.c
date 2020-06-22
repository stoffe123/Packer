#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_packer.h"  
#include "common_tools.h"
#include "multi_packer.h"
#include "packer_commons.h"
#include "Two_byte_packer.h"
#include "RLE_packer_advanced.h"
#include "RLE_simple_packer.h"
#include "canonical.h"

#define META_SIZE_MAX 1048575

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

bool isSeqPacked(int packType) {
	return isKthBitSet(packType, 7);
}

bool isCanonicalHeaderPacked(int packType) {
	return (isKthBitSet(packType, 1) && isKthBitSet(packType, 2) && !isKthBitSet(packType, 7));
}

void copyMeta(FILE* in, pack_info_t pi, const char* name, long size) {
	const char metaName[100] = { 0 };
	concat(metaName, pi.dir, name);
	copy_chunk(in, metaName, size);
}
void untar(FILE* in, pack_info_t pi) {

	if (isKthBitSet(pi.pack_type, 7)) {
		
		uint32_t seqlens_size = 0;
		uint32_t offsets_size = 0;
		uint32_t distances_size = 0;

		fread(&seqlens_size, 3, 1, in);
		fread(&offsets_size, 3, 1, in);
		fread(&distances_size, 3, 1, in);

		copyMeta(in, pi, "seqlens", seqlens_size);
		copyMeta(in, pi, "offsets", offsets_size);
		copyMeta(in, pi, "distances", distances_size);
	}
	const char main_name[100] = { 0 };
	concat(main_name, pi.dir, "main");
	copy_the_rest(in, main_name);
	fclose(in);
}

void store(const char* src, const char* dst, uint8_t pack_type, bool storePackType) {
	FILE* out_file = fopen(dst, "wb");
	if (storePackType) {
		fwrite(&pack_type, 1, 1, out_file);
	}
	append_to_file(out_file, src);
	fclose(out_file);
}

uint8_t tar(const char* dst, const char* base_dir, uint8_t packType, bool storePackType) {

	const char seqlens_name[100] = { 0 };
	const char offsets_name[100] = { 0 };
	const char distances_name[100] = { 0 };
	concat(seqlens_name, base_dir, "seqlens");
	concat(offsets_name, base_dir, "offsets");
	concat(distances_name, base_dir, "distances");
	uint32_t size_seqlens = 0;
	uint32_t size_offsets = 0;
	uint32_t size_distances = 0;
	FILE* outFile = fopen(dst, "wb");

	if (isKthBitSet(packType, 7)) {
		size_seqlens = get_file_size_from_name(seqlens_name);
		size_offsets = get_file_size_from_name(offsets_name);
		size_distances = get_file_size_from_name(distances_name);
	}
	if (storePackType) {
		fwrite(&packType, 1, 1, outFile);
	}

	if (isKthBitSet(packType, 7)) {

		fwrite(&size_seqlens, 3, 1, outFile);
		fwrite(&size_offsets, 3, 1, outFile);
		fwrite(&size_distances, 3, 1, outFile);

		append_to_file(outFile, seqlens_name);
		append_to_file(outFile, offsets_name);
		append_to_file(outFile, distances_name);
	}
	const char main_name[100] = { 0 };
	concat(main_name, base_dir, "main");
	append_to_file(outFile, main_name);

	fclose(outFile);
	return packType;
}

void my_rename(const char* f_old, const char* f_new) {
	if (!equals(f_old, f_new)) {
		remove(f_new);
		rename(f_old, f_new);
	}
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
	return packAndTest("twobyte", src, profile, profile, profile, profile);
}

bool RLEAdvancedPackAndTest(const char* src, packProfile profile) {
	return packAndTest("rle advanced", src, profile, profile, profile, profile);
}

void unpackAndReplace(const char* kind, const char* src) {
	const char tmp[100] = { 0 };
	const char tmp2[100] = { 0 };
	concat(tmp2, "multiunp_", kind);
	getTempFile(tmp, tmp2);
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
 3   distances multipack
 4 - last block chunk
 5 - RLE simple
 6 - Two byte 
 7 - Sequence pack

 Canonical header pack =>  bit 7 = 0   bit 1 = 1   bit 2 = 1
 */

uint8_t multiPackInternal(const char* src, const char* dst, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile, packProfile distancesProfile, bool storePackType) {
	static int metacount = 101;

	int canonicalRecursiveLimit = 20;
 
	packCandidate_t packCandidates[100];
	int candidatesIndex = 0;

	printf("\n* Multi pack * %s => %s", src,dst);
	//printProfile(&profile);
	unsigned long long source_size = get_file_size_from_name(src);
	unsigned char pack_type = 0;
		
	packProfile twobyte100Profile = getPackProfile(0, 0, 0);
	twobyte100Profile.twobyte_ratio = 100;
	twobyte100Profile.twobyte_threshold_divide = 1;
	twobyte100Profile.twobyte_threshold_max = 3;
	twobyte100Profile.twobyte_threshold_min = 3;

	bool do_store = source_size < 6;
	if (!do_store) {
		char before_seqpack[100] = { 0 };
		getTempFile(before_seqpack, "multi_rlepacked");

		packProfile prof = getPackProfile(0, 0, 0);
		prof.twobyte_ratio = 90;
		prof.rle_ratio = 90;
		prof.recursive_limit = 10;
		prof.twobyte_threshold_divide = 100;
		prof.twobyte_threshold_max = 100;
		prof.twobyte_threshold_min = 20;

		
		char slim_multipacked[100];
		uint8_t slimPackType = 0;
		/*
		if (profile.seqlen_pages + profile.offset_pages > 0 && source_size > 10) {	
			getTempFile(slim_multipacked, "multi_multipacked");			
			slimPackType = multiPackInternal(src, slim_multipacked, prof, prof, prof, storePackType);			
			packCandidates[candidatesIndex++] = getPackCandidate(slim_multipacked, 0);
		}
		*/

		if (source_size < 300) {
			char head_pack[100] = { 0 };
			getTempFile(head_pack, "multi_head_pack");
			canonical_header_pack(src, head_pack);
			int pt = setKthBit(0, 1);
			pt = setKthBit(pt, 2);
			packCandidates[candidatesIndex++] = getPackCandidate(head_pack, pt);
		}

		bool got_smaller = RLE_pack_and_test(src, before_seqpack, profile.rle_ratio);
		if (got_smaller) {
			pack_type = setKthBit(pack_type, 5);
		}
	
		char just_two_byte[100] = { 0 };
		getTempFile(just_two_byte, "multi_just_two_byte");		
		two_byte_pack(src, just_two_byte, twobyte100Profile);
		packCandidates[candidatesIndex++] = getPackCandidate(just_two_byte, 0b1000000);

		if (source_size > canonicalRecursiveLimit) {
			char just_canonical[100] = { 0 };
			getTempFile(just_canonical, "multi_just_canonical");
			CanonicalEncode(src, just_canonical);
			packCandidates[candidatesIndex++] = getPackCandidate(just_canonical, 1);
		}
		
		char base_dir[100] = { 0 };
		get_clock_dir(base_dir);
		got_smaller = TwoBytePackAndTest(before_seqpack, profile);
		if (got_smaller) {
			pack_type = setKthBit(pack_type, 6);
		}
		packCandidates[candidatesIndex++] = getPackCandidate(before_seqpack, pack_type);
		uint64_t before_seqpack_size = get_file_size_from_name(before_seqpack);	
		
		if (source_size > canonicalRecursiveLimit) {
			char canonical_instead_of_seqpack[100] = { 0 };
			getTempFile(canonical_instead_of_seqpack, "multi_canonical_instead_of_seqpack");
			CanonicalEncode(before_seqpack, canonical_instead_of_seqpack);
			packCandidates[candidatesIndex++] = getPackCandidate(canonical_instead_of_seqpack, setKthBit(pack_type, 0));
		}		
		seq_pack_separate(before_seqpack, base_dir, profile);

		const char seqlens_name[100] = { 0 };
		const char offsets_name[100] = { 0 };
		const char distances_name[100] = { 0 };
		const char main_name[100] = { 0 };
		concat(seqlens_name, base_dir, "seqlens");
		concat(offsets_name, base_dir, "offsets");
		concat(distances_name, base_dir, "distances");
		concat(main_name, base_dir, "main");

		if (DOUBLE_CHECK_PACK) {
			printf("\n double checking the seqpack of: %s", before_seqpack);
			const char tmp2[100] = { 0 };
			getTempFile(tmp2, "multi_maksureseqpack");
			seq_unpack_separate(main_name, tmp2, base_dir);
			doDoubleCheck(tmp2, before_seqpack, "seq");
		}

		//try to pack meta files!
		
		if (get_file_size_from_name(seqlens_name) > profile.recursive_limit) {

			// ---------- Pack the meta files (seqlens/offsets) recursively
			got_smaller = MultiPackAndTest(seqlens_name, seqlensProfile, seqlensProfile, offsetsProfile, distancesProfile);
			if (got_smaller) {
				pack_type = setKthBit(pack_type, 1);
			}
		}
		if (get_file_size_from_name(offsets_name) > profile.recursive_limit) {

			got_smaller = MultiPackAndTest(offsets_name, offsetsProfile, seqlensProfile, offsetsProfile, distancesProfile);
			if (got_smaller) {
				pack_type = setKthBit(pack_type, 2);
			}
		}
		if (get_file_size_from_name(distances_name) > profile.recursive_limit) {

			got_smaller = MultiPackAndTest(distances_name, distancesProfile, seqlensProfile, offsetsProfile, distancesProfile);
			if (got_smaller) {
				pack_type = setKthBit(pack_type, 3);
			}
		}
		/*
		
		char tmp7[100] = { 0 };
		concat_int(tmp7, "c:/test/meta/offsets", metacount);
		copy_file(offsets_name, tmp7);
		concat_int(tmp7, "c:/test/meta/seqlens", metacount);
		copy_file(seqlens_name, tmp7);
		concat_int(tmp7, "c:/test/meta/distances", metacount);
		copy_file(distances_name, tmp7);
		metacount++;
		*/


		uint64_t offsets_size = get_file_size_from_name(offsets_name);
		uint64_t seqlens_size = get_file_size_from_name(seqlens_name);
		uint64_t distances_size = get_file_size_from_name(distances_name);
		uint64_t meta_size = offsets_size +
			seqlens_size + distances_size + 9;
		uint64_t size_after_seq = meta_size + get_file_size_from_name(main_name);

		bool metaTooLarge = false; //  offsets_size > META_SIZE_MAX || seqlens_size > META_SIZE_MAX;
		if (metaTooLarge) {
			printf("\n meta size too large for file %s .. size was %d and %d", src, offsets_size, seqlens_size);
		}

		if (size_after_seq < before_seqpack_size && !metaTooLarge) {
			//printf("\n Normal seqpack worked with ratio %.3f", (double)size_after_seq / (double)before_seqpack_size);
			pack_type = setKthBit(pack_type, 7);
			remove(before_seqpack);
			if (get_file_size_from_name(main_name) > canonicalRecursiveLimit) {
				char canonicalled[100];
				getTempFile(canonicalled, "multi_canonicalled");
				uint64_t size_before_canonical = get_file_size_from_name(main_name);
				CanonicalEncode(main_name, canonicalled);
				uint64_t size_after_canonical = get_file_size_from_name(canonicalled);
				if (size_after_canonical < size_before_canonical) {
					pack_type = setKthBit(pack_type, 0);
					my_rename(canonicalled, main_name);
				}
				remove(canonicalled);
			}
			packCandidates[candidatesIndex++] = getPackCandidate2(main_name, pack_type, get_file_size_from_name(main_name) + meta_size);
		}
		else {
			//seqpack not used so clear metafile pack bits
			pack_type = clearKthBit(pack_type, 1);
			pack_type = clearKthBit(pack_type, 2);
		}
		packCandidate_t bestCandidate = packCandidates[0];
		for (int i = 1; i < candidatesIndex; i++) {
			if (packCandidates[i].size < bestCandidate.size) {
				bestCandidate = packCandidates[i];				
			}
		}
		do_store = bestCandidate.size >= source_size;
		if (!do_store) {
			printf("\nWinner is %s  packed %d > %d", bestCandidate.filename, source_size, bestCandidate.size);
			pack_type = bestCandidate.packType;
			if (equals(bestCandidate.filename, slim_multipacked)) {
				printf("\n Using only slimseq");
				my_rename(slim_multipacked, dst);	
				pack_type = slimPackType;
			}
			else {
				my_rename(bestCandidate.filename, main_name);
				printf("\nTar writing destination file: %s basedir:%s\nPack_type = %d", dst, base_dir, pack_type);
				pack_type = tar(dst, base_dir, pack_type, storePackType);
			}
		}								
		for (int i = 0; i < candidatesIndex; i++) {
			remove(packCandidates[i].filename);				
		}
		remove(seqlens_name);
		remove(offsets_name);
		remove(distances_name);
		remove(main_name);		
	}
	if (do_store) {
		
		pack_type = 0;
		
		printf("\n  CHOOOSING STORE in multi_packer");
		store(src, dst, pack_type, storePackType);	
	}
	return pack_type;
}

// ----------------------------------------------------------------



void multiUnpackInternal(const char* src, const char* dst, uint8_t pack_type, bool readPackTypeFromFile) {
	pack_info_t pi;
	FILE* in = fopen(src, "rb");
	if (in == NULL) {
		printf("\n Couldn't open file %s ", src);
		exit(1);
	}
	if (readPackTypeFromFile) {
		fread(&pack_type, 1, 1, in);
	}
	printf("\n Multiunpack of %s  =>  %s   with packtype %d", src, dst, pack_type);
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

	const char main_name[100] = { 0 };
	const char seqlens_name[100] = { 0 };
	const char offsets_name[100] = { 0 };
	const char distances_name[100] = { 0 };
	concat(main_name, base_dir, "main");
	concat(seqlens_name, base_dir, "seqlens");
	concat(offsets_name, base_dir, "offsets");
	concat(distances_name, base_dir, "distances");
	

	if (isKthBitSet(pack_type, 0)) { //main was huffman coded
		CanonicalDecodeAndReplace(main_name);
	}	
	bool seqPacked = isKthBitSet(pack_type, 7);
	if (seqPacked) {
		if (isKthBitSet(pack_type, 1)) {
			MultiUnpackAndReplace(seqlens_name);
		}
		if (isKthBitSet(pack_type, 2)) {			
			MultiUnpackAndReplace(offsets_name);
		}
		if (isKthBitSet(pack_type, 3)) {
			MultiUnpackAndReplace(distances_name);
		}
	}
	const char seq_dst[100] = { 0 };
	getTempFile(seq_dst, "multi_seqsepunp");
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
	if (isCanonicalHeaderPacked(pack_type)) {
		canonical_header_unpack(seq_dst, dst);
	}
	else {
		if (isKthBitSet(pack_type, 5)) {
			RLE_simple_unpack(seq_dst, dst);
		}
		else {
			my_rename(seq_dst, dst);
		}
	}
	remove(seq_dst);

	remove(seqlens_name);
	remove(offsets_name);
	remove(main_name);
}

uint8_t multiPack(const char* src, const char* dst, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile, packProfile distancesProfile) {
	return multiPackInternal(src, dst, profile, seqlensProfile, offsetsProfile, distancesProfile, false);
}

void multi_pack(const char* src, const char* dst, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile, packProfile distancesProfile) {
	multiPackInternal(src, dst, profile, seqlensProfile, offsetsProfile, distancesProfile, true);
}

void multiUnpack(const char* src, const char* dst, uint8_t pack_type) {
	multiUnpackInternal(src, dst, pack_type, false);
}

void multi_unpack(const char* src, const char* dst) {
	multiUnpackInternal(src, dst, 0, true);
}