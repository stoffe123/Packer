#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common_tools.h"
#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "multi_packer.h"
#include "block_packer.h"
#include <Windows.h>
#include "packer_commons.h"
#include "Two_byte_packer.h"
#include "Two_byte_unpacker.h"
#include "RLE_packer_advanced.h"

//#include "huffman2.h"
//#include "canonical.h"


#define CMP_N 128

void print_string_rep(unsigned char* tt) {
	printf("\n");
	for (int i = 0; i < CMP_N; i++) {
		printf(", ", tt[i]);
	}
}



int fuzz(int i) {
	do {
		if (rand() % 2 == 0) {
			i += (rand() % 4);
		}
		else {
			i -= (rand() % 4);
		}
	} while (!(i >= 0 && i <= 246));
	return i;
}


int taken[1000];
int taken_index;

bool is_taken(int x, int y) {
	for (int i = 0; i < taken_index; i++) {
		if (taken[i] == x + 256 * y) {
			return true;
		}
	}
	return false;
}

void set_taken(int x, int y) {
	taken[taken_index++] = x + 256 * y;
}

void init_taken() {
	for (int i = 0; i < 1000; i++) {
		taken[i] = 0;
	}
	taken_index = 0;
}

int fuzzVal(int r, int v) {
	if (rand() % 2 == 0) {
		r += (rand() % v + 1);
	}
	else {
		r -= (rand() % v + 1);
		if (r < 0) {
			r = (rand() % v + 1);
		}
	}
	return r;
}

int fuzzRatio(int r, int best, int min, int max) {
	r = fuzzVal(r, max / 10);
	if (r < 0 || r > max || r < min || rand() % 6 == 0) {
		r = best;
	}
	return r;
}

void fuzzProfile(packProfile_t* profile, packProfile_t best) {
	if (rand() % 7 == 0) {
		profile->offset_pages = rand() % 252;
		profile->seqlen_pages = rand() % 252;
	}
	else {

		profile->offset_pages = fuzz(best.offset_pages);
		profile->seqlen_pages = fuzz(best.seqlen_pages);

	}
	profile->rle_ratio = fuzzRatio(profile->rle_ratio, best.rle_ratio, 10, 100);
	profile->twobyte_ratio = fuzzRatio(profile->twobyte_ratio, best.twobyte_ratio, 10, 100);
	profile->seq_ratio = fuzzRatio(profile->seq_ratio, best.seq_ratio, 10, 100);
	profile->recursive_limit = fuzzRatio(profile->recursive_limit, best.recursive_limit, 10, 800);
	profile->twobyte_threshold = fuzzRatio(profile->twobyte_threshold, best.twobyte_threshold, 30, 2000);
}





unsigned long long presentResult(bool earlyBreak, int before_suite, unsigned long long acc_size, unsigned long long acc_size_org,
	unsigned long long best_size, packProfile_t profile, packProfile_t* best) {
	if (!earlyBreak) {
		long total_time = clock() - before_suite;
		double size_kb = (double)acc_size / (double)1024;
		printf("\n\n **** ALL SUCCEEDED ****\n %.0f kb   (%llu)", size_kb, acc_size);
		double time_sec = (double)total_time / 1000.0;
		printf("\n\Time %.0fs  (%d)", time_sec, total_time);
		double pack_ratio = (double)acc_size / (double)acc_size_org;
		printf("\nPack Ratio %.2f%%", pack_ratio * (double)100);

		double eff = size_kb / time_sec;
		printf("\nEfficiency %.2f kb/s\n\n", eff);

		if (best_size == 0 || (acc_size < best_size && acc_size != 0)) {
			copyProfile(&profile, best);
			best_size = acc_size;
			printf("\n\n\a ************ BEST FOUND *************");
		}
		else {
			printf("\n Not enough this time...\n");
		}
	}
	printf("\n  STATUS: %.0f kb   (%llu)", round((double)best_size / 1024.0), best_size);
	printProfile(best);
	printf("\n");
	return best_size;
}

void testmeta() {
	init_taken();

	packProfile_t profile, bestProfile, offsetProfile;

	profile.offset_pages = 216;
	profile.seqlen_pages = 2;
	profile.rle_ratio = 82;
	profile.twobyte_ratio = 88;
	profile.seq_ratio = 73;
	profile.recursive_limit = 64;
	profile.twobyte_threshold = 1415;

	offsetProfile.offset_pages = 105;
	offsetProfile.seqlen_pages = 57;
	offsetProfile.rle_ratio = 80;
	offsetProfile.twobyte_ratio = 79;
	offsetProfile.seq_ratio = 94;
	offsetProfile.recursive_limit = 100;
	offsetProfile.twobyte_threshold = 1328;

	copyProfile(&profile, &bestProfile);

	unsigned long long best_size = 0;
	while (true) {

		//const char** test_filenames = get_test_filenames();
		unsigned long long acc_size_packed = 0,
			acc_size_org = 0;

		int before_suite = clock();
		int kk = 0;
		bool earlyBreak = true;
		for (; kk < 34; kk++)
		{
			//const char* src = concat("C:/test/testsuite/", test_filenames[kk]);
			//const char* src = "C:/test/book_seqlens";

			const char src[100] = { 0 };

			concat_int(src, "C:/test/meta/seqlens", kk + 1000);

			const char* dst = "C:/test/unp";

			const char* packed_name = "c:/test/packed.bin";

			long long size_org = get_file_size_from_name(src);

			printf("\n Packing... %s with length:%d", src, size_org);

			int cl = clock();

			printProfile(&profile);
			/*
				copy_file(src, packed_name);
				bool seq = SeqPackAndTest(packed_name, offset_pages, seqlen_pages, ratio_limit);
				bool huffman = CanonicalEncodeAndTest(packed_name);
				*/

			packProfile_t seqlenProfile;
			seqlenProfile.offset_pages = 227;
			seqlenProfile.seqlen_pages = 1;
			seqlenProfile.rle_ratio = 82;
			seqlenProfile.twobyte_ratio = 88;
			seqlenProfile.seq_ratio = 67;
			seqlenProfile.recursive_limit = 86;
			seqlenProfile.twobyte_threshold = 1328;




			multi_pack(src, packed_name, profile, profile, offsetProfile);

			//seq_pack_separate(src, "c:/test/", 219, 2);
			//seq_pack(src, packed_name, 219, 2);

			int pack_time = (clock() - cl);
			//printf("\n Packing finished time it took: %d", pack_time);
			long long size_packed = get_file_size_from_name(packed_name);

			printf("\nLength of packed: %d", size_packed);
			printf("  (%f)", (double)size_packed / (double)size_org);

			acc_size_packed += size_packed;
			if (best_size > 0 && acc_size_packed > best_size) {
				break;
			}
			acc_size_org += size_org;
			printf("\n Accumulated size %d kb", acc_size_packed / 1024);
			cl = clock();
			/*
			if (huffman) {
				CanonicalDecodeAndReplace(packed_name);
			}
			if (seq) {
				seq_unpack(packed_name, dst);
			}
			else {
				copy_file(packed_name, dst);
			}
			*/

			multi_unpack(packed_name, dst);
			//seq_unpack_separate("c:/test/main", dst, "c:/test/");


			int unpack_time = (clock() - cl);
			//printf("\n Unpacking finished time it took: %d", unpack_time);
			printf("\nTimes %d/%d/%d", pack_time, unpack_time, pack_time + unpack_time);


			printf("\n\n Comparing files!");

			if (files_equal(src, dst)) {
				printf("\n ****** SUCCESS ****** (equal)\n");
			}
			else {
				return 1;
			}
			earlyBreak = false;
		}//end for
		best_size = presentResult(earlyBreak, before_suite, acc_size_packed, acc_size_org, best_size, profile, &bestProfile);
		fuzzProfile(&profile, bestProfile);
	}
}//end test_meta

void onefile() {

	unsigned long long acc_size_packed = 0,
		acc_size_org = 0;

	int before_suite = clock();

	const char* src = "c:/test/seqlens_fail";
	const char* unpacked_finished = "C:/test/unp";

	const char* packed_name = "c:/test/packed.bin";

	long long size_org = get_file_size_from_name(src);

	printf("\n Packing... %s with length:%d", src, size_org);

	int cl = clock();

	packProfile_t profile, offsetProfile;
	profile.offset_pages = 219;
	profile.seqlen_pages = 2;
	profile.rle_ratio = 63;
	profile.twobyte_ratio = 83;
	profile.seq_ratio = 82;
	profile.recursive_limit = 230;
	profile.twobyte_threshold = 1363;

	offsetProfile.offset_pages = 105;
	offsetProfile.seqlen_pages = 57;
	offsetProfile.rle_ratio = 80;
	offsetProfile.twobyte_ratio = 79;
	offsetProfile.seq_ratio = 94;
	offsetProfile.recursive_limit = 100;
	offsetProfile.twobyte_threshold = 1328;

	//seq_pack_separate(src, "c:/test/", 219, 2);
	//seq_pack(src, packed_name, 219, 2);
	multi_pack(src, packed_name, profile, profile, offsetProfile);

	int pack_time = (clock() - cl);
	//printf("\n Packing finished time it took: %d", pack_time);
	/*
	long long size_packed = get_file_size_from_name(packed_name);

	printf("\nLength of packed: %d", size_packed);
	
	printf("  (%f)", (double)size_packed / (double)size_org);

	acc_size_packed += size_packed;
		*/

	acc_size_org += size_org;
	//printf("\n Accumulated size %d kb", acc_size_packed / 1024);
	cl = clock();


	//seq_unpack_separate("c:/test/main", dst, "c:/test/");
	multi_unpack(packed_name, unpacked_finished);

	int unpack_time = (clock() - cl);
	//printf("\n Unpacking finished time it took: %d", unpack_time);
	printf("\nTimes %d/%d/%d", pack_time, unpack_time, pack_time + unpack_time);


	printf("\n\n Comparing files!");

	if (files_equal(src, unpacked_finished)) {
		printf("\n ****** SUCCESS ****** (equal)\n");
	}
	else {
		return 1;
	}
}//onefile

void test16() {

	char test_filenames[16][100] = { "book.txt","empty.txt","onechar.txt","oneseq.txt","repeatchar.txt",

		"book_med.txt",



		"bad.cdg",
		"aft.htm",
		"did.csh",
		"rel.pdf",
		"tob.pdf",
		"nex.doc",
		"amb.dll",
		"pazera.exe",
		"voc.wav",
		"bad.mp3"

	};

	//char test_filenames[2][100] = { "ragg.wav", "voc_short.wav" };

	init_taken();

	packProfile_t profile;
	profile.offset_pages = 230;
	profile.seqlen_pages = 29;
	profile.rle_ratio = 86;
	profile.twobyte_ratio = 94;
	profile.seq_ratio = 100;
	profile.recursive_limit = 94;
	profile.twobyte_threshold = 1650;

	packProfile_t bestProfile;
	copyProfile(&profile, &bestProfile);

	unsigned long long best_size = ULONG_MAX; // 44219553;
	while (true) {

		//const char** test_filenames = get_test_filenames();
		unsigned long long acc_size_packed = 0,
			acc_size_org = 0;

		int before_suite = clock();
		int kk = 0;
		bool earlyBreak = true;
		for (; kk < 16; kk++)
		{
			const char src[100] = { 0 };
			concat(src, "C:/test/test16/", test_filenames[kk]);


			const char* dst = "C:/test/unp";

			const char* packed_name = "c:/test/packed.bin";

			long long size_org = get_file_size_from_name(src);
			printf("\n------------------------------------------------");
			printf("\n Packing... %s with length:%d", src, size_org);

			int cl = clock();

			block_pack(src, packed_name, profile);
			//seq_pack_separate(src, "c:/test/", offset_pages, seqlen_pages);

			int pack_time = (clock() - cl);
			//printf("\n Packing finished time it took: %d", pack_time);
			long long size_packed = get_file_size_from_name(packed_name);

			printf("\n\n   --   RATIO OF PACKED   '%s'   %.2f%%   --\n\n", src, ((double)size_packed / (double)size_org) * 100.0);

			acc_size_packed += size_packed;
			if (best_size > 0 && acc_size_packed > best_size) {
				break;
			}
			acc_size_org += size_org;

			printf("\n Accumulated size %lu kb", acc_size_packed / 1024);

			cl = clock();

			block_unpack(packed_name, dst);
			//seq_unpack_separate("main", dst, "c:/test/");

			int unpack_time = (clock() - cl);
			//printf("\n Unpacking finished time it took: %d", unpack_time);
			printf("\nTimes %d/%d/%d", pack_time, unpack_time, pack_time + unpack_time);

			printf("\n\n Comparing files!");

			if (files_equal(src, dst)) {
				printf("\n ****** SUCCESS ****** (equal)\n");
			}
			else {
				return 1;
			}
			earlyBreak = false;
		}//end for
		best_size = presentResult(earlyBreak, before_suite, acc_size_packed, acc_size_org, best_size, profile, &bestProfile);
		fuzzProfile(&profile, bestProfile);
	}//end while true
}//end test suit 16

//-------------------------------------------------------------------------------

int main()
{

	time_t t;
	srand((unsigned)time(&t));

	//test16();
	testmeta();
	//onefile();
}

