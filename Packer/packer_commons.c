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
		get_temp_file2(tmp, kind);
		unpackByKind(kind, packedName, tmp);
		doDoubleCheck(tmp, src, kind);
	}
	my_rename(packedName, src);
}

void doDoubleCheck(const char* tmp, const char* src, const char* type) {
	bool sc = files_equal(tmp, src);
	if (!sc) {
		printf("\n\n\n ** Failed to %s pack seperate: %s", type, src);
		exit(1);
	}
	remove(tmp);
}

uint64_t CanonicalEncodeAndTest(const char* src) {
	const char packedName[100] = { 0 };
	get_temp_file2(packedName, "multi_canonicaled");
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
	packProfile seqlensProfile, packProfile offsetsProfile) {

	const char packedName[100] = { 0 };
	const char tmp[100] = { 0 };
	concat(tmp, "multi_", kind);
	int limit = 100;
	get_temp_file2(packedName, tmp);
	if (equals(kind, "multi")) {
		multi_pack(src, packedName, profile, seqlensProfile, offsetsProfile);
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

bool MultiPackAndTest(const char* src, packProfile profile, packProfile seqlenProfile, packProfile offsetProfile) {	
	return packAndTest("multi", src, profile, seqlenProfile, offsetProfile);
}

void printProfile(packProfile* profile) {
	printf("\n");
	printf("\nPages:       (%d, %d)", profile->offset_pages, profile->seqlen_pages);
	printf("\nRLE ratio:         %d", profile->rle_ratio);
	printf("\nTwobyte ratio:     %d", profile->twobyte_ratio);
	printf("\nRecursive limit:   %d", profile->recursive_limit);
	printf("\nTwobyte threshold (max,divide,min): (%d, %d, %d)", 
	profile->twobyte_threshold_max, profile->twobyte_threshold_divide, profile->twobyte_threshold_min);
}

packProfile getPackProfile(int o, int s) {
	packProfile profile;
	profile.offset_pages = o;
	profile.seqlen_pages = s;
	profile.rle_ratio = 84;
	profile.twobyte_ratio = 89;
	profile.seq_ratio = 100;
	profile.recursive_limit = 15;
	profile.twobyte_threshold_max = 10581;
	profile.twobyte_threshold_divide = 27;
	profile.twobyte_threshold_min = 3150;
	return profile;
}

void copyProfile(packProfile* src, packProfile* dst) {
	dst->offset_pages = src->offset_pages;
	dst->seqlen_pages = src->seqlen_pages;
	dst->rle_ratio = src->rle_ratio;
	dst->twobyte_ratio = src->twobyte_ratio;
	dst->seq_ratio = src->seq_ratio;
	dst->recursive_limit = src->recursive_limit;
	dst->twobyte_threshold_max = src->twobyte_threshold_max;
	dst->twobyte_threshold_divide = src->twobyte_threshold_divide;
	dst->twobyte_threshold_min = src->twobyte_threshold_min;
	
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

