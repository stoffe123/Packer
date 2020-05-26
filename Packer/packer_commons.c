#include <stdbool.h>
#include <limits.h>
#include "common_tools.h"
#include "packer_commons.h"


void CanonicalDecodeAndReplace(const char* src) {
	printf("\n Canonical unpacking (in place) %s", src);
	const char tmp[100] = { 0 };
	get_temp_file2(tmp, "multi_canonicaldec");
	CanonicalDecode(src, tmp);
	remove(src);
	rename(tmp, src);
}

bool testPack(const char* src, const char* tmp, const char* packerName, int limit) {
	uint64_t size_org = get_file_size_from_name(src);
	uint64_t size_packed = get_file_size_from_name(tmp);
	double packed_ratio = ((double)size_packed / (double)size_org) * 100.0;
	printf("\n %s packed %s  got ratio %.1f (limit %d)", packerName, src, packed_ratio, limit);
	return packed_ratio < (double)limit;
}

uint64_t CanonicalEncodeAndTest(const char* src) {
	const char tmp[100] = { 0 };
	get_temp_file2(tmp, "multi_canonicaled");
	CanonicalEncode(src, tmp);
	uint64_t size_org = get_file_size_from_name(src);
	uint64_t size_packed = get_file_size_from_name(tmp);
	//printf("\n CanonicalEncode:%s  (%f)", src, (double)size_packed / (double)size_org);
	bool compression_success = (size_packed < size_org);
	if (compression_success) {
		
		if (DOUBLE_CHECK_PACK) {
			//test if compression worked!
			const char tmp2[100] = { 0 };
			get_temp_file2(tmp2, "multi_maksurecanonical");
			CanonicalDecode(tmp, tmp2);
			bool sc = files_equal(tmp2, src);
			if (!sc) {
				printf("\n\n\n ** Failed to canoical pack: %s", src);
				exit(1);
			}
			remove(tmp2);
		}

		remove(src);
		rename(tmp, src);
	}
	else {
		remove(tmp);
	}
	return size_packed;
}


bool SeqPackAndTest(const char* src, int seqlen_pages, int offset_pages, int ratio_limit) {
	const char tmp[100] = { 0 };
	get_temp_file2(tmp, "multi_seqpacked");
	seq_pack(src, tmp, seqlen_pages, offset_pages);	
	bool compression_success = testPack(src, tmp, "Seq", ratio_limit);
	if (compression_success) {
		remove(src);
		rename(tmp, src);
	}
	else {
		remove(tmp);
	}
	return compression_success;
}

bool MultiPackAndTest(const char* src, packProfile_t profile,
	packProfile_t seqlensProfile, packProfile_t offsetsProfile) {
	const char tmp[100] = { 0 };
	get_temp_file2(tmp, "multi_seqpacked");
	multi_pack(src, tmp, profile, seqlensProfile, offsetsProfile);	
	bool compression_success = testPack(src, tmp, "Multi", 100);
	if (compression_success) {

		if (DOUBLE_CHECK_PACK) {
			//test if compression worked!
			const char tmp2[100] = { 0 };
			get_temp_file2(tmp2, "multi_makingsure");
			multi_unpack(tmp, tmp2);
			bool sc = files_equal(tmp2, src);
			if (!sc) {
				printf("\n\n\n ** Failed to multipack: %s", src);
				exit(1);
			}
			remove(tmp2);
		}

		remove(src);
		rename(tmp, src);
	}
	else {
		remove(tmp);
	}
	return compression_success;
}

void printProfile(packProfile_t* profile) {
	printf("\n");
	printf("\nPages:       (%d, %d)", profile->offset_pages, profile->seqlen_pages);
	printf("\nRLE ratio:         %d", profile->rle_ratio);
	printf("\nTwo byte ratio:    %d", profile->twobyte_ratio);
	printf("\nSeq ratio:         %d", profile->seq_ratio);
	printf("\nRecursive limit:   %d", profile->recursive_limit);
	printf("\nTwobyte threshold (max,divide,min): (%d, %d, %d)\n", 
		profile->twobyte_threshold_max, profile->twobyte_threshold_divide, profile->twobyte_threshold_min);
}

packProfile_t getPackProfile(int o, int s) {
	packProfile_t profile;
	profile.offset_pages = o;
	profile.seqlen_pages = s;
	profile.rle_ratio = 84;
	profile.twobyte_ratio = 89;
	profile.seq_ratio = 100;
	profile.recursive_limit = 136;
	profile.twobyte_threshold_max = 10581;
	profile.twobyte_threshold_divide = 27;
	profile.twobyte_threshold_min = 3150;
	return profile;
}

void copyProfile(packProfile_t* src, packProfile_t* dst) {
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
	unsigned char best_code;
	unsigned long freq = ULONG_MAX;
	for (unsigned int i = 0; i < 256; i++) {
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

