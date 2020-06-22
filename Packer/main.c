#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common_tools.h"  
#include "seq_packer.h"  
#include "multi_packer.h"
#include "block_packer.h"
#include <Windows.h>
#include "packer_commons.h"
#include "Two_byte_packer.h"
#include "RLE_packer_advanced.h"
#include "archive_packer.h"
#include "canonical_header_packer.h"

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
	
	profile->rle_ratio = doFuzz(profile->rle_ratio, best.rle_ratio, 10, 100);
	profile->twobyte_ratio = doFuzz(profile->twobyte_ratio, best.twobyte_ratio, 10, 100);
	profile->recursive_limit = doFuzz(profile->recursive_limit, best.recursive_limit, 10, 700);

	profile->twobyte_threshold_max = doFuzz(profile->twobyte_threshold_max, best.twobyte_threshold_max, 3, 13000);
	profile->twobyte_threshold_divide = doFuzz(profile->twobyte_threshold_divide, best.twobyte_threshold_divide, 20, 4000);
	profile->twobyte_threshold_min = doFuzz(profile->twobyte_threshold_min, best.twobyte_threshold_min, 3, 1000);
	
	profile->seqlenMinLimit3 = doFuzz(profile->seqlenMinLimit3, best.seqlenMinLimit3, 0, 255);
	profile->seqlenMinLimit4 = doFuzz(profile->seqlenMinLimit4, best.seqlenMinLimit4, 1, 255);
	profile->blockSizeMinus = doFuzz(profile->blockSizeMinus, best.blockSizeMinus, 0, 255);
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
	printf("\n  STATUS:\n%.0f kb   (%llu)", round((double)best_size / 1024.0), best_size);
	printProfile(best);
	printf("\n");
	return best_size;
}

//------------------------------------------------------------------------------------------

void testmeta() {

	packProfile bestProfile, bestOffsetProfile, bestDistanceProfile;

	//meta testsuit 1170291
	packProfile seqlenProfile = getPackProfile(0, 0, 0);
	seqlenProfile.rle_ratio = 50;
	seqlenProfile.twobyte_ratio = 87;
	seqlenProfile.recursive_limit = 208;
	seqlenProfile.twobyte_threshold_max = 10531;
	seqlenProfile.twobyte_threshold_divide = 3066;
	seqlenProfile.twobyte_threshold_min = 167;
	seqlenProfile.seqlenMinLimit3 = 57;
	seqlenProfile.seqlenMinLimit4 = 205;
	
	packProfile offsetProfile = getPackProfile(0, 0, 0);
	offsetProfile.rle_ratio = 99;
	offsetProfile.twobyte_ratio = 74;
	offsetProfile.recursive_limit = 19;
	offsetProfile.twobyte_threshold_max = 5807;
	offsetProfile.twobyte_threshold_divide = 3053;
	offsetProfile.twobyte_threshold_min = 397;
	offsetProfile.seqlenMinLimit3 = 83;
	offsetProfile.seqlenMinLimit4 = 183;

	packProfile distanceProfile = getPackProfile(0, 0, 0);
	distanceProfile.rle_ratio = 93;
	distanceProfile.twobyte_ratio = 99;
	distanceProfile.recursive_limit = 45;
	distanceProfile.twobyte_threshold_max = 3092;
	distanceProfile.twobyte_threshold_divide = 2749;
	distanceProfile.twobyte_threshold_min = 37;
	distanceProfile.seqlenMinLimit3 = 119;
	distanceProfile.seqlenMinLimit4 = 183;
	
	copyProfile(&seqlenProfile, &bestProfile);
	copyProfile(&offsetProfile, &bestOffsetProfile);
	copyProfile(&distanceProfile, &bestDistanceProfile);
	bool unpack = false;
	unsigned long long best_size = 0;
	while (true) {
		/*
		seqlenProfile.offset_pages = 0;
		seqlenProfile.seqlen_pages = 0;
		offsetProfile.offset_pages = 0;
		offsetProfile.seqlen_pages = 0;
		*/

		//const char** test_filenames = get_test_filenames();
		unsigned long long acc_size_packed = 0,
			acc_size_org = 0;

		int before_suite = clock();
		int kk = 0;
		bool earlyBreak = true;
		printf("\n Now testing with seqlenprofile:");
		printProfile(&seqlenProfile);
		printf("\n\n and offset profile:");
		printProfile(&offsetProfile);
		
		printf("\n-------------------------------");
		for (; kk < 38; kk++)
		{
			const char src[100] = { 0 };

			const char* metaDir = "D:/Dropbox/Personal/Programmering/Compression/test/meta/";
			concat(src, metaDir, "seqlens");
			concat_int(src, src, kk + 101);			
			const char* unpackedFilename = "C:/test/unp";
			const char* packed_name = "c:/test/packed.bin";
			long long size_org = get_file_size_from_name(src);
			printf("\n Packing %s      length:%d", src, size_org);
			int cl = clock();
			multi_pack(src, packed_name, seqlenProfile, seqlenProfile, offsetProfile, distanceProfile);
				
			long long size_packed = get_file_size_from_name(packed_name);
			acc_size_packed += size_packed;
			earlyBreak = (best_size > 0 && acc_size_packed > best_size);
			if (earlyBreak) {
				break;
			}
			acc_size_org += size_org;
			if (unpack) {
				multi_unpack(packed_name, unpackedFilename);
				printf("\n\n Comparing files!");
				if (files_equal(src, unpackedFilename)) {
					printf("\n ****** SUCCESS ****** (equal)\n");
				}
				else {
					exit(1);
				}
			}


			concat(src, metaDir, "offsets");
			concat_int(src, src, kk + 101);
			size_org = get_file_size_from_name(src);
			printf("\n Packing... %s with length:%d", src, size_org);
			
			multi_pack(src, packed_name, offsetProfile, seqlenProfile, offsetProfile, distanceProfile);
			int pack_time = (clock() - cl);
		
			size_packed = get_file_size_from_name(packed_name);
			acc_size_packed += size_packed;
			earlyBreak = (best_size > 0 && acc_size_packed > best_size);
			if (earlyBreak) {
				break;
			}
			acc_size_org += size_org;
			if (unpack) {
				multi_unpack(packed_name, unpackedFilename);
				printf("\n\n Comparing files!");
				if (files_equal(src, unpackedFilename)) {
					printf("\n ****** SUCCESS ****** (equal)\n");
				}
				else {
					exit(1);
				}
			}

			concat(src, metaDir, "distances");
			concat_int(src, src, kk + 101);
			size_org = get_file_size_from_name(src);
			printf("\n Packing... %s with length:%d", src, size_org);

			multi_pack(src, packed_name, distanceProfile, seqlenProfile, offsetProfile, distanceProfile);
			pack_time = (clock() - cl);

			size_packed = get_file_size_from_name(packed_name);
			acc_size_packed += size_packed;
			earlyBreak = (best_size > 0 && acc_size_packed > best_size);
			if (earlyBreak) {
				break;
			}
			acc_size_org += size_org;

			if (unpack) multi_unpack(packed_name, unpackedFilename);

			printf("\n Accumulated size %d kb", acc_size_packed / 1024);
			cl = clock();

			int unpack_time = (clock() - cl);
			//printf("\n Unpacking finished time it took: %d", unpack_time);
			printf("\nTimes %d/%d/%d", pack_time, unpack_time, pack_time + unpack_time);

			if (unpack) {
				printf("\n\n Comparing files!");
				if (files_equal(src, unpackedFilename)) {
					printf("\n ****** SUCCESS ****** (equal)\n");
				}
				else {
					exit(1);
				}
			}
		
			earlyBreak = false;
		}//end for
		unsigned long long old_best_size = best_size;
		best_size = presentResult(earlyBreak, before_suite, acc_size_packed, acc_size_org, best_size, seqlenProfile, &bestProfile);
		
		if (old_best_size != best_size) {
			old_best_size = best_size;
			copyProfile(&offsetProfile, &bestOffsetProfile);	
			copyProfile(&distanceProfile, &bestDistanceProfile);
		}
		printf("\nOffset Profile:");
		printProfile(&bestOffsetProfile);
		printf("\nDistance Profile:");
		printProfile(&bestDistanceProfile);
		printf("\n-------------------------------\n");
		fuzzProfile(&seqlenProfile, bestProfile);
		fuzzProfile(&offsetProfile, bestOffsetProfile);
		fuzzProfile(&distanceProfile, bestDistanceProfile);
	}
}//end test_meta

//-------------------------------------------------------------------------------------------

void onefile() {

	int before_suite = clock();

	const wchar_t* src = L"c:/test/short.txt";
	const wchar_t* unpackedFilename = L"C:/test/unp";

	const wchar_t* packed_name = L"c:/test/packed.bin";

	long long size_org = get_file_size_from_wname(src);

	wprintf(L"\n Packing... %s with length:%d", src, size_org);

	int cl = clock();

	packProfile profile = getPackProfile();
	profile.rle_ratio = 85;
	profile.twobyte_ratio = 90;
	profile.recursive_limit = 242;
	profile.twobyte_threshold_max = 11788;
	profile.twobyte_threshold_divide = 159;
	profile.twobyte_threshold_min = 3150;

	//meta testsuit 838297
	packProfile seqlenProfile = getPackProfile();
	seqlenProfile.rle_ratio = 64;
	seqlenProfile.twobyte_ratio = 62;
	seqlenProfile.recursive_limit = 10;
	seqlenProfile.twobyte_threshold_max = 8373;
	seqlenProfile.twobyte_threshold_divide = 3598;
	seqlenProfile.twobyte_threshold_min = 24;
	seqlenProfile.seqlenMinLimit4 = 156;

	packProfile offsetProfile = getPackProfile(72, 56, 10);
	offsetProfile.rle_ratio = 56;
	offsetProfile.twobyte_ratio = 82;
	offsetProfile.recursive_limit = 220;
	offsetProfile.twobyte_threshold_max = 11235;
	offsetProfile.twobyte_threshold_divide = 1906;
	offsetProfile.twobyte_threshold_min = 913;
	offsetProfile.seqlenMinLimit4 = 156;

	seqPackSeparate(src, L"c:/test/", profile);
	//seqPack(src, packed_name, profile);
	//two_byte_pack(src, packed_name, profile);
	//multi_pack(src, packed_name, offsetProfile, seqlenProfile, offsetProfile);
	//RLE_simple_pack(src, packed_name);
	//block_pack(src, packed_name, profile);

	int pack_time = (clock() - cl);
		
	cl = clock();

	seqUnpackSeparate(L"c:/test/main", unpackedFilename, L"c:/test/");
	//two_byte_unpack(packed_name, unpackedFilename);
	//multi_unpack(packed_name, unpackedFilename);
	//RLE_simple_unpack(packed_name, unpackedFilename);
	//block_unpack(packed_name, unpackedFilename);
	//seqUnpack(packed_name, unpackedFilename);

	int unpack_time = (clock() - cl);
	//printf("\n Unpacking finished time it took: %d", unpack_time);
	printf("\nTimes %d/%d/%d", pack_time, unpack_time, pack_time + unpack_time);
	uint64_t size_packed = get_file_size_from_wname(packed_name);
	wprintf(L"\n\n   --   RATIO OF PACKED   '%s'   %.2f%%   --\n\n", src, ((double)size_packed / (double)size_org) * 100.0);


	printf("\n\n Comparing files!");

	if (files_equalw(src, unpackedFilename)) {
		printf("\n ****** SUCCESS ****** (equal)\n");
	}
	else {
		exit(1);
	}
}//onefile

//---------------------------------------------------------------------------------------

void test16() {
	
	wchar_t test_filenames[16][100] = { 
		L"rel.pdf",
		L"did.csh",	
		L"tob.pdf",
		L"pazera.exe",
		L"voc.wav",
		L"amb.dll",
        L"bad.cdg",
		L"bad.mp3",
		L"nex.doc",		
		L"aft.htm",
		L"book.txt",
		L"oneseq.txt",
		L"empty.txt",
		L"onechar.txt",
		L"book_med.txt",
		L"repeatchar.txt",		
	};
	
	//wchar_t test_filenames[3][100] = { L"ragg.wav", L"voc_short.wav", L"voc.wav" };

	//43965493 w winsize = 95536
	packProfile profile = getPackProfile();
	profile.rle_ratio = 93;
	profile.twobyte_ratio = 89;
	profile.recursive_limit = 242;
	profile.twobyte_threshold_max = 11951;
	profile.twobyte_threshold_divide = 260;
	profile.twobyte_threshold_min = 3150;
	profile.seqlenMinLimit3 = 80;
	profile.seqlenMinLimit4 = 185;
	profile.blockSizeMinus = 139;
	
	packProfile bestProfile;
	copyProfile(&profile, &bestProfile);
	
	bool unpack = false;
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
			if (unpack) {
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
					exit(1);
				}
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
	profile.rle_ratio = 85;
	profile.twobyte_ratio = 73;
	profile.recursive_limit = 79;
	profile.twobyte_threshold_max = 1180;
	profile.twobyte_threshold_divide = 529;
	profile.twobyte_threshold_min = 81;
	profile.seqlenMinLimit3 = 80;
	profile.seqlenMinLimit4 = 185;
	profile.blockSizeMinus = 139;

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
				exit(1);
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