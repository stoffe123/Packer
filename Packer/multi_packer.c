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
#include "halfbyte_rle_packer.h"
#include "seq_packer_commons.h"
#include "memfile.h"

#define META_SIZE_MAX 1048575

#define RLE_BIT 5
#define TWOBYTE_BIT 6

static 	packProfile twobyte100Profile = {
.twobyte_ratio = 100,
.twobyte_threshold_divide = 1,
.twobyte_threshold_max = 3,
.twobyte_threshold_min = 3
};

typedef struct packCandidate_t {
	memfile* filename;
	unsigned char packType;
	uint64_t size;
	bool canBeFreed;
} packCandidate_t;

bool isSeqPacked(int packType) {
	return isKthBitSet(packType, 7);
}




seqPackBundle untar(memfile* in, uint8_t packType) {

	seqPackBundle res;
	res.seqlens = NULL;
	res.offsets = NULL;
	res.distances = NULL;
	if (isKthBitSet(packType, 7)) {
		
		uint32_t seqlens_size = 0;
		uint32_t offsets_size = 0;
		uint32_t distances_size = 0;

		memRead(&seqlens_size, 3, in);
		memRead(&offsets_size, 3, in);
		memRead(&distances_size, 3, in);

		res.seqlens = getMemfile(seqlens_size, L"multipack_untar_seqlens");
		res.offsets = getMemfile(offsets_size, L"multipack_untar_offsets");
		res.distances = getMemfile(distances_size, L"multipack_untar_distances");

		copy_chunk_mem(in, res.seqlens, seqlens_size);
		copy_chunk_mem(in, res.offsets, offsets_size);
		copy_chunk_mem(in, res.distances, distances_size);
	}
	res.main = getMemfile((uint64_t)1 + getMemSize(in) - getMemPos(in), L"multipack_untar_main");
	copy_the_rest_mem(in, res.main);
	return res;
}



uint8_t tar(memfile* outFile, seqPackBundle mf_arr, uint8_t packType, bool storePackType) {

	rewindMem(outFile);
	uint64_t size_seqlens = 0;
	uint64_t size_offsets = 0;
	uint64_t size_distances = 0;

	if (storePackType) {
		fputcc(packType, outFile);
	}
	bool seqPacked = isKthBitSet(packType, 7);


	if (seqPacked) {
		size_seqlens = getMemSize(mf_arr.seqlens);
		size_offsets = getMemSize(mf_arr.offsets);
		size_distances = getMemSize(mf_arr.distances);
	}
	uint64_t sizeOut = getMemSize(mf_arr.main) + size_seqlens + size_offsets + size_distances;
	checkAlloc(outFile, sizeOut);
	if (seqPacked) {
		memWrite(&size_seqlens, 3, outFile);
		memWrite(&size_offsets, 3, outFile);
		memWrite(&size_distances, 3, outFile);

		append_to_mem(outFile, mf_arr.seqlens);
		append_to_mem(outFile, mf_arr.offsets);
		append_to_mem(outFile, mf_arr.distances);
	}
	append_to_mem(outFile, mf_arr.main);
	return packType;
}

int packAndTest2(wchar_t* kind, memfile* src, packProfile profile, int pt, int bit) {
	completePackProfile comp = getCompletePackProfileSimple(profile);
	bool succ = packAndTest(kind, src, comp);
	if (succ) {
		pt = setKthBit(pt, bit);
	}
	return pt;
}

memfile* unpackAndReplace2(const wchar_t* kind, memfile* src) {
	memfile* tmp = unpackByKind(kind, src);
	freeMem(src);
	return tmp;
}

int getPackCandidate2(packCandidate_t* bestCandidate, memfile* m, int packType, uint64_t size) {
	//assert(getMemSize(m) <= BLOCK_SIZE && size <= BLOCK_SIZE, "multi_packer candidate size over BLOCK_SIZE");

	if (size < bestCandidate->size) {
		if (bestCandidate->canBeFreed) {
			freeMem(bestCandidate->filename);
		}
		bestCandidate->filename = m;
		bestCandidate->size = size;
		bestCandidate->packType = packType;
		bestCandidate->canBeFreed = false;		
	}
	
}

void getPackCandidate3(packCandidate_t* bestCandidate, memfile* m, int packType, bool canonicalHeaderCase) {
	uint64_t size = getMemSize(m);
	if (canonicalHeaderCase) {
		if (packType == packTypeForHalfbyteRlePack(0) || packType == packTypeRlePlusTwobyte()) {
			// those two cases are separately treated in canonical.c and no packtype has to be written then
		    size--;
		}
	}
	getPackCandidate2(bestCandidate, m, packType, size);
}

void getPackCandidate(packCandidate_t* bestCandidate, memfile* m, int packType) {
	getPackCandidate2(bestCandidate, m, packType, getMemSize(m));
}

void getPackCandidateAndFree(packCandidate_t* bestCandidate, memfile* m, int packType) {
	getPackCandidate2(bestCandidate, m, packType, getMemSize(m));
	if (bestCandidate->filename != m) {
		freeMem(m);
	}
	else {
		bestCandidate->canBeFreed = true;
	}
}

void getPackCandidate3AndFree(packCandidate_t* bestCandidate, memfile* m, int packType, 
	bool canonicalHeaderCase) {
	getPackCandidate3(bestCandidate, m, packType, canonicalHeaderCase);
	if (bestCandidate->filename != m) {
		freeMem(m);
	}
	else {
		bestCandidate->canBeFreed = true;
	}
}

static packProfile prof = { .rle_ratio = 0,
			.twobyte_ratio = 0,
			.recursive_limit = 10,
			.twobyte_threshold_max = 11750,
			.twobyte_threshold_divide = 20,
			.twobyte_threshold_min = 848,
			.seqlenMinLimit3 = 151,
			
			.blockSizeMinus = 121,
			.winsize = 2000,
			.sizeMaxForCanonicalHeaderPack = 1,
			.sizeMinForSeqPack = 10,
			.sizeMinForCanonical = 100000,
			.sizeMaxForSuperslim = 16384
};

/*
Half byte RLE pack = > bit 7 = 0   
kind 0 = > bit 1 = 1   bit 2 = 1
kind 1 = > bit 1 = 0   bit 2 = 1
kind 2 = > bit 1 = 1   bit 2 = 0 
*/
int packTypeForHalfbyteRlePack(int kind) {	
	if (kind == 0) {
		return getKindBits(1, 1);		
	} 
	if (kind == 1) {
		return getKindBits(0, 1);
	}
	if (kind == 2) {
		return getKindBits(1, 0);
	}
	printf("\n wrong kind in multi_packer.c %d", kind); exit(1);
}

int getKindBits(int bit1, int bit2) {
	int pt = setKthBitToVal(0, 1, bit1);
	return setKthBitToVal(pt, 2, bit2);
}

int getHalfbyteKindFromPackType(int packType) {
	if (isKthBitSet(packType, 1)) {
		return isKthBitSet(packType, 2) ? 0 : 2;
	}
	return 1;
}


bool isHalfByteRlePacked(int packType) {
	if (isKthBitSet(packType, 7) ||
		((!isKthBitSet(packType, 1) && !isKthBitSet(packType, 2)))) {
		return false;
	}
	return true;
}


int packTypeRlePlusTwobyte() {
	int pt = setKthBit(0, RLE_BIT);
	pt = setKthBit(pt, TWOBYTE_BIT);
	return pt;
}

bool belowRatio(uint64_t packedSize, uint64_t orgSize, int ratio) {
	double packed_ratio = ((double)packedSize / (double)orgSize) * 100.0;
	return (packed_ratio < (double)ratio);
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

 Half byte RLE pack =>  bit 7 = 0   kind 0 =>  bit 1 = 1   bit 2 = 1
                                       kind 1 =>  bit 1 = 0   bit 2 = 1
									   kind 2 =>  bit 1 = 1   bit 2 = 0
 */

uint8_t multiPackInternal(memfile* src, memfile* dst, completePackProfile comp, bool storePackType) {


	packProfile profile = comp.main;
	static int metacount = 101;
	rewindMem(src);
	rewindMem(dst);
	
	packCandidate_t bestCandidate;
	bestCandidate.filename = NULL;
	bestCandidate.size = UINT64_MAX;
	getPackCandidate(&bestCandidate, src, 0);

	//printProfile(&profile);
	uint64_t source_size = getMemSize(src);
	printf("\n --------- Multi pack started of file sized %lld -------------", source_size);

	if (source_size > BLOCK_SIZE) {
		printf("\n Can't multiPack file because it is larger than BLOCK_SIZE size was: %lld", source_size);
		myExit();
	}

	uint8_t pack_type = 0;

	bool canonicalHeaderCase = (profile.sizeMinForCanonical == INT64_MAX);
	
	
	memfile* before_seqpack = getMemfile(source_size, L"multipacker.before_seqpack");
	
	uint8_t slimPackType = 0;
	seqPackBundle mb;
	mb.main = NULL;
	mb.seqlens = NULL;
	mb.distances = NULL;
	mb.offsets = NULL;
	if (source_size >= 9) {

	
		if (source_size < profile.sizeMaxForCanonicalHeaderPack) {
			for (int i = 0; i < 3; i++) {
				memfile* head_pack = halfbyteRlePack(src, i);
				assert(getMemSize(head_pack) > 0, "getMemSize(head_pack) > 0 failed in multipacker!");
				int pt = packTypeForHalfbyteRlePack(i);
				getPackCandidate3AndFree(&bestCandidate, head_pack, pt, canonicalHeaderCase);
			}
		}

		if (source_size > 20 && profile.rle_ratio > 0) {
			memfile* just_two_byte = twoBytePack(src, twobyte100Profile);
			getPackCandidateAndFree(&bestCandidate, just_two_byte, 0b1000000);
		}

		if (source_size > profile.sizeMinForCanonical && profile.rle_ratio > 0) {
			memfile* just_canonical = canonicalEncode(src);
			getPackCandidateAndFree(&bestCandidate, just_canonical, 1);
		}

		memfile* before_seqpack = RleSimplePack(src);
		uint64_t sizeAfterRle = getMemSize(before_seqpack);		
		if (belowRatio(sizeAfterRle, source_size, profile.rle_ratio)) {
			pack_type = setKthBit(pack_type, RLE_BIT);
		}
		else {
			freeMem(before_seqpack);
			before_seqpack = getMemfile(source_size, L"multipacker_beforeseqpack");
			deepCopyMem(src, before_seqpack);
		}

		assert(getMemSize(before_seqpack) > 0, "before_seqpack size was 0 in multi_packer.c");
		
		
		if (source_size > profile.sizeMinForSeqPack  && profile.rle_ratio > 0) {
			memfile* afterTwoByte = twoBytePack(before_seqpack, profile);
			uint64_t sizeAfterTwoByte= getMemSize(afterTwoByte);			
			if (belowRatio(sizeAfterTwoByte, getMemSize(before_seqpack), profile.twobyte_ratio)) {
				if (bestCandidate.filename != before_seqpack) {
					freeMem(before_seqpack);
				}
				before_seqpack = afterTwoByte;
				pack_type = setKthBit(pack_type, TWOBYTE_BIT);
			}
			else {
				freeMem(afterTwoByte);
			}
			
			//pack_type = packAndTest2(L"twobyte", before_seqpack, profile, pack_type, TWOBYTE_BIT);			
		}


		getPackCandidate3(&bestCandidate, before_seqpack, pack_type, canonicalHeaderCase);
		uint64_t before_seqpack_size = getMemSize(before_seqpack);

		if (source_size > profile.sizeMinForCanonical) {
			memfile* canonical_instead_of_seqpack = canonicalEncode(before_seqpack);
			getPackCandidateAndFree(&bestCandidate, canonical_instead_of_seqpack, setKthBit(pack_type, 0));
		}
	
		uint64_t size_after_seq = UINT64_MAX;
		if (source_size > profile.sizeMinForSeqPack) {
			//printf("\n now trying seqPack of file w size %d", getMemSize(before_seqpack));
			 mb = seqPackSep(before_seqpack, profile);

			//try to pack meta files!
			uint64_t seqlens_size = getMemSize(mb.seqlens);
			uint64_t offsets_size = getMemSize(mb.offsets);
			uint64_t distances_size = getMemSize(mb.distances);

			// ---------- Pack the meta files (seqlens/offsets) recursively
			if (seqlens_size > profile.recursive_limit) {
			
				pack_type = MultiPackAndTest(mb.seqlens, comp, pack_type, 1);
				
			}
			if (offsets_size > profile.recursive_limit) {

				pack_type = MultiPackAndTest(mb.offsets, comp, pack_type, 2);				
			}
			if (distances_size > profile.recursive_limit) {

				pack_type = MultiPackAndTest(mb.distances, comp, pack_type, 3);				
			}
			seqlens_size = getMemSize(mb.seqlens);
			offsets_size = getMemSize(mb.offsets);
			distances_size = getMemSize(mb.distances);
			/*

			char tmp7[100] = { 0 };
			concat_int(tmp7, "c:/test/meta/offsets", metacount);
			cp(offsets_name, tmp7);
			concat_int(tmp7, "c:/test/meta/seqlens", metacount);
			cp(seqlens_name, tmp7);
			concat_int(tmp7, "c:/test/meta/distances", metacount);
			cp(distances_name, tmp7);
			metacount++;
			*/

			uint64_t meta_size = offsets_size +
				seqlens_size + distances_size + 9;
			uint64_t size_after_seq = getMemSize(mb.main) + meta_size;

			if (size_after_seq < before_seqpack_size) {
				//printf("\n Normal seqpack worked with ratio %.3f", (double)size_after_seq / (double)before_seqpack_size);
				pack_type = setKthBit(pack_type, 7);
				if (bestCandidate.filename != before_seqpack) {
					freeMem(before_seqpack);				
				}
				before_seqpack = NULL;
				if (getMemSize(mb.main) > profile.sizeMinForCanonical) {
					uint64_t size_before_canonical = getMemSize(mb.main);
					memfile* canonicalled = canonicalEncode(mb.main);
					uint64_t size_after_canonical = getMemSize(canonicalled);
					if (size_after_canonical < size_before_canonical) {
						pack_type = setKthBit(pack_type, 0);
						freeMem(mb.main);
						mb.main = canonicalled;
					}
					else {
						freeMem(canonicalled);
					}
				}
				getPackCandidate2(&bestCandidate, mb.main, pack_type, getMemSize(mb.main) + meta_size);
			}
			else {
				//seqpack not used so clear metafile pack bits
				pack_type = clearKthBit(pack_type, 1);
				pack_type = clearKthBit(pack_type, 2);	
			}

		}
		else {
			//to small for seqpack		
			freeMem(mb.main);
			mb.main = before_seqpack;
		}
	}
	printf("\nWinner is %ls packtype %d  packed %lld > %llu", getMemName(bestCandidate.filename), bestCandidate.packType, source_size, bestCandidate.size);
	pack_type = bestCandidate.packType;	
	if (bestCandidate.filename != mb.main) {
		freeMem(mb.main);
		mb.main = bestCandidate.filename;
	}
	//printf("\nTar writing %s packtype %d", dst, pack_type);
	pack_type = tar(dst, mb, pack_type, storePackType);

	if (mb.main != before_seqpack && before_seqpack != src) {
		freeMem(before_seqpack);
	}

	if (mb.main != src) {
		freeMem(mb.main);
	}
	freeMem(mb.seqlens);
	freeMem(mb.offsets);
	freeMem(mb.distances);

	//printf("\n ---------------  returning multipack -----------------");
	return pack_type;
}

memfile* multiUnpackAndReplaceWithPackType(memfile* src, uint8_t packType) {
	memfile* res = multiUnpackWithPackType(src, packType);
	freeMem(src);
	return res;
}

memfile* multiUnpackAndReplace(memfile* src) {
	memfile* res = multiUnpack(src);
	freeMem(src);
	return res;
}

// ------------------------------------------------------------------

//TODO: instead have pack_type = -1 signaling to read pack type from file
memfile* multiUnpackInternal(memfile* in, uint8_t pack_type, bool readPackTypeFromFile) {
	rewindMem(in);
	memfile* dst;
	
	if (readPackTypeFromFile) {
		pack_type = fgetcc(in);
	}
	if (pack_type == 0) {
		printf("\n  UNSTORE!!  ");
		dst = getMemfile(getMemSize(in), L"multiunpack_dst");
		copy_the_rest_mem(in, dst);
		return dst;
	}
	wprintf(L"\n Multi unpack of %s with size %d and packtype %d", getMemName(in), getMemSize(in), pack_type);
	seqPackBundle mb = untar(in, pack_type);
	
	if (isKthBitSet(pack_type, 0)) { //main was huffman coded
		mb.main = unpackAndReplace2(L"canonical", mb.main); 
	}
	bool seqPacked = isKthBitSet(pack_type, 7);
	if (seqPacked) {
		if (isKthBitSet(pack_type, 1)) {
			mb.seqlens = multiUnpackAndReplace(mb.seqlens);
		}
		if (isKthBitSet(pack_type, 2)) {
			mb.offsets = multiUnpackAndReplace(mb.offsets);
		}
		if (isKthBitSet(pack_type, 3)) {
			mb.distances = multiUnpackAndReplace(mb.distances);
		}
	}
	memfile* seq_dst;
	if (seqPacked) {		
		seq_dst = seqUnpack(mb);				
		freBundle(mb);		
	}
	else {
		seq_dst = mb.main;		
	}
	if (isKthBitSet(pack_type, TWOBYTE_BIT)) {
		seq_dst = unpackAndReplace2(L"twobyte", seq_dst);
	}
	if (isHalfByteRlePacked(pack_type)) {
		
		 dst = halfbyteRleUnpack(seq_dst, getHalfbyteKindFromPackType(pack_type));
	}
	else {
		if (isKthBitSet(pack_type, RLE_BIT)) {  // bit 5
			 dst = RleSimpleUnpack(seq_dst);			
		}
		else {		
			dst = seq_dst;	
		}
	}
	if (dst != seq_dst) {
		freeMem(seq_dst);
	}
	return dst;
}

uint8_t multiPackFiles(const wchar_t* src, const wchar_t* dst, completePackProfile profile) {	

	memfile* srcm = getMemfileFromFile(src);
	memfile* dstm = getEmptyMem(L"multipackfiles_dstm");

	uint8_t pt = multiPackInternal(srcm, dstm, profile, false);

	freeMem(srcm);
	memfileToFile(dstm, dst);
	freeMem(dstm);
	return pt;
}

uint8_t multiPackAndReturnPackType(memfile* src, memfile* dst, completePackProfile profile) {
	return multiPackInternal(src, dst, profile, false);

}

memfile* multiPackAndStorePackType(memfile* src, completePackProfile profile) {
	memfile* dst = getEmptyMem(L"multipacker.multipackstorepacktype");
	multiPackInternal(src, dst, profile, true);
	return dst;
}

memfile* multiUnpackWithPackType(memfile* m, uint8_t pack_type) {
	return multiUnpackInternal(m, pack_type, false);
}

void multi_unpackw(const wchar_t* srcw, const wchar_t* dstw) {
	memfile* srcm = getMemfileFromFile(srcw);	
	memfile* dstm = multiUnpackInternal(srcm, 0, true);
	memfileToFile(dstm, dstw);
	freeMem(srcm);
	freeMem(dstm);
}

void multi_packw(const wchar_t* srcw, const wchar_t* dstw, completePackProfile profile) {

	memfile* srcm = getMemfileFromFile(srcw);
	memfile* dstm = getEmptyMem(L"multi_packw_dstm");
	
	multiPackInternal(srcm, dstm, profile, true);
	memfileToFile(dstm, dstw);
	freeMem(srcm);
	freeMem(dstm);
}

memfile* multiPack(memfile* src, completePackProfile profile) {
	memfile* dst = getEmptyMem(L"multipack2_dst");
	multiPackInternal(src, dst, profile, true);
	return dst;
}

memfile* multiUnpack(memfile* m) {
	return multiUnpackInternal(m, 0, true);
}

memfile* multiUnpackBlock(FILE* in, uint64_t bytesToRead) {
	memfile* src = getEmptyMem(L"multiunpackblock_src");
	copy_chunk_to_mem(in, src, bytesToRead);
	return multiUnpack(src);
}

void multiUnpackBlockToFile(FILE* in, wchar_t* dstFilename, uint64_t bytesToRead) {
	memfile* dstMem = multiUnpackBlock(in, bytesToRead);
	FILE* dstFile = openWrite(dstFilename);
	append_mem_to_file(dstFile, dstMem);
	fclose(dstFile);
}
