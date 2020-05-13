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

//#include "huffman2.h"
//#include "canonical.h"


#define CMP_N 128

void print_string_rep(unsigned char* tt) {
	printf("\n");
	for (int i = 0; i < CMP_N; i++) {
		printf(", ", tt[i]);
	}
}

int files_equal(const char* source_filename, const char* dest_filename) {
	FILE* f1, * f2;

	fopen_s(&f1, source_filename, "rb");
	if (!f1) {
		printf("\nHittade inte utfil: %s", source_filename);
		getchar();
		exit(1);
	}

	fopen_s(&f2, dest_filename, "rb");
	if (!f2) {
		puts("Hittade inte utfil!");
		getchar();
		exit(1);
	}
	long f1_size = get_file_size(f1);
	long f2_size = get_file_size(f2);
	int res = 1;
	if (f1_size != f2_size) {
		printf("\n\a >>>>>>>>>>>>>>> FILES NOT EQUAL!!!! <<<<<<<<<<<<<<<<<<");
		printf("\n Lengths differ   %d  %d", f1_size, f2_size);
		res = 0;
	}
	unsigned char tmp1, tmp2;

	size_t bytes = 0;
	int count = 0;
	while (!feof(f1) && !feof(f2)) {
		fread(&tmp1, 1, 1, f1);
		fread(&tmp2, 1, 1, f2);

		if (tmp1 != tmp2) {

			printf("\n Contents differ at position  %d ", count);
			printf("\n File1:");
			printf("%c", tmp1);
			//print_string_rep(tmp1);
			printf("\n File2:");
			//print_string_rep(tmp2);
			printf("%c", tmp2);
			return 0;
		}
		count++;
	}
	fclose(f1);
	fclose(f2);
	return res;
}

int fuzz(int i) {
	do {
		if (rand() % 2 == 0) {
			i += (rand() % 10);
		}
		else {
			i -= (rand() % 10);
		}
	} while (!(i >= 0 && i <= 245));
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

int fuzzRatio(int r, int best) {
	if (rand() % 2 == 0) {
		r += (rand() % 5 + 1);
	}
	else {
		r -= (rand() % 5 + 1);
	}
	if (r > 100 || rand() % 6 == 0) {

		r = best;

	}
	return r;
}

void fuzzProfile(packProfile_t* profile, packProfile_t best) {
	if (rand() % 4 == 0) {
		profile->offset_pages = rand() % 240;
		profile->seqlen_pages = rand() % 240;
	}
	else {

		profile->offset_pages = fuzz(best.offset_pages);
		profile->seqlen_pages = fuzz(best.seqlen_pages);

	}
	profile->rle_ratio = fuzzRatio(profile->rle_ratio, best.rle_ratio);
	profile->twobyte_ratio = fuzzRatio(profile->twobyte_ratio, best.twobyte_ratio);
	profile->seq_ratio = fuzzRatio(profile->seq_ratio, best.seq_ratio);
}

unsigned long long presentResult(bool earlyBreak, int before_suite, unsigned long long acc_size, unsigned long long acc_size_org,
	unsigned long long best_size, packProfile_t profile, packProfile_t* best) {
	if (!earlyBreak) {
		long total_time = clock() - before_suite;
		double size_kb = round((double)acc_size / (double)1024);
		printf("\n\n **** ALL SUCCEEDED **** ");
		double time_sec = round((double)total_time / (double)1000);
		printf("\n\Time %.0fs  (%d)", time_sec, total_time);
		double pack_ratio = (double)acc_size / (double)acc_size_org;
		printf("\nPack Ratio %.2f%%", pack_ratio * (double)100);

		double eff = ((double)(acc_size_org - acc_size) / (double)1024) / time_sec;
		printf("\nSpeed %.2f kb/s\n\n", eff);

		if (best_size == 0 || acc_size < best_size) {
			copyProfile(&profile, best);
			best_size = acc_size;
			printf("\n\n\a ************ BEST FOUND *************");
		}
	}
	printf("\n\n STATUS: size %llu   (%.0f kb)", best_size, round((double)best_size / 1024.0));
	printProfile(best);
	printf("\n");
	return best_size;
}

void testmeta() {
	init_taken();

	packProfile_t profile, bestProfile;

	profile.offset_pages = 51;
	profile.seqlen_pages = 115;
	profile.rle_ratio = 90;
	profile.twobyte_ratio = 80;
	profile.seq_ratio = 97;

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

			concat_int(src, "C:/test/meta/offsets", kk + 1000);

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

			multi_pack(src, packed_name, profile);

			//seq_pack_separate(src, "c:/test/", offset_pages, seqlen_pages);

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

void test16() {

	char test_filenames[16][100] = { "book.txt",
		"empty.txt",
		"onechar.txt",
			"oneseq.txt",
		"repeatchar.txt",
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

		"bad.mp3",

	};

	init_taken();

	packProfile_t profile;
	profile.offset_pages = 230;
	profile.seqlen_pages = 31;
	profile.rle_ratio = 96;
	profile.twobyte_ratio = 97;
	profile.seq_ratio = 100;

	packProfile_t bestProfile;
	copyProfile(&profile, &bestProfile);

	unsigned long long best_size = 0;
	while (true) {

		//const char** test_filenames = get_test_filenames();
		unsigned long long acc_size_packed = 0,
			acc_size_org = 0;

		int before_suite = clock();
		int kk = 0;
		bool earlyBreak = true;
		for (; kk < 13; kk++)
		{
			const char src[100] = { 0 };
			concat(src, "C:/test/testsuite/", test_filenames[kk]);


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

			printf("\nLength of packed: %d", size_packed);
			printf("  (%f)", (double)size_packed / (double)size_org);

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
	}
}//end test suit 16

//-------------------------------------------------------------------------------

int main()
{

	time_t t;
	srand((unsigned)time(&t));

	test16();
	//testmeta();

}

