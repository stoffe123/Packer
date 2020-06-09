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
#include "archive_packer.h"

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
	} while (!(i >= 0 && i <= 248));
	return i;
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

int doFuzz(int r, int best, int min, int max) {
	r = fuzzVal(r, max / 10);
	if (r < 0 || r > max || r < min || rand() % 6 == 0) {
		r = best;
	}
	return r;
}

void fuzzProfile(packProfile* profile, packProfile best) {
	
	if (rand() % 4 == 0) {
		profile->offset_pages = rand() % 252;
		profile->seqlen_pages = rand() % 252;
	}
	else {

		profile->offset_pages = fuzz(best.offset_pages);
		profile->seqlen_pages = fuzz(best.seqlen_pages);
	}
	
	profile->rle_ratio = doFuzz(profile->rle_ratio, best.rle_ratio, 10, 100);
	profile->twobyte_ratio = doFuzz(profile->twobyte_ratio, best.twobyte_ratio, 10, 100);
	profile->seq_ratio = doFuzz(profile->seq_ratio, best.seq_ratio, 99, 100);
	profile->recursive_limit = doFuzz(profile->recursive_limit, best.recursive_limit, 10, 800);

	profile->twobyte_threshold_max = doFuzz(profile->twobyte_threshold_max, best.twobyte_threshold_max, 40, 13000);
	profile->twobyte_threshold_divide = doFuzz(profile->twobyte_threshold_divide, best.twobyte_threshold_divide, 20, 3000);
	profile->twobyte_threshold_min = doFuzz(profile->twobyte_threshold_min, best.twobyte_threshold_min, 10, 1000);

	profile->twobyte2_ratio = doFuzz(profile->twobyte2_ratio, best.twobyte2_ratio, 10, 100);
	profile->twobyte2_threshold_max = doFuzz(profile->twobyte2_threshold_max, best.twobyte2_threshold_max, 40, 13000);
	profile->twobyte2_threshold_divide = doFuzz(profile->twobyte2_threshold_divide, best.twobyte2_threshold_divide, 20, 3000);
	profile->twobyte2_threshold_min = doFuzz(profile->twobyte2_threshold_min, best.twobyte2_threshold_min, 10, 1000);
}


unsigned long long presentResult(bool earlyBreak, int before_suite, unsigned long long acc_size, unsigned long long acc_size_org,
	unsigned long long best_size, packProfile profile, packProfile* best) {
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

//------------------------------------------------------------------------------------------

void testmeta() {

	packProfile bestProfile, bestOffsetProfile, seqlenProfile;

	//seqlen testsuit 236758
	packProfile profile = getPackProfile(197, 49);
	profile.rle_ratio = 73;
	profile.twobyte_ratio = 84;
	profile.seq_ratio = 68;
	profile.recursive_limit = 100;
	profile.twobyte_threshold_max = 454;
	profile.twobyte_threshold_divide = 2346;
	profile.twobyte_threshold_min = 332;
	profile.twobyte2_ratio = 100;
	profile.twobyte2_threshold_max = 116;
	profile.twobyte2_threshold_divide = 1495;
	profile.twobyte2_threshold_min = 63;

	packProfile offsetProfile = getPackProfile(109, 54);
	offsetProfile.rle_ratio = 61;
	offsetProfile.twobyte_ratio = 90;
	offsetProfile.seq_ratio = 92;
	offsetProfile.recursive_limit = 47;
	offsetProfile.twobyte_threshold_max = 3062;
	offsetProfile.twobyte_threshold_divide = 1367;
	offsetProfile.twobyte_threshold_min = 898;
	offsetProfile.twobyte2_ratio = 95;
	offsetProfile.twobyte2_threshold_max = 3000;
	offsetProfile.twobyte2_threshold_divide = 1000;
	offsetProfile.twobyte2_threshold_min = 22;

	copyProfile(&profile, &bestProfile);
	copyProfile(&offsetProfile, &bestOffsetProfile);

	unsigned long long best_size = 0;
	while (true) {

		//const char** test_filenames = get_test_filenames();
		unsigned long long acc_size_packed = 0,
			acc_size_org = 0;

		int before_suite = clock();
		int kk = 0;
		bool earlyBreak = true;
		printProfile(&profile);
		for (; kk < 34; kk++)
		{
			const char src[100] = { 0 };

			concat_int(src, "C:/test/meta/seqlens", kk + 1000);
			const char* dst = "C:/test/unp";
			const char* packed_name = "c:/test/packed.bin";
			long long size_org = get_file_size_from_name(src);
			printf("\n Packing... %s with length:%d", src, size_org);
			int cl = clock();
			multi_pack(src, packed_name, offsetProfile, profile, profile);
				
			long long size_packed = get_file_size_from_name(packed_name);
			acc_size_packed += size_packed;
			earlyBreak = (best_size > 0 && acc_size_packed > best_size);
			if (earlyBreak) {
				break;
			}
			acc_size_org += size_org;


			concat_int(src, "C:/test/meta/offsets", kk + 1000);
			size_org = get_file_size_from_name(src);
			printf("\n Packing... %s with length:%d", src, size_org);
			
			multi_pack(src, packed_name, offsetProfile, profile, offsetProfile);
			int pack_time = (clock() - cl);
		
			size_packed = get_file_size_from_name(packed_name);
			acc_size_packed += size_packed;
			earlyBreak = (best_size > 0 && acc_size_packed > best_size);
			if (earlyBreak) {
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

			//multi_unpack(packed_name, dst);
			//seq_unpack_separate("c:/test/main", dst, "c:/test/");


			int unpack_time = (clock() - cl);
			//printf("\n Unpacking finished time it took: %d", unpack_time);
			printf("\nTimes %d/%d/%d", pack_time, unpack_time, pack_time + unpack_time);

			/*
			printf("\n\n Comparing files!");
			
			if (files_equal(src, dst)) {
				printf("\n ****** SUCCESS ****** (equal)\n");
			}
			else {
				return 1;
			}
			*/
			earlyBreak = false;
		}//end for
		unsigned long long old_best_size = best_size;
		best_size = presentResult(earlyBreak, before_suite, acc_size_packed, acc_size_org, best_size, profile, &bestProfile);
		if (old_best_size != best_size) {
			old_best_size = best_size;
			copyProfile(&offsetProfile, &bestOffsetProfile);
			
		}
		printf("\nBest Offset Profile:");
		printProfile(&offsetProfile);
		fuzzProfile(&profile, bestProfile);
		fuzzProfile(&offsetProfile, bestOffsetProfile);
	}
}//end test_meta

//-------------------------------------------------------------------------------------------

void onefile() {

	unsigned long long acc_size_packed = 0,
		acc_size_org = 0;

	int before_suite = clock();

	const char* src = "c:/test/voc.wav";
	const char* unpacked_finished = "C:/test/unp";

	const char* packed_name = "c:/test/packed.bin";

	long long size_org = get_file_size_from_name(src);

	printf("\n Packing... %s with length:%d", src, size_org);

	int cl = clock();

	packProfile profile, offsetProfile;
	profile.offset_pages = 219;
	profile.seqlen_pages = 2;
	profile.rle_ratio = 63;
	profile.twobyte_ratio = 100;
	profile.seq_ratio = 82;
	profile.recursive_limit = 230;
	profile.twobyte_threshold_max = 3;
	profile.twobyte_threshold_divide = 2500;
	profile.twobyte_threshold_min = 3;

	offsetProfile.offset_pages = 105;
	offsetProfile.seqlen_pages = 57;
	offsetProfile.rle_ratio = 80;
	offsetProfile.twobyte_ratio = 79;
	offsetProfile.seq_ratio = 94;
	offsetProfile.recursive_limit = 100;
	offsetProfile.twobyte_threshold_max = 1328;
	offsetProfile.twobyte_threshold_divide = 1000;
	offsetProfile.twobyte_threshold_min = 50;

	//seq_pack_separate(src, "c:/test/", 219, 2);
	//seq_pack(src, packed_name, 219, 2);
	two_byte_pack(src, packed_name, profile);

	int pack_time = (clock() - cl);
		
	acc_size_org += size_org;
	//printf("\n Accumulated size %d kb", acc_size_packed / 1024);
	cl = clock();


	//seq_unpack_separate("c:/test/main", dst, "c:/test/");
	two_byte_unpack(packed_name, unpacked_finished);

	int unpack_time = (clock() - cl);
	//printf("\n Unpacking finished time it took: %d", unpack_time);
	printf("\nTimes %d/%d/%d", pack_time, unpack_time, pack_time + unpack_time);
	uint64_t size_packed = get_file_size_from_name(packed_name);
	printf("\n\n   --   RATIO OF PACKED   '%s'   %.2f%%   --\n\n", src, ((double)size_packed / (double)size_org) * 100.0);


	printf("\n\n Comparing files!");

	if (files_equal(src, unpacked_finished)) {
		printf("\n ****** SUCCESS ****** (equal)\n");
	}
	else {
		return 1;
	}
}//onefile

//---------------------------------------------------------------------------------------

void test16() {
	
	wchar_t test_filenames[16][100] = { 
		L"rel.pdf",		
		
		
		
		L"bad.cdg",
		L"did.csh",
		
		L"nex.doc",
		L"amb.dll",
	    L"bad.mp3",
		L"aft.htm",
		L"book_med.txt",
		L"book.txt",
		L"empty.txt",
		L"onechar.txt",
		L"oneseq.txt",
		L"repeatchar.txt",
		L"pazera.exe",
	    L"voc.wav",
		L"tob.pdf"
		
	};
	
	//wchar_t test_filenames[3][100] = { L"ragg.wav", L"voc_short.wav", L"voc.wav" };

	packProfile profile = getPackProfile(236, 158);
	profile.rle_ratio = 84;
	profile.twobyte_ratio = 94;
	profile.seq_ratio = 100;
	profile.recursive_limit = 136;
	profile.twobyte_threshold_max = 11788;
	profile.twobyte_threshold_divide = 69;
	profile.twobyte_threshold_min = 3150;
	profile.twobyte2_ratio = 93;
	profile.twobyte2_threshold_max = 5000;
	profile.twobyte2_threshold_divide = 309;
	profile.twobyte2_threshold_min = 12;

	packProfile bestProfile;
	copyProfile(&profile, &bestProfile);
	

	unsigned long long best_size = 0; // 44127835; // (43094 kb)
	while (true) 
	{

		//const char** test_filenames = get_test_filenames();
		unsigned long long acc_size_packed = 0,
			acc_size_org = 0;

		int before_suite = clock();
		int kk = 0;
		bool earlyBreak = true;
		for (; kk < 16; kk++)
		{
			const wchar_t src[200] = { 0 };
			concatw(src, L"C:/test/test16/", test_filenames[kk]);

			const wchar_t* dst = L"C:/test/unp";

			const wchar_t* packed_name = L"c:/test/packed.bin";

			uint64_t size_org = get_file_size_from_wname(src);
			printf("\n------------------------------------------------");
			wprintf(L"\n Packing... %s with length:%d", src, size_org);

			int cl = clock();

			block_pack(src, packed_name, profile);
			//seq_pack_separate(src, "c:/test/", profile);

			int pack_time = (clock() - cl);
			//printf("\n Packing finished time it took: %d", pack_time);
			uint64_t size_packed = get_file_size_from_wname(packed_name);

			wprintf(L"\n\n   --   RATIO OF PACKED   '%s'   %.2f%%   --\n\n", src, ((double)size_packed / (double)size_org) * 100.0);

			acc_size_packed += size_packed;
			earlyBreak = (best_size > 0 && acc_size_packed > best_size);
			if (earlyBreak) {
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
			if (files_equalw(src, dst)) {
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

//-----------------------------------------------------------------------------------------

void testarchive() {
	wchar_t* test_filenames[16] = {
	L"rel.pdf",
	L"bad.cdg",		
	L"nex.doc",					
	L"book_med.txt",
	L"book.txt",
	L"empty.txt",
	L"onechar.txt",
	L"oneseq.txt",
	L"repeatchar.txt",	
		L"did.csh",				
		L"aft.htm",
		L"amb.dll",
		L"bad.mp3",
		L"voc.wav",
		L"tob.pdf",
		L"pazera.exe"
	};

	packProfile profile;
	profile.offset_pages = 221;
	profile.seqlen_pages = 204;
	profile.rle_ratio = 85;
	profile.twobyte_ratio = 73;
	profile.seq_ratio = 100;
	profile.recursive_limit = 79;
	profile.twobyte_threshold_max = 1180;
	profile.twobyte_threshold_divide = 529;
	profile.twobyte_threshold_min = 81;

	packProfile bestProfile;
	copyProfile(&profile, &bestProfile);
	wchar_t* destDir = L"c:/test/archiveunp/";
	unsigned long long best_size = 0;
	const wchar_t* packed_name = L"c:/test/packed.bin";
	while (true) {

		unsigned long long acc_size_packed = 0,
			acc_size_org = 0;

		int before_suite = clock();
		wchar_t* source_dir = L"c:\\test\\test13";
		
		wchar_t filenames2[16][200], filenames3[16][200];
		for (int i = 0; i < 13; i++) {
			concat3w(filenames2[i], source_dir, L"\\", test_filenames[i]);
			concat3w(filenames3[i], destDir, L"\\", test_filenames[i]);			
		}

		int cl = clock();
		archive_pack(source_dir, packed_name, profile);
		int pack_time = (clock() - cl);
		uint64_t size_packed = get_file_size_from_wname(packed_name);
		acc_size_packed += size_packed;
		
		printf("\n Accumulated size %lu kb", acc_size_packed / 1024);

		cl = clock();

		archive_unpack(packed_name, destDir);

		int unpack_time = (clock() - cl);
		//printf("\n Unpacking finished time it took: %d", unpack_time);
		printf("\nTimes %d/%d/%d", pack_time, unpack_time, pack_time + unpack_time);

	
		printf("\n\n Comparing files!");
		for (int i = 0; i < 13; i++) {

			if (files_equalw(filenames2[i], filenames3[i])) {
				printf("\n ****** SUCCESS ****** (equal)\n");
			}
			else {
				return 1;
			}		
		}
		best_size = presentResult(false, before_suite, acc_size_packed, acc_size_org, best_size, profile, &bestProfile);
		fuzzProfile(&profile, bestProfile);
	}//end while true

}

//-------------------------------------------------------------------------------

int main()
{

	time_t t;
	srand((unsigned)time(&t));

	//testmeta();
	test16();
	//testarchive();
	//onefile();
}