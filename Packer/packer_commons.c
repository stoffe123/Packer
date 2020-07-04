#include <stdbool.h>
#include <limits.h>
#include "common_tools.h"
#include "packer_commons.h"


bool testPack(const char* src, const char* tmp, const char* packerName, int limit) {
	uint64_t size_org = get_file_size_from_name(src);
	uint64_t size_packed = get_file_size_from_name(tmp);
	double packed_ratio = ((double)size_packed / (double)size_org) * 100.0;
	printf("\n %s packed %s  got ratio %.1f (limit %d)", packerName, src, packed_ratio, limit);
	return packed_ratio < (double)limit;
}

void doDoubleCheck2(const char* src, const char* packedName, const char* kind) {
	if (DOUBLE_CHECK_PACK) {
		const char tmp[100] = { 0 };
		getTempFile(tmp, kind);
		unpackByKind(kind, packedName, tmp);
		doDoubleCheck(tmp, src, kind);
	}
	my_rename(packedName, src);
}

void doDoubleCheck(const char* tmp, const char* src, const char* type) {
	bool sc = files_equal(tmp, src);
	if (!sc) {
		printf("\n\n\n ** Failed to %s pack: %s", type, src);
		exit(1);
	}
	remove(tmp);
}

uint64_t CanonicalEncodeAndTest(const char* src) {
	const char packedName[100] = { 0 };
	getTempFile(packedName, "multi_canonicaled");
	CanonicalEncode(src, packedName);
	uint64_t size_org = get_file_size_from_name(src);
	uint64_t size_packed = get_file_size_from_name(packedName);
	//printf("\n CanonicalEncode:%s  (%f)", src, (double)size_packed / (double)size_org);
	bool compression_success = (size_packed < size_org);
	if (compression_success) {
		
		doDoubleCheck2(src, packedName, "canonical");
	}
	else {
		remove(packedName);
	}
	return size_packed;
}

void unpackByKind(const char* kind, const char* tmp, const char* tmp2) {
	if (equals(kind, "multi")) {
		multi_unpack(tmp, tmp2);
	}
	else if (equals(kind, "rle simple")) {
		RLE_simple_unpack(tmp, tmp2);
	}
	else if (equals(kind, "twobyte")) {
		two_byte_unpack(tmp, tmp2);
	}
	else if (equals(kind, "rle advanced")) {
		RLE_advanced_unpack(tmp, tmp2);
	}
	else if (equals(kind, "canonical")) {
		CanonicalDecode(tmp, tmp2);
	}
	else {
		printf("\n kind=%s not found in packer_commons.unpackByKind", kind);
		exit(1);
	}
}


bool packAndTest(const char* kind, const char* src, packProfile profile,
	packProfile seqlensProfile, packProfile offsetsProfile, packProfile distancesProfile) {

	const char packedName[100] = { 0 };
	const char tmp[100] = { 0 };
	concat(tmp, "multi_", kind);
	int limit = 100;
	getTempFile(packedName, tmp);
	if (equals(kind, "multi")) {
		multi_pack(src, packedName, profile, seqlensProfile, offsetsProfile, distancesProfile);
	} 
	else if (equals(kind, "rle simple")) {
		RLE_simple_pack(src, packedName, profile);
		limit = profile.rle_ratio;
	}
	else if (equals(kind, "twobyte")) {
		two_byte_pack(src, packedName, profile);
		limit = profile.twobyte_ratio;
	}
	else if (equals(kind, "rle advanced")) {
		RLE_advanced_pack(src, packedName, profile);
	}
	else {
		printf("\n kind=%s not found in packer_commons.packAndTest", kind);
		exit(1);
	}

	bool under_limit = testPack(src, packedName, kind, limit);
	if (under_limit) {
		doDoubleCheck2(src, packedName, kind);
	}
	else {
		remove(packedName);
	}
	return under_limit;
}

bool MultiPackAndTest(const char* src, packProfile profile, packProfile seqlenProfile, packProfile offsetProfile, 
	packProfile distancesProfile) {
	return packAndTest("multi", src, profile, seqlenProfile, offsetProfile, distancesProfile);
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
	char ext1[500] = { 0 };
	char ext2[500] = { 0 };

	substringAfterLast(ext1, s1, L".");
	substringAfterLast(ext2, s2, L".");

	if (equalsw(ext1, L"txt")) {
		strcpy(ext1, "zzz");
	}
	if (equalsw(ext2, L"txt")) {
		strcpy(ext2, "zzz");
	}
	if (equalsw(ext1, L"htm")) {
		strcpy(ext1, "zzy");
	}
	if (equalsw(ext2, L"htm")) {
		strcpy(ext2, "zzy");
	}
	if (equalsw(ext1, L"html")) {
		strcpy(ext1, "zzx");
	}
	if (equalsw(ext2, L"html")) {
		strcpy(ext2, "zzx");
	}

	if (equalsw(ext1, L"wav")) {
		strcpy(ext1, "aab");
	}
	if (equalsw(ext2, L"wav")) {
		strcpy(ext2, "aab");
	}

	if (equalsw(ext1, L"exe")) {
		strcpy(ext1, "aaa");
	}
	if (equalsw(ext2, L"exe")) {
		strcpy(ext2, "aaa");
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

void quicksort(file_t* number, int32_t first, int32_t last) {
	int32_t i, j, pivot;
	file_t temp;

	if (first < last) {
		pivot = first;
		i = first;
		j = last;

		while (i < j) {
			while (compareEndings(number[i].name, number[pivot].name) <= 0 && i < last)
				i++;
			while (compareEndings(number[j].name, number[pivot].name) > 0)
				j--;
			if (i < j) {
				temp = number[i];
				number[i] = number[j];
				number[j] = temp;
			}
		}

		temp = number[pivot];
		number[pivot] = number[j];
		number[j] = temp;
		quicksort(number, first, j - 1);
		quicksort(number, j + 1, last);

	}
}

void bubbleSort(file_t* f, uint64_t size)
{
	quicksort(f, 0, size - 1);
}


