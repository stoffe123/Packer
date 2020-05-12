#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common_tools.h"
#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "multi_packer.h"
//#include "huffman2.h"
//#include "canonical.h"
#include "multi_packer.h"
#include "block_packer.h"
#include <Windows.h>
#include "packer_commons.h"

#include "RLE_packer_advanced.h"

#define CMP_N 128

void print_string_rep(unsigned char* tt) {
	printf("\n");
	for (int i = 0; i < CMP_N; i++) {
		printf("%d, ", tt[i]);
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


void testmeta() {
	init_taken();

	int offset_pages = 51, seqlen_pages = 115, best_offset_pages = offset_pages, best_seqlen_pages = seqlen_pages,
		rle_ratio = 90, twobyte_ratio = 80, seq_ratio = 97,
		best_seq_ratio = seq_ratio, best_rle_ratio = rle_ratio, best_twobyte_ratio = twobyte_ratio;

	unsigned long long best_size = 1789985;
	while (true) {

		set_taken(offset_pages, seqlen_pages);

		//const char** test_filenames = get_test_filenames();
		unsigned long long acc_size = 0, acc_size_org = 0;

		int before_suite = clock();
		int kk = 0;
		for (; kk < 34; kk++)
		{
			//const char* src = concat("C:/test/testsuite/", test_filenames[kk]);
			//const char* src = "C:/test/book_seqlens";

			const char* src = concat_int("C:/test/meta/offsets", kk + 1000);

			const char* dst = "C:/test/unp";

			const char* packed_name = "c:/test/packed.bin";

			long long size_org = get_file_size_from_name(src);

			printf("\n Packing... %s with length:%d", src, size_org);

			int cl = clock();

			//	printf("\n\n  ------- Pages %d --------- ", i);
			printf("\nrle_pack Ratio limit %d", rle_ratio);
			printf("\ntwobyte_pack Ratio limit %d", twobyte_ratio);
			printf("\nSeqpack Ratio limit %d", seq_ratio);
			/*
				copy_file(src, packed_name);
				bool seq = SeqPackAndTest(packed_name, offset_pages, seqlen_pages, ratio_limit);
				bool huffman = CanonicalEncodeAndTest(packed_name);
				*/

			multi_pack(src, packed_name, offset_pages, seqlen_pages, rle_ratio, twobyte_ratio, seq_ratio);

			//seq_pack_separate(src, "c:/test/", offset_pages, seqlen_pages);

			int pack_time = (clock() - cl);
			//printf("\n Packing finished time it took: %d", pack_time);
			long long size_packed = get_file_size_from_name(packed_name);

			printf("\nLength of packed: %d", size_packed);
			printf("  (%f)", (double)size_packed / (double)size_org);

			acc_size += size_packed;
			if (acc_size > best_size) {
				break;
			}
			acc_size_org += size_org;
			printf("\n Accumulated size %d kb", acc_size / 1024);
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
		}
		if (kk == 34) {
			long total_time = clock() - before_suite;
			double size_kb = round((double)acc_size / (double)1024);
			printf("\n\n **** ALL SUCCEEDED **** pages (%d,%d) \n%.0f kb   (%d)", offset_pages, seqlen_pages, size_kb, acc_size);
			double time_sec = round((double)total_time / (double)1000);
			printf("\n\Time %.0fs  (%d)", time_sec, total_time);
			double pack_ratio = (double)acc_size / (double)acc_size_org;
			printf("\nPack Ratio %.2f \%", pack_ratio * (double)100);

			double eff = ((double)(acc_size_org - acc_size) / (double)1024) / time_sec;
			printf("\nRate  %.2f kb/s\n\n", eff);
			//break;
			if (acc_size < best_size) {
				best_offset_pages = offset_pages;
				best_seqlen_pages = seqlen_pages;
				best_size = acc_size;
				best_seq_ratio = seq_ratio;
				best_rle_ratio = rle_ratio;
				best_twobyte_ratio = twobyte_ratio;
				printf("\n\n\a ************ BEST FOUND *************");
			}
		}
		printf("\n\n STATUS BEST: (%d,%d) size %d", best_offset_pages, best_seqlen_pages, best_size);
		printf("\n rle_ratio %d", best_rle_ratio);
		printf("\n twobyte_ratio %d", best_twobyte_ratio);
		printf("\n seq_ratio %d", best_seq_ratio);
		printf("\n");


		if (rand() % 4 == 0) {
			offset_pages = rand() % 240;
			seqlen_pages = rand() % 240;
		}
		else {

			offset_pages = fuzz(best_offset_pages);
			seqlen_pages = fuzz(best_seqlen_pages);

		}
		rle_ratio = fuzzRatio(rle_ratio, best_rle_ratio);
		twobyte_ratio = fuzzRatio(twobyte_ratio, best_twobyte_ratio);
		seq_ratio = fuzzRatio(seq_ratio, best_seq_ratio);


	}
}//end test_meta

void testsuit16() {

	char test_filenames[16][100] = { "bad.mp3", "book.txt",
		"empty.txt",
		"onechar.txt",
			"oneseq.txt",
		"repeatchar.txt",
		 "book_med.txt",
		"bad.cdg",
		"aft.htm",
			"tob.pdf",
		"voc.wav",
		  "rel.pdf",
		"did.csh",
			 "nex.doc",
		"amb.dll",
		"pazera.exe"

	};

	init_taken();
	set_taken(230, 34);

	int offset_pages = 230, seqlen_pages = 31,
		best_offset_pages = 230, best_seqlen_pages = 31;

	unsigned long long best_size = 44244806;
	while (true) {

		set_taken(offset_pages, seqlen_pages);
		//const char** test_filenames = get_test_filenames();
		unsigned long long acc_size = 0, acc_size_org = 0;

		int before_suite = clock();
		for (int kk = 0; kk < 16; kk++)
		{
			const char* src = concat("C:/test/testsuite/", test_filenames[kk]);


			const char* dst = "C:/test/unp";

			const char* packed_name = "c:/test/packed.bin";

			long long size_org = get_file_size_from_name(src);
			printf("\n------------------------------------------------");
			printf("\n Packing... %s with length:%d", src, size_org);

			int cl = clock();

			//	printf("\n\n  ------- Pages %d --------- ", i);

			block_pack(src, packed_name, offset_pages, seqlen_pages, 96, 97, 100);
			//seq_pack_separate(src, "c:/test/", offset_pages, seqlen_pages);

			int pack_time = (clock() - cl);
			//printf("\n Packing finished time it took: %d", pack_time);
			long long size_packed = get_file_size_from_name(packed_name);

			printf("\nLength of packed: %d", size_packed);
			printf("  (%f)", (double)size_packed / (double)size_org);

			acc_size += size_packed;
			if (acc_size > best_size) {
				//	break;
			}
			acc_size_org += size_org;

			printf("\n Accumulated size %d kb", acc_size / 1024);

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
		}
		long total_time = clock() - before_suite;
		double size_kb = round((double)acc_size / (double)1024);
		printf("\n\n\a **** ALL SUCCEEDED **** pages (%d,%d)\n%.0f kb   (%d)", offset_pages, seqlen_pages, size_kb, acc_size);
		printf("\nBlock size %d", BLOCK_SIZE);
		double time_sec = round((double)total_time / (double)1000);
		printf("\n\Time %.0fs  (%d)", time_sec, total_time);
		double ratio = (double)acc_size / (double)acc_size_org;
		printf("\nPack Ratio %.2f \%", ratio * (double)100);

		double eff = ((double)(acc_size_org - acc_size) / (double)1024) / time_sec;
		printf("\nRate  %.2f kb/s\n\n", eff);

		if (acc_size < best_size) {
			best_offset_pages = offset_pages;
			best_seqlen_pages = seqlen_pages;
			best_size = acc_size;
			printf("\n\n\a ************ BEST FOUND ************* (%d,%d) %d", offset_pages, seqlen_pages, acc_size);
		}
		else {
			printf("\n\n STATUS: (%d,%d) %d", best_offset_pages, best_seqlen_pages, best_size);
		}

		do {
			if (rand() % 9 == 0) {
				offset_pages = rand() % 240;
				seqlen_pages = rand() % 240;
			}
			else {

				offset_pages = fuzz(best_offset_pages);
				seqlen_pages = fuzz(best_seqlen_pages);

			}
		} while (is_taken(offset_pages, seqlen_pages));

	}
}//end test suit 16

//-------------------------------------------------------------------------------

int main()
{

	time_t t;
	srand((unsigned)time(&t));

	//testsuit16();
	testmeta();

}

