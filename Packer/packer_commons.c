#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include "common_tools.h"
#include "packer_commons.h"
#include "memfile.h"
#include "Two_byte_packer.h"
#include "RLE_simple_packer.h"
#include "canonical.h"
#include "multi_packer.h"


void lockTempfileMutex() {

}

void releaseTempfileMutex() {

}


bool testPack(memfile* src, memfile* tmp, const wchar_t* kind, int limit) {
	wprintf(L"\n testPack of %s to %s with kind %s", getMemName(src), getMemName(tmp), kind);
	uint64_t size_org = getMemSize(src);
	uint64_t size_packed = getMemSize(tmp);
	double packed_ratio = ((double)size_packed / (double)size_org) * 100.0;
	wprintf(L"\n %s packed got ratio %.1f (limit %d)", kind, packed_ratio, limit);
	return packed_ratio < (double)limit;
}

void doDoubleCheck2(memfile* src, memfile* packedName, const wchar_t* kind) {
	if (DOUBLE_CHECK_PACK) {		
		wprintf(L"\n ?Double check of %s pack of %s", kind, getMemName(src));
		memfile* tmp = unpackByKind(kind, packedName);
		doDoubleCheck(tmp, src, kind);
	}
	//has to fre src 
	deepCopyMem(packedName, src);
}

void doDoubleCheck(memfile* tmp, memfile* src, const wchar_t* kind) {
	bool sc = memsEqual(tmp, src);
	if (!sc) {
		wprintf(L"\n\n\n ** Failed to %s pack: %s", kind, getMemName(src));
		const wchar_t filename[100] = { 0 };
		concatw(filename, L"c:/test/temp_files/", getMemName(src));
		memfileToFile(src, filename);
		exit(1);
	}
	fre(tmp);
}

memfile* unpackByKind(const wchar_t* kind, memfile* packedFilename) {
	if (equalsw(kind, L"multi")) {
		return multiUnpack2(packedFilename);
	}
	if (equalsw(kind, L"rle simple")) {
		return RleSimpleUnpack(packedFilename);
	}
	else if (equalsw(kind, L"twobyte")) {
		return twoByteUnpack(packedFilename);
	}	
	else if (equalsw(kind, L"canonical")) {
		return CanonicalDecodeMem(packedFilename);
	}
	else {
		wprintf(L"\n kind=%s not found in packer_commons.unpackByKind", kind);
		exit(1);
	}
}


bool packAndTest(const wchar_t* kind, memfile* src, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile, packProfile distancesProfile) {
	
	wprintf(L"\n PackAndTest %s with kind=%s src.size=%d", getMemName(src), kind, getMemSize(src));

	memfile* packedName = NULL;
	
	int limit = 100;
	
	if (equalsw(kind, L"multi")) {
		packedName = multiPackAndStorePackType(src, profile, seqlensProfile, offsetsProfile, distancesProfile);		
	}
	else if (equalsw(kind, L"rle simple")) {
		packedName = RleSimplePack(src, profile);
		limit = profile.rle_ratio;
	}
	else if (equalsw(kind, L"twobyte")) {
		packedName = twoBytePack(src, profile);
		limit = profile.twobyte_ratio;
	}	
	else {
		wprintf(L"\n kind=%s not found in packer_commons.packAndTest", kind);
		exit(1);
	}

	bool under_limit = testPack(src, packedName, kind, limit);
	if (under_limit) {
		doDoubleCheck2(src, packedName, kind);
	}
	else {
		fre(packedName);
	}
	return under_limit;
}

bool MultiPackAndTest(memfile* src, packProfile profile, packProfile seqlenProfile, packProfile offsetProfile, 
	packProfile distancesProfile) {
	return packAndTest(L"multi", src, profile, seqlenProfile, offsetProfile, distancesProfile);
}

void printProfile(packProfile* profile) {
	printf("\nRLE ratio         %d", profile->rle_ratio);
	printf("\nTwobyte ratio     %d", profile->twobyte_ratio);
	printf("\nRecursive limit   %d", profile->recursive_limit);
	printf("\nTwobyte threshold (max,divide,min): (%d %d %d)", profile->twobyte_threshold_max, profile->twobyte_threshold_divide, profile->twobyte_threshold_min);
	printf("\nSeqlenMin limit3 %d", profile->seqlenMinLimit3);	
	printf("\nSeqlenMin limit4 %d", profile->seqlenMinLimit4);
	printf("\nBlock size minus (10k): %d", profile->blockSizeMinus);
	printf("\nWinsize: %d", profile->winsize);
	printf("\nSize max for Canonical Header Pack %d", profile->sizeMaxForCanonicalHeaderPack);
	printf("\nSize min for seqpack %d", profile->sizeMinForSeqPack);
	printf("\nSize min for canonical %d", profile->sizeMinForCanonical);
	printf("\nSize max for superslim %d", profile->sizeMaxForSuperslim);
}

packProfile getPackProfile() {
	packProfile profile = {
	.rle_ratio = 84,
	.twobyte_ratio = 89,
	.recursive_limit = 15,
	.twobyte_threshold_max = 10581,
	.twobyte_threshold_divide = 27,
	.twobyte_threshold_min = 3150,
	.seqlenMinLimit3 = 128,
	.seqlenMinLimit4 = 57360,
	.blockSizeMinus = 139,
	.winsize = 95536,
	.sizeMaxForCanonicalHeaderPack = 256,
	.sizeMinForSeqPack = 300,
	.sizeMinForCanonical = 40,
	.sizeMaxForSuperslim = 16384
	};
	return profile;
}

void copyProfile(packProfile* src, packProfile* dst) {
	
	dst->rle_ratio = src->rle_ratio;
	dst->twobyte_ratio = src->twobyte_ratio;
	dst->recursive_limit = src->recursive_limit;
	dst->twobyte_threshold_max = src->twobyte_threshold_max;
	dst->twobyte_threshold_divide = src->twobyte_threshold_divide;
	dst->twobyte_threshold_min = src->twobyte_threshold_min;
	dst->seqlenMinLimit3 = src->seqlenMinLimit3;
	dst->seqlenMinLimit4 = src->seqlenMinLimit4;
	dst->blockSizeMinus = src->blockSizeMinus;
	dst->winsize = src->winsize;
	dst->sizeMaxForCanonicalHeaderPack = src->sizeMaxForCanonicalHeaderPack;
	dst->sizeMinForSeqPack = src->sizeMinForSeqPack;
	dst->sizeMinForCanonical = src->sizeMinForCanonical;
	dst->sizeMaxForSuperslim = src->sizeMaxForSuperslim;
}

value_freq_t find_best_code(unsigned long* char_freq) {
	unsigned char best_code = 0;
	unsigned long freq = char_freq[0];	
	for (unsigned int i = 1; i < 256; i++) {
		if (char_freq[i] < freq) {
			freq = char_freq[i];
			best_code = i;
		}
	}

	//printf("\n Found code: %d that occured: %d times.", best_code, freq);

	char_freq[best_code] = ULONG_MAX; // mark it as used!
	value_freq_t res;
	res.value = best_code;
	res.freq = freq;
	return res;
}

static int compareEndings(const wchar_t* s1, const wchar_t* s2) {

	int res;
	wchar_t ext1[500] = { 0 };
	wchar_t ext2[500] = { 0 };

	substringAfterLast(ext1, s1, L".");
	substringAfterLast(ext2, s2, L".");

	if (equalsw(ext1, L"txt")) {
		wcscpy(ext1, "zzz");
	}
	if (equalsw(ext2, L"txt")) {
		wcscpy(ext2, "zzz");
	}
	if (equalsw(ext1, L"htm")) {
		wcscpy(ext1, "zzy");
	}
	if (equalsw(ext2, L"htm")) {
		wcscpy(ext2, "zzy");
	}
	if (equalsw(ext1, L"html")) {
		wcscpy(ext1, "zzx");
	}
	if (equalsw(ext2, L"html")) {
		wcscpy(ext2, "zzx");
	}

	if (equalsw(ext1, L"wav")) {
		wcscpy(ext1, "aab");
	}
	if (equalsw(ext2, L"wav")) {
		wcscpy(ext2, "aab");
	}

	if (equalsw(ext1, L"exe")) {
		wcscpy(ext1, "aaa");
	}
	if (equalsw(ext2, L"exe")) {
		wcscpy(ext2, "aaa");
	}



	if (wcslen(ext1) + wcslen(ext2) > 0) {

		res = wcscmp(ext1, ext2);
		if (res) {
			return -res;
		}
	}
	uint64_t size1 = get_file_size_from_wname(s1);
	uint64_t size2 = get_file_size_from_wname(s2);
	if (size1 < size2) {
		return 1;
	}
	if (size1 > size2) {
		return -1;
	}
	return 0;
}

void swap(file_t* number, uint32_t i, uint32_t j) {
	file_t temp = number[i];
	number[i] = number[j];
	number[j] = temp;
}

typedef int (*compareFuncType)();

void quicksort_internal(file_t* A, int32_t lo, int32_t hi, compareFuncType cmp) {	
	if (lo < hi) {
		int32_t p = partition(A, lo, hi, cmp);
		quicksort_internal(A, lo, p - 1, cmp);
		quicksort_internal(A, p + 1, hi, cmp);
	}
}
		
int32_t partition(file_t* A, int32_t lo, int32_t hi, compareFuncType cmp) {
	file_t pivot = A[hi];
	int32_t p = lo;
	for (int32_t j = lo; j <= hi; j++) {
		if ((*cmp)(A[j].name, pivot.name) < 0) {
			swap(A, p, j);
			p++;
		}
	}
	swap(A, p, hi);
	return p;
}

int simpleCompare(wchar_t* s1, wchar_t* s2) {
	return wcscmp(s1, s2);
}


void quickSortCompareEndings(file_t* f, uint64_t size)
{
	quicksort_internal(f, 0, size - 1, compareEndings);
}

void quickSort(file_t* f, uint64_t size)
{
	quicksort_internal(f, 0, size - 1, simpleCompare);
}


