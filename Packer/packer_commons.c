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
	//wprintf(L"\n testPack of %s to %s with kind %s", getMemName(src), getMemName(tmp), kind);
	uint64_t size_org = getMemSize(src);
	uint64_t size_packed = getMemSize(tmp);
	double packed_ratio = ((double)size_packed / (double)size_org) * 100.0;
	wprintf(L"\n %s packed got ratio %.1f (limit %d)", kind, packed_ratio, limit);
	return packed_ratio < (double)limit;
}

void doubleCheckPack(memfile* src, memfile* packedMem, const wchar_t* kind, packProfile profile) {
	if (DOUBLE_CHECK_PACK) {		
		wprintf(L"\n ?Double check of %s pack of %s", kind, getMemName(src));
		memfile* tmp = unpackByKind(kind, packedMem, profile);
		doDoubleCheck(tmp, src, kind);
	}
	//has to fre src 
	deepCopyMem(packedMem, src);
	freeMem(packedMem);
}

void doDoubleCheck(memfile* unpackedMem, memfile* sourceMem, const wchar_t* kind) {
	bool sc = memsEqual(unpackedMem, sourceMem);
	if (!sc) {
		printf("\n\n\n !!!!! Failed to %ls pack: %ls", kind, getMemName(sourceMem));
		const wchar_t filename[300] = { 0 };
		concatw(filename, L"c:/test/temp_files/", getMemName(sourceMem));
		memfileToFile(sourceMem, filename);	
		concatw(filename, L"c:/test/temp_files/", getMemName(unpackedMem));
		memfileToFile(unpackedMem, filename);
		exit(1);
	}
	freeMem(unpackedMem);
}

memfile* unpackByKind(const wchar_t* kind, memfile* packedFilename, packProfile profile) {
	if (equalsw(kind, L"multi")) {
		return multiUnpack(packedFilename, profile);
	}
	if (equalsw(kind, L"rle simple")) {
		return RleSimpleUnpack(packedFilename);
	}
	else if (equalsw(kind, L"twobyte")) {
		return twoByteUnpack(packedFilename);
	}	
	else if (equalsw(kind, L"canonical")) {		
		return canonicalDecode(packedFilename);
	}
	else {
		wprintf(L"\n kind=%s not found in packer_commons.unpackByKind", kind);
		exit(1);
	}
}


bool packAndTest(const wchar_t* kind, memfile* src, completePackProfile comp) {
	
	//wprintf(L"\n PackAndTest %s with kind=%s src.size=%d", getMemName(src), kind, getMemSize(src));

	memfile* packedMem = NULL;
	
	int limit = 100;
	
	if (equalsw(kind, L"multi")) {	
		packedMem = multiPackAndStorePackType(src, comp);
	}
	else if (equalsw(kind, L"rle simple")) {
		packedMem = RleSimplePack(src, comp.main);
		limit = comp.main.rle_ratio;
	}
	else if (equalsw(kind, L"twobyte")) {
		packedMem = twoBytePack(src, comp.main);
		limit = comp.main.twobyte_ratio;
	}
	else {
		wprintf(L"\n kind=%s not found in packer_commons.packAndTest", kind);
		exit(1);
	}

	bool under_limit = testPack(src, packedMem, kind, limit);
	if (under_limit) {
		doubleCheckPack(src, packedMem, kind, comp.main);
	}
	else {
		freeMem(packedMem);
	}
	return under_limit;
}

int MultiPackAndTest(memfile* src, completePackProfile comp, int packType, int bit) {	
	bool succ = packAndTest(L"multi", src, comp);
	if (succ) {
		return setKthBit(packType, bit);
	}
	return packType;
}




char* profileToString(packProfile* profile) {
	char msg[4096];
	sprintf(msg, "\n.rle_ratio = %lld,\n"
		                 ".twobyte_ratio = %lld,\n"
		                 ".recursive_limit = %lld,"
	"\n.twobyte_threshold_max = %lld,"
	"\n.twobyte_threshold_divide = %lld,"
	"\n.twobyte_threshold_min = %lld,"
	"\n.seqlenMinLimit3 = %lld,"
	"\n.seqlenMinLimit4 = %lld,"
	"\n.blockSizeMinus = %lld,"	
	"\n.sizeMaxForCanonicalHeaderPack = %lld,"
	"\n.sizeMinForSeqPack = %lld,"
	"\n.sizeMinForCanonical = %lld,"
	"\n.sizeMaxForSuperslim = %lld,"	
	"\n.metaCompressionFactor = %lld,"
	"\n.offsetLimit1 = %lld,"
	"\n.offsetLimit2 = %lld,"
	"\n.offsetLimit3 = %lld,"
	"\n.bytesWonMin = %lld\n"
	 , profile->rle_ratio, profile->twobyte_ratio, profile->recursive_limit, profile->twobyte_threshold_max,
		 profile->twobyte_threshold_divide, profile->twobyte_threshold_min, 
		profile->seqlenMinLimit3,
		profile->seqlenMinLimit4,
		profile->blockSizeMinus,
		 profile->sizeMaxForCanonicalHeaderPack, profile->sizeMinForSeqPack, profile->sizeMinForCanonical, 
		 profile->sizeMaxForSuperslim, profile->metaCompressionFactor, 
		profile->offsetLimit1,
		profile->offsetLimit2,
		profile->offsetLimit3,
		profile->bytesWonMin);
	return msg;
}

void printProfile(packProfile* profile) {
	char* msg = profileToString(profile);
	printf(msg);	
}

void fprintProfile(FILE* file, packProfile* profile) {
	char* msg = profileToString(profile);
	fprintf(file, msg);	
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
	.seqlenMinLimit4 = 65360,
	.blockSizeMinus = 139,
	.winsize = 95536,
	.sizeMaxForCanonicalHeaderPack = 256,
	.sizeMinForSeqPack = 300,
	.sizeMinForCanonical = 40,
	.sizeMaxForSuperslim = 16384,
	.archiveType = 0,
	.metaCompressionFactor = 70,
	.offsetLimit1 = 255,
	.offsetLimit2 = 1018,
	.offsetLimit3 = 66600,
	.bytesWonMin = 40
	};
	return profile;
}

completePackProfile getCompletePackProfile(packProfile main, packProfile seqlen, packProfile offset, packProfile distance) {
	completePackProfile res;
	res.main = main;
	res.seqlen = seqlen;
	res.offset = offset;
	res.distance = distance;
	return res;
}

completePackProfile getCompletePackProfileSimple(packProfile main) {
	return getCompletePackProfile(main, main, main, main);
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
	dst->archiveType = src->archiveType;

	dst->metaCompressionFactor = src->metaCompressionFactor;
	dst->offsetLimit1 = src->offsetLimit1;
	dst->offsetLimit2 = src->offsetLimit2;
	dst->offsetLimit3 = src->offsetLimit3;
	dst->bytesWonMin = src->bytesWonMin;
}

completePackProfile cloneCompleteProfile(completePackProfile src) {
	completePackProfile dst;
	copyProfile(&src.main, &dst.main);
	copyProfile(&src.seqlen, &dst.seqlen);
	copyProfile(&src.offset, &dst.offset);
	copyProfile(&src.distance, &dst.distance);
	return dst;
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

static int compareSizes(const wchar_t* s1, const wchar_t* s2, uint64_t size1, uint64_t size2) {
	if (size1 < size2) {
		return -1;
	}
	if (size1 > size2) {
		return 1;
	}
	return 0;
}

static int compareEndings(const wchar_t* s1, const wchar_t* s2) {

	int res;
	wchar_t ext1[20] = { 0 };
	wchar_t ext2[20] = { 0 };

	getFileExtension(ext1, s1);
	getFileExtension(ext2, s2);

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
	uint64_t size1 = getFileSizeByName(s1);
	uint64_t size2 = getFileSizeByName(s2);
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
		if ((*cmp)(A[j].name, pivot.name, A[j].size, pivot.size) < 0) {
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

void quickSortOnSizes(file_t* f, uint64_t size) {
	quicksort_internal(f, 0, size - 1, compareSizes);
}

void quickSort(file_t* f, uint64_t size)
{
	quicksort_internal(f, 0, size - 1, simpleCompare);
}


void writeDynamicSize16or64(uint64_t s, FILE* out) {
	uint16_t new_s;
	if (s < UINT16_MAX) {
		new_s = (uint16_t)s;
		fwrite(&new_s, sizeof(uint16_t), 1, out);
	}
	else {
		new_s = UINT16_MAX;
		fwrite(&new_s, sizeof(uint16_t), 1, out);
		fwrite(&s, sizeof(uint64_t), 1, out);
	}
}

void writeDynamicSize32or64(uint64_t s, FILE* out) {
	uint32_t new_s;
	if (s < UINT32_MAX) {
		new_s = (uint32_t)s;
		fwrite(&new_s, sizeof(uint32_t), 1, out);
	}
	else {
		new_s = UINT32_MAX;
		fwrite(&new_s, sizeof(uint32_t), 1, out);
		fwrite(&s, sizeof(uint64_t), 1, out);
	}
}

uint64_t readDynamicSize16or64(FILE* in) {
	uint16_t new_s;
	fread(&new_s, sizeof(uint16_t), 1, in);
	if (new_s < 65535) {
		return (uint64_t)new_s;
	}
	uint64_t res;
	fread(&res, sizeof(uint64_t), 1, in);
	return res;
}

uint64_t readDynamicSize32or64(FILE* in) {
	uint32_t new_s;
	fread(&new_s, sizeof(uint32_t), 1, in);
	if (new_s < UINT32_MAX) {
		return (uint64_t)new_s;
	}
	uint64_t res;
	fread(&res, sizeof(uint64_t), 1, in);
	return res;
}



